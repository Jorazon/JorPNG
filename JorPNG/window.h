#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitstream.h"

typedef struct window_struct {
  uint8_t* window;
  int window_pos;
  size_t size;
  BitStream* output;
} Window;

void create_window(Window* window, size_t size);
void copy_from_window(int length, int distance, Window* window);
void output_byte(uint8_t byte, Window* window);
