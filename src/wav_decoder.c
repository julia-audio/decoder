#include "wav_decoder.h"

struct wav_header read_wav_header(FILE *fp) {
  struct wav_header wh;

  // RIFF
  fread(wh.chunk_id, 4, 1, fp);
  fread(&wh.chunk_size, 4, 1, fp);
  fread(wh.format, 4, 1, fp);

  // FMT
  while (1) {
    if (fread(wh.subchunk1_id, 4, 1, fp) != 1) {
      fprintf(stderr, "EOF while searching for 'fmt ' subchunk\n");
      exit(EXIT_FAILURE);
    }

    if (strncmp(wh.subchunk1_id, "fmt ", 4) == 0) {
      break;
    }
  }

  while (1) {
    if (fread(&wh.subchunk1_size, 4, 1, fp) != 1) {
      fprintf(stderr, "EOF while searching for subchunk size\n");
      exit(EXIT_FAILURE);
    }

    if (wh.subchunk1_size == 16) {
      break;
    }
  }

  fread(&wh.audio_format, 2, 1, fp);
  fread(&wh.num_channels, 2, 1, fp);
  fread(&wh.sample_rate, 4, 1, fp);
  fread(&wh.byte_rate, 4, 1, fp);
  fread(&wh.block_align, 2, 1, fp);
  fread(&wh.bits_per_sample, 2, 1, fp);

  if (wh.subchunk1_size > 16) {
    fseek(fp, wh.subchunk1_size - 16, SEEK_CUR);
  }

  // DATA (header)
  fread(wh.subchunk2_id, 4, 1, fp);
  fread(&wh.subchunk2_size, 4, 1, fp);

  return wh;
}

int16_t *pcm_convert(const float *data, size_t num_samples) {
  int16_t *int_data = malloc(sizeof(int) * num_samples);

  if (!int_data)
    return NULL;

  for (int i = 0; i < num_samples; ++i) {
    float sample = data[i];

    if (sample > 1.0f)
      sample = 1.0f;
    if (sample < -1.0f)
      sample = -1.0f;

    int_data[i] = (int)lrintf(sample * 32767.0f);
  }

  return int_data;
}

int16_t *wav_decoder(FILE *fp) {
  rewind(fp);
  struct wav_header wh = read_wav_header(fp);
  size_t num_samples = wh.subchunk2_size / (wh.bits_per_sample / 8);
  int16_t *buffer = malloc(wh.subchunk2_size);

  buffer = NULL;

  if (wh.audio_format == 1 && wh.bits_per_sample == 16) {
    fread(buffer, sizeof(int16_t), num_samples, fp);

    if (!buffer) {
      free(buffer);
      return NULL;
    }
  }

  if (wh.audio_format == 3 && wh.bits_per_sample == 32) {
    float *float_data = malloc(wh.subchunk2_size);
    fread(float_data, sizeof(float), num_samples, fp);

    buffer = pcm_convert(float_data, num_samples);
    free(float_data);
  }

  return buffer;
}
