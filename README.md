## DECODER

decode `.wav`, `.mp3` file to PCM format (int16_t)

---

## STRUCTS

### `struct wav_header`

contains `.wav` header information

refrerences
* [WAVEFORMAT structure(mmreg.h)][1]
* [riffmci.pdf][2]

<details>
<summary>struct wav_header</summary>

``` c
struct wav_header {
  // RIFF
  char chunk_id[4];
  int chunk_size;
  char format[4];

  // FMT
  char subchunk1_id[4];
  int subchunk1_size;
  short audio_format;
  short num_channels;
  int sample_rate;
  int byte_rate;
  short block_align;
  short bits_per_sample;

  // DATA
  char subchunk2_id[4];
  int subchunk2_size;
};
```
</details>


## FUNCTIONS

### `int16_t *wav_decoder(FILE *fp)`

* if decode success return `int16_t *data`
* if decode failed return `int16_t *data = NULL`

---


[1]: https://learn.microsoft.com/en-us/windows/win32/api/mmreg/ns-mmreg-waveformat
[2]: https://www.aelius.com/njh/wavemetatools/doc/riffmci.pdf