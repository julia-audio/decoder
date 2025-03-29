#ifndef WAV_DECODER_H
#define WAV_DECODER_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int16_t *wav_decoder(FILE *fp);

#endif
