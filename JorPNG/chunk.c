#include "chunk.h"

print_chunk_data(uint8_t* data, uint32_t length) {
  printf("Chunk data:\n| ");
  // pretty
  for (size_t i = 0; i < length; i++) {
    printf("%02X ", data[i]);
    for (size_t j = 0; j < 8; j++) {
      printf("%d", data[i] >> (7 - j) & 1);
    }
    printf("%s", !((i + 1) % 6) ? " |\n| " : " | ");
  }
  printf("\nBytes: ");
  // bytes
  for (size_t i = 0; i < length; i++) {
    printf("%02X", data[i]);
  }
  printf("\nBits: ");
  // bits
  for (size_t i = 0; i < length; i++) {
    for (size_t j = 0; j < 8; j++) {
      printf("%d", (data[i] >> j) & 1);
    }
  }
  printf("\n");
}

uint8_t color_types[][28] = { "Grayscale", "", "Truecolor (RGB)", "Indexed-color (Palette)", "Greyscale with alpha", "", "Truecolor with alpha (RGBA)" };
uint8_t color_channels[] = { 1, 0, 3, 1, 2, 0, 4 };
uint8_t compression_methods[][8] = { "Deflate" };
uint8_t filter_methods[][19] = { "Adaptive filtering" };
uint8_t interlace_methods[][16] = { "No interlace" ,"Adam7 interlace" };
uint8_t rendering_intents[][22] = { "Perceptual", "Relative colorimetric", "Saturation", "Absolute colorimetric" };

void print_IHDR(png_IHDR* ihdr) {
  fprintf(stdout, "\
Width: %u pixels\n\
Height: %u pixels\n\
Bit depth: %u bits per channel\n\
Color type: %s (%u)\n\
Compression method: %s (%u)\n\
Filter method: %s (%u)\n\
Interlace method: %s (%u)\n\
Deflated IDAT size: %u bytes\n",
    ihdr->width,
    ihdr->height,
    ihdr->bit_depth,
    color_types[ihdr->color_type],
    ihdr->color_type,
    compression_methods[ihdr->compression_method],
    ihdr->compression_method,
    filter_methods[ihdr->filter_method],
    ihdr->filter_method,
    interlace_methods[ihdr->interlace_method],
    ihdr->interlace_method,
    ihdr->width * ihdr->height * ihdr->bit_depth * color_channels[ihdr->color_type] / 8
  );
}

void print_gAMA(png_gAMA* gama) {
  fprintf(stdout, "Gamma: %f\n", (double)gama->gamma / 100000.0);
}

void print_sRGB(png_sRGB* srgb) {
  fprintf(stdout, "Rendering intent: %s (%u)\n",
    rendering_intents[srgb->rendering_intent],
    srgb->rendering_intent
  );
}

static double M_PER_IN = 0.0254;
static double MM_PER_M = 1000.0;

void print_pHYs(png_pHYs* phys) {
  fprintf(stdout, "Pixel aspect ratio: %f\n", phys->ppuX / (double)phys->ppuY);
  if (phys->unit) {
    fprintf(stdout, "Pixel dimensions: %f x %f mm\n", MM_PER_M / phys->ppuX, MM_PER_M / phys->ppuY);
    fprintf(stdout, "DPI: %f x %f\n", M_PER_IN * phys->ppuX, M_PER_IN * phys->ppuY);
  }
}
