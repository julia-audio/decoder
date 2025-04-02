#ifndef MP3_DECODER_H
#define MP3_DECODER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID3v1_HEADER_SIZE = 128

struct ID3v1_header {
  char id3_sig[4];
  char title[31];
  char artist[31];
  char album[31];
  char year[5];
  char comment[29];
  uint8_t zero;
  uint8_t track;
  uint8_t genre;
};

struct ID3v2_header {
  char id3_sig[4];
  uint8_t version_major;
  uint8_t version_revision;
  uint8_t flags;
  uint32_t size;
};

struct mp3_header {
  // byte 2
  uint8_t version;
  uint8_t layer;
  uint8_t protection_bit;

  // byte 3
  uint8_t bitrate;
  uint8_t samplerate;
  uint8_t has_padding;
  uint8_t private_bit;

  // byte 4
  uint8_t channels;
  uint8_t mode_extension;
  uint8_t copyright;
  uint8_t original;
  uint8_t emphasis;
};

int16_t *mp3_decoder(FILE *fp);
struct mp3_header read_mp3_header(FILE *fp);

#endif
