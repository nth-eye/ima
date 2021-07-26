#ifndef ima_H
#define ima_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
#define extern "C" {
#endif

typedef struct {
    int     step_idx;
    int16_t last;
} IMA;

void ima_init(IMA *state);
void ima_encode(IMA *state, int16_t *in, uint8_t *out, size_t len, bool header);
void ima_decode(IMA *state, uint8_t *in, int16_t *out, size_t len, bool header);

uint8_t ima_encode_step(IMA *state, int16_t sample);
int16_t ima_decode_step(IMA *state, uint8_t code);

#ifdef __cplusplus
}
#endif

#endif // ima_H