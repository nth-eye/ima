#ifndef IMA_H
#define IMA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int     step_idx;
    int16_t last;
} IMA;

/**
 * @brief Reset codec state to default.
 * 
 * @param state Codec context
 */
void ima_init(IMA *state);

/**
 * @brief Encode 16-bit PCM samples to IMA/DVI ADPCM. Codec has 
 * 4x compression, so each 16-bit sample will be encoded to nibble
 * (4 bits). Two consecutive samples will be encoded as byte with 
 * first sample being most significant nibble. Additional 4 bytes 
 * must be reserved at output memory for header if used.
 * 
 * @param state Codec context
 * @param in Input samples
 * @param out Encoded output bytes
 * @param len Input length in samples, not in bytes
 * @param header Insert header at beginning
 */
void ima_encode(IMA *state, int16_t *in, uint8_t *out, size_t len, bool header);

/**
 * @brief Decode dialogic ADPCM to 16-bit raw PCM. Each byte has 2 
 * encoded samples and will be decoded as two int16_t's. First 4 bytes
 * are interpreted as header if that parameter is true.
 * 
 * @param state Codec context
 * @param in Encoded input bytes
 * @param out Decoded output sample
 * @param len Input length in bytes, not in samples, doesn't include header length (4 bytes)
 * @param header Is header present at beginning
 */
void ima_decode(IMA *state, uint8_t *in, int16_t *out, size_t len, bool header);

/**
 * @brief Single encoding step.
 * 
 * @param state Codec context
 * @param sample Sample to encode
 * @return Encoded nibble (4 bits) 
 */
uint8_t ima_encode_step(IMA *state, int16_t sample);

/**
 * @brief Single decoding step
 * 
 * @param state Codec context
 * @param code Encoded byte, contains 2 samples
 * @return Decoded PCM sample 
 */
int16_t ima_decode_step(IMA *state, uint8_t code);

#ifdef __cplusplus
}
#endif

#endif
