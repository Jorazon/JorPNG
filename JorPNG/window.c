#include "window.h"

// initialize passed window. rememeber to free
void init_window(Window* window, size_t size, BitStream* output) {
  window->window_pos = 0;
  window->size = size;
  window->output = output;
  window->window = (uint8_t*)malloc(size * sizeof(uint8_t));
  if (!window->window) {
    fprintf(stderr, "Failed to create window!\n");
    return;
  }
  for (size_t i = 0; i < size; i++) {
    window->window[i] = 0;
  }
}

// Output a literal byte to the decompressed data
void output_byte(uint8_t byte, Window* window) {
  window->window[window->window_pos] = byte;
  // when the window_pos reaches the end, it wraps around to the beginning.
  window->window_pos++;
  window->window_pos %= window->size;
  //putchar(byte);  // Write to stdout or save to buffer
  put_byte(byte, window->output);
  printf("Outputting 0x%02X %u 0b", byte, byte);
  for (size_t j = 0; j < 8; j++) {
    printf("%d", byte >> (7 - j) & 1);
  }
  printf("\n");
}

// Copy from sliding window based on length and distance
void copy_from_window(int length, int distance, Window* window) {
  size_t src_pos = (window->window_pos - distance + window->size) % window->size;
  for (int i = 0; i < length; i++) {
    uint8_t byte = window->window[src_pos];
    output_byte(byte, window);
    // when the src_pos reaches the end, it wraps around to the beginning.
    src_pos++;
    src_pos %= window->size;
  }
  printf("Copied %d bytes (%d..%d)\n", length, distance, distance + length);
}