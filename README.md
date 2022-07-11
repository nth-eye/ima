# ima

IMA/DVI ADPCM codec in C++.

## How to use

```cpp
Ima ima;

int16_t samples[1024];
uint8_t encoded[512]; // must be at least sizeof(samples) / 4

// ...fill samples...

ima.init();
ima.encode(samples, encoded, sizeof(samples), false);

```

Example with reading from file can be found in main.cpp
