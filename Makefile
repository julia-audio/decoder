BUILD_DIR = ./build
TARGET = $(BUILD_DIR)/decoder

PKG_CONFIG_LIBS=$(shell pkg-config --libs glew glfw3 glut)
PKG_CONFIG_CFLAGS=$(shell pkg-config --cflags glew glfw3 glut)

TARGET_FILES 		= src/main.c src/wav_decoder.c
TARGET_HEADERS 	= src/wav_decoder.h

CFLAGS = -Wall -Wextra -pedantic -std=c99 $(PKG_CONFIG_CFLAGS)
LDFLAGS = -lm $(PKG_CONFIG_LIBS)

$(TARGET): $(TARGET_FILES) | $(BUILD_DIR)
		$(CC) $(CFLAGS) $(TARGET_FILES) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR):
		mkdir -pv $(BUILD_DIR)

%.o: %.c $(TARGET_HEADERS)
		$(CC) $(CFLAGS) -c $<

clean:
		rm -rf $(BUILD_DIR)
