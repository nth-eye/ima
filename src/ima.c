#include "ima.h"

#define CLAMP(x, min, max)  (x = x < min ? min : (x > max ? max : x))

static const int16_t step_size[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

static const int step_adjust[8] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
};

static void next_step_idx(IMA *state, uint8_t sample)
{
    state->step_idx += step_adjust[sample & 0x7];

    CLAMP(state->step_idx, 0, 88);
}

uint8_t ima_encode_step(IMA *state, int16_t sample)
{
    int32_t ss      = step_size[state->step_idx];
    int32_t diff    = sample - state->last;
    uint8_t code    = 0x0;

    if (diff < 0) {
        diff = -diff;
        code = 0x8;
    }

    if (diff >= ss) {
        diff -= ss;
        code |= 0x4;
    }

    if (diff >= ss >> 1) {
        diff -= ss >> 1;
        code |= 0x2;
    }

    if (diff >= ss >> 2)
        code |= 0x1;

    state->last = ima_decode_step(state, code);

    return code;
}

int16_t ima_decode_step(IMA *state, uint8_t code)
{
    int32_t ss      = step_size[state->step_idx];
    int32_t diff    = (((code & 0x7) * 2 + 1) * ss) >> 3;

    if (code & 0x8)
        diff = -diff;

    int32_t sample  = state->last + diff;

    CLAMP(sample, -32768, 32767);

    next_step_idx(state, code);

    return sample;
}

void ima_init(IMA *state)
{
    state->step_idx = 0;
    state->last     = 0;
}

void ima_encode(IMA *state, int16_t *in, uint8_t *out, size_t len, bool header)
{
    if (header) {
        out[0] = state->last >> 8;
        out[1] = state->last & 0xff;
        out[2] = state->step_idx & 0xff;
        out[3] = 0;
    }

    size_t even_len = len & ~1;

    for (size_t i = 0; i < even_len; i += 2) {

        uint8_t msb = ima_encode_step(state, in[i]     >> 4);
        uint8_t lsb = ima_encode_step(state, in[i + 1] >> 4);

        *out++ = (msb << 4) | lsb;
    }
}

// Input length doesn't include header length (4 bytes)
void ima_decode(IMA *state, uint8_t *in, int16_t *out, size_t len, bool header)
{
    size_t offset = 0;

    if (header) {
        state->last     = in[1] | (in[0] << 8);
        state->step_idx = in[2];
        offset          = 4;
    }

    for (size_t i = offset; i < len + offset; ++i) {
        *out++ = ima_decode_step(state, in[i] >> 4);
        *out++ = ima_decode_step(state, in[i] & 0xf);
    }
}
