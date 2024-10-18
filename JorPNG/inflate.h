#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct bitstream_struct {
  uint8_t* stream;
  size_t bit_position;
  size_t byte_position;
} Bitstream;


#define WINDOW_SIZE 32768

typedef struct window_struct {
uint8_t* window;
int window_pos;
} Window;

int inflate_block(uint8_t* input, uint8_t* output);
void create_window(Window* window, size_t size);
