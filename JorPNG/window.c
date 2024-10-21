#include "window.h"

// initialize passed window. rememeber to free
void create_window(Window* window, size_t size) {
  window->window_pos = 0;
  window->size = size;
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
  window->window_pos = (window->window_pos + 1) % window->size;
  //putchar(byte);  // Write to stdout or save to buffer
  put_byte(byte, window->output);
  printf("Outputting 0x%02X (%u) ", byte, byte);
  for (size_t j = 0; j < 8; j++) {
    printf("%d", byte >> (7 - j) & 1);
  }
  printf("\n");
}

// Copy from sliding window based on length and distance
void copy_from_window(int length, int distance, Window* window) {
  size_t src_pos = (window->window_pos - distance + window->size) % window->size;
  for (int i = 0; i < length; i++) {
    // when the window_pos reaches the end, it wraps around to the beginning.
    uint8_t byte = window->window[src_pos];
    output_byte(byte, window);
    src_pos = (src_pos + 1) % window->size;
  }
  printf("Copied %d..%d\n", distance, distance + length);
}