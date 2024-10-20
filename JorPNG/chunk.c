#include "chunk.h"

print_chunk_data(uint8_t* data, uint32_t length) {
  printf("Chunk data:\n| ");
  for (size_t i = 0; i < length; i++) {
    printf("%02X ", data[i]);
    for (size_t j = 0; j < 8; j++) {
      printf("%d", data[i] >> (7 - j) & 1);
    }
    printf("%s", !((i + 1) % 6) ? " |\n| " : " | ");
  }
  printf("\n");
  for (size_t i = 0; i < length; i++) {
    for (size_t j = 0; j < 8; j++) {
      printf("%d", (data[i] >> j) & 1);
    }
  }
  printf("\n");
}

uint8_t color_types[][28] = { "Grayscale", "", "Truecolor (RGB)", "Indexed-color (Palette)", "Greyscale with alpha", "", "Truecolor with alpha (RGBA)" };
uint8_t compression_methods[][8] = { "Deflate" };
uint8_t filter_methods[][19] = { "Adaptive filtering" };
uint8_t interlace_methods[][16] = { "No interlace" ,"Adam7 interlace" };
uint8_t rendering_intents[][22] = { "Perceptual", "Relative colorimetric", "Saturation", "Absolute colorimetric" };

void print_IHDR(png_IHDR* ihdr) {
  fprintf(stdout, "\
Dimensions: %u x %u\n\
Bit depth: %u\n\
Color type: %s\n\
Compression method: %s\n\
Filter method: %s\n\
Interlace method: %s\n",
ihdr->width,
ihdr->height,
ihdr->bit_depth,
color_types[ihdr->color_type],
compression_methods[ihdr->compression_method],
filter_methods[ihdr->filter_method],
interlace_methods[ihdr->interlace_method]
);
}

void print_gAMA(png_gAMA* gama) {
  fprintf(stdout, "Gamma: %f\n", (double)gama->gamma / 100000.0);
}

void print_sRGB(png_sRGB* srgb) {
  fprintf(stdout, "Rendering intent: %s\n", rendering_intents[srgb->rendering_intent]);
}

void print_pHYs(png_pHYs* phys) {
  fprintf(stdout, "Pixel aspect ratio: %f\n", phys->ppuX / (double)phys->ppuY);
  if (phys->unit) {
    fprintf(stdout, "Pixel dimensions: %f x %f mm\n", 1000.0 / phys->ppuX, 1000.0 / phys->ppuY);
    fprintf(stdout, "DPI: %f x %f\n", 0.0254 * phys->ppuX, 0.0254 * phys->ppuY);
  }
}
