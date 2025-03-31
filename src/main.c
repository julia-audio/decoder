#include <GL/glew.h>

#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "wav_decoder.h"

void render_text(float x, float y, const char *text) {
  glRasterPos2f(x, y);

  for (const char *c = text; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
  }
}

void draw_header_info(const struct wav_header wh, const char *filename) {
  char buf[128];

  snprintf(buf, sizeof(buf), "FILE_NAME: %s", filename);
  render_text(-0.95f, 0.90f, buf);

  snprintf(buf, sizeof(buf), "CHUNK_ID: %.4s", wh.chunk_id);
  render_text(-0.95f, 0.80f, buf);

  snprintf(buf, sizeof(buf), "CHUNK_SIZE: %d", wh.chunk_size);
  render_text(-0.95f, 0.75f, buf);

  snprintf(buf, sizeof(buf), "FORMAT: %.4s", wh.format);
  render_text(-0.95f, 0.70f, buf);

  // FMT
  snprintf(buf, sizeof(buf), "SUBCHUNK1_ID: %.4s", wh.subchunk1_id);
  render_text(-0.95f, 0.60f, buf);

  snprintf(buf, sizeof(buf), "SUBCHUNK1_SIZE: %d", wh.subchunk1_size);
  render_text(-0.95f, 0.55f, buf);

  snprintf(buf, sizeof(buf), "AUDIO_FORMAT: %d", wh.audio_format);
  render_text(-0.95f, 0.50f, buf);

  snprintf(buf, sizeof(buf), "NUM_CHANNELS: %d", wh.num_channels);
  render_text(-0.95f, 0.45f, buf);

  snprintf(buf, sizeof(buf), "SAMPLE_RATE: %d", wh.sample_rate);
  render_text(-0.95f, 0.40f, buf);

  snprintf(buf, sizeof(buf), "BYTE_RATE: %d", wh.byte_rate);
  render_text(-0.95f, 0.35f, buf);

  snprintf(buf, sizeof(buf), "BLOCK_ALIGN: %d", wh.block_align);
  render_text(-0.95f, 0.30f, buf);

  snprintf(buf, sizeof(buf), "BITS_PER_SAMPLE: %d", wh.bits_per_sample);
  render_text(-0.95f, 0.25f, buf);

  // DATA
  snprintf(buf, sizeof(buf), "SUBCHUNK2_ID: %.4s", wh.subchunk2_id);
  render_text(-0.95f, 0.15f, buf);

  snprintf(buf, sizeof(buf), "SUBCHUNK2_SIZE: %d", wh.subchunk2_size);
  render_text(-0.95f, 0.10f, buf);
}

int main(int argc, char *argv[]) {
  char *filename = NULL;

  if (argc < 2) {
    return EXIT_FAILURE;
  }

  filename = argv[1];

  FILE *file = fopen(filename, "rb");
  if (!file) {
    fprintf(stderr, "Faild to open file %s\n", filename);
    return EXIT_FAILURE;
  }

  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return EXIT_FAILURE;
  }

  glfwWindowHintString(GLFW_WAYLAND_APP_ID, "decoder");
  GLFWwindow *window = glfwCreateWindow(800, 600, "DECODER", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Failed to create window\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);
  glewInit();
  glutInit(&argc, argv);

  struct wav_header wh = read_wav_header(file);
  int16_t *buffer = wav_decoder(file);
  fclose(file);

  if (!buffer) {
    fprintf(stderr, "Failed to decode wav\n");
    return EXIT_FAILURE;
  }

  size_t num_samples = wh.subchunk2_size / (wh.bits_per_sample / 8);

  float *points = malloc(sizeof(float) * num_samples * 2);
  for (size_t i = 0; i < num_samples; ++i) {
    float x = (float)i / (float)num_samples * 2.0f - 1.0f;
    float y = (float)buffer[i] / 32768.0f;
    points[i * 2] = x;
    points[i * 2 + 1] = y;
  }

  free(buffer);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, points);
    glDrawArrays(GL_LINE_STRIP, 0, num_samples);
    glDisableClientState(GL_VERTEX_ARRAY);

    draw_header_info(wh, filename);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  free(points);
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
