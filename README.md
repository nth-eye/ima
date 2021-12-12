# ima

IMA/DVI ADPCM codec in C.

## How to use

```c
IMA ima;

uint8_t encoded[512]; // must be at least sizeof(samples) / 2
int16_t samples[1024];

// ...fill samples...

ima_init(&ima);
ima_encode(&ima, samples, encoded, sizeof(samples), false);

```

Example with reading from file can be found in main.c

## TODO

**_Nothing_**

