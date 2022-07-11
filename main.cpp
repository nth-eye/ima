#include <stdio.h>
#include "ima.h"

int main() 
{
    FILE *in = fopen("../../audio_2.raw", "rb");

    if (!in) {
        puts("Couldn't open audio.pcm");
        return 1;
    }

    FILE *out = fopen("../../audio_2.ima", "w+b");

    if (!out) {
        puts("Couldn't open audio_2.ima");
        fclose(in);
        return 1;
    }

    Ima     state;
    int16_t samples[2];
    uint8_t byte;
    size_t  cnt;
    size_t  total = 0;

    state.init();

    do {
        cnt = fread(samples, 2, 2, in);

        state.encode(samples, &byte, 2, false);

        fwrite(&byte, 1, 1, out);

        ++total;

    } while (cnt == 2);

    printf("Total 16-bit samples processed: %lu \n", total * 2);

    fclose(in);
    fclose(out);
}
