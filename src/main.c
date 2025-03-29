#include <stdio.h>

int main(int argc, char *argv[]) {
  char *filename = NULL;

  if (argc < 2) {
    return 0;
  }

  FILE *file = fopen(filename, "rb");
  if (!file) {
    return 1;
  }

  fclose(file);
  return 0;
}
