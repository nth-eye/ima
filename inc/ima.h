#ifndef IMA_H
#define IMA_H

#include <cstdint>
#include <cstddef>

struct Ima {

    /**
     * @brief Reset codec state to default.
     * 
     */
    void init()
    {
        step_idx = last = 0;
    }

    /**
     * @brief Encode 16-bit PCM samples to IMA/DVI ADPCM. Codec has 
     * 4x compression, so each 16-bit sample will be encoded to nibble
     * (4 bits). Two consecutive samples will be encoded as byte with 
     * first sample being most significant nibble. Additional 4 bytes 
     * must be reserved at output memory for header if used.
     * 
     * @param in Input samples
     * @param out Encoded output bytes
     * @param len Input length in samples, not in bytes
     * @param header Insert header at beginning
     */
    void encode(int16_t *in, uint8_t *out, size_t len, bool header)
    {
        if (header) {
            *out++ = last >> 8;
            *out++ = last & 0xff;
            *out++ = step_idx & 0xff;
            *out++ = 0;
        }
        size_t even_len = len & ~1;

        for (size_t i = 0; i < even_len; i += 2) {
            uint8_t msb = encode_step(in[i]);
            uint8_t lsb = encode_step(in[i + 1]);
            *out++ = (msb << 4) | lsb;
        }
    }

    /**
     * @brief Decode IMA/DVI ADPCM to 16-bit raw PCM. Each byte has 2 
     * encoded samples and will be decoded as two int16_t's. First 4 bytes
     * are interpreted as header if that parameter is true. 
     * 
     * @param in Encoded input bytes
     * @param out Decoded output samples
     * @param len Input length in bytes, not in samples
     * @param header Is header present at beginning
     */
    void decode(uint8_t *in, int16_t *out, size_t len, bool header)
    {
        size_t i = 0;

        if (header) {
            last        = in[1] | (in[0] << 8);
            step_idx    = in[2];
            i           = 2;
        }
        size_t end = len + i;

        for (; i < end; ++i) {
            *out++ = decode_step(in[i] >> 4);
            *out++ = decode_step(in[i] & 0xf);
        }
    }

    /**
     * @brief Single encoding step.
     * 
     * @param sample Sample to encode
     * @return Encoded nibble (4 bits)
     */
    uint8_t encode_step(int16_t sample)
    {
        int32_t ss      = step_size[step_idx];
        int32_t diff    = sample - last;
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
        
        last = decode_step(code);

        return code;
    }

    /**
     * @brief Single decoding step.
     * 
     * @param code Encoded byte, contains 2 samples
     * @return Decoded PCM sample 
     */
    int16_t decode_step(uint8_t code)
    {
        int32_t ss = step_size[step_idx];
        int32_t diff = (((code & 0x7) * 2 + 1) * ss) >> 3;
        if (code & 0x8)
            diff = -diff;
        next_step_idx(code);
        return clamp(int32_t(last) + diff, -32768, 32767);
    }
private:
    void next_step_idx(uint8_t sample)
    {
        step_idx += step_adjust[sample & 0x7];
        step_idx = clamp(step_idx, 0, 88);
    }

    template<class T>
    static T clamp(T x, int min, int max)  
    {
        return x < min ? min : (x > max ? max : x);
    }

    static constexpr int16_t step_size[89] = {
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
    static constexpr int step_adjust[8] = { 
        -1, -1, -1, -1, 2, 4, 6, 8,
    };

    int step_idx;
    int16_t last;
};

#endif
