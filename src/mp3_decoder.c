#include "mp3_decoder.h"

int is_id3v1 = 0;
long data_start_pos = 0;

uint32_t decode_syncsafe(uint8_t b[4]) {
  return ((b[0] & 0x7F) << 21) | ((b[1] & 0x7F) << 14) | ((b[2] & 0x7F) << 7) |
         (b[3] & 0x7F);
}

struct ID3v2_header read_idv3_header(FILE *fp) {
  struct ID3v2_header ih;
  uint8_t raw_size[4];

  fread(ih.id3_sig, 3, 1, fp);
  ih.id3_sig[3] = '\0';

  fread(&ih.version_major, 1, 1, fp);
  fread(&ih.version_revision, 1, 1, fp);
  fread(&ih.flags, 1, 1, fp);
  fread(&raw_size, 1, 4, fp);

  ih.size = decode_syncsafe(raw_size);
  data_start_pos = 10 + ih.size;

  return ih;
}

struct mp3_header read_mp3_header(FILE *fp) {
  rewind(fp);
  char signature[4];

  fread(signature, 3, 1, fp);
  signature[3] = '\0';

  if (strncmp(signature, "TAG", 3) == 0) {
    rewind(fp);
    is_id3v1 = 1;
  }

  if (strncmp(signature, "ID3", 3) == 0) {
    rewind(fp);
    struct ID3v2_header id3 = read_idv3_header(fp);
    fseek(fp, 10 + id3.size, SEEK_SET);
  }

  struct mp3_header mh;
  rewind(fp);

  // FRAME SYNC
  int is_sync_success = 0;
  int byte1, byte2;
  while ((byte1 = fgetc(fp)) != EOF) {
    if ((byte1 & 0xFF) == 0xFF) {
      byte2 = fgetc(fp);

      if (byte2 == EOF) {
        exit(EXIT_FAILURE);
      }

      // SYNC SUCCESS
      if ((byte2 & 0xE0) == 0xE0) {
        is_sync_success = 1;

        mh.version = (byte2 & 0x18) >> 3;
        mh.layer = (byte2 & 0x06) >> 1;
        mh.protection_bit = byte2 & 0x01;

        int byte3, byte4;

        byte3 = fgetc(fp);
        byte4 = fgetc(fp);

        mh.bitrate = (byte3 & 0xF0) >> 4;
        mh.samplerate = (byte3 & 0x0C) >> 2;
        mh.has_padding = (byte3 & 0x02) >> 2;
        mh.private_bit = byte3 & 0x01;

        mh.channels = (byte4 & 0xC0) >> 6;
        mh.mode_extension = (byte4 & 0x30) >> 4;
        mh.copyright = (byte4 & 0x08) >> 3;
        mh.original = (byte4 & 0x04) >> 2;
        mh.emphasis = byte4 & 0x03;

        data_start_pos = ftell(fp) - 4;

        break;
      }
    }
  }

  if (!is_sync_success) {
    exit(EXIT_FAILURE);
  }

  return mh;
}

// @TODO MP3(MPEG) TO PCM

int16_t *mp3_decoder(FILE *fp) {
  rewind(fp);
  read_mp3_header(fp);

  fseek(fp, 0, SEEK_END);
  long end_pos = ftell(fp);
  long data_size = end_pos - data_start_pos;

  if (is_id3v1) {
    data_size -= 128;
  }

  fseek(fp, data_start_pos, SEEK_SET);

  int16_t *buffer = malloc(data_size);

  if (!buffer)
    return NULL;

  fread(buffer, 1, data_size, fp);

  return buffer;
}
