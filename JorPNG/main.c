#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define __builtin_bswap32(word) (word & 0xff000000) >> 24 | (word & 0xff0000) >> 8 | (word & 0xff00) << 8 | (word & 0xff) << 24

#include "chunk.h"
#include "crc.h"
#include "zlib.h"

// PNG file signature (8 bytes)
// http://www.libpng.org/pub/png/spec/1.2/PNG-Rationale.html#R.PNG-file-signature
const uint8_t png_signature[8] = { 0x89, 'P', 'N', 'G', '\r', '\n', 26, '\n'};

void read_png(const char* filename) {
  FILE* file = fopen(filename, "rb");
  if (!file) {
    printf("Could not open file\n");
    return;
  }

  // Read and verify PNG signature
  uint8_t signature[8];
  fread(signature, 1, 8, file);
  if (memcmp(signature, png_signature, 8) != 0) {
    printf("Not a PNG file\n");
    fclose(file);
    return;
  }

  fprintf(stdout, "PNG file\n");

  // Start reading chunks
  int hasMore = 1;
  while (hasMore) {
    png_chunk chunk = { 0 };

    // Read chunk length (4 bytes)
    fread(&chunk.length, 4, 1, file);
    chunk.length = __builtin_bswap32(chunk.length); // Convert from big-endian

    // Read chunk type (4 bytes)
    fread(&chunk.chunk_type, 4, 1, file);
    
    // Allocate memory for chunk data
    chunk.data = (uint8_t*)malloc(chunk.length);
    
    if (chunk.length != 0) { // if length is 0, don't do data operations
      if (chunk.data) {
        fread(chunk.data, chunk.length, 1, file);
      }
      else {
        printf("Could not allocate memory for chunk\n");
        free(chunk.data);
        fclose(file);
        return;
      }
    }

    // Read chunk CRC (4 bytes)
    fread(&chunk.crc, 4, 1, file);
    chunk.crc = __builtin_bswap32(chunk.crc);

    fprintf(stdout, 
      "%c%c%c%c chunk. Length: %u CRC: %08X\n", 
      chunk.chunk_type & 0xFF, 
      chunk.chunk_type >> 8 & 0xFF, 
      chunk.chunk_type >> 16 & 0xFF,
      chunk.chunk_type >> 24 & 0xFF,
      chunk.length,
      chunk.crc
    );

    uint32_t c = chunk_crc((uint8_t*) &chunk.chunk_type, chunk.data, chunk.length);

    if (chunk.crc != c) {
      fprintf(stderr, "CRC mismatch! %X != %X\n", chunk.crc, c);
      return;
    }

    // chunk type has to be reversed for comparison, but not for crc check, so I do it here.
    chunk.chunk_type = __builtin_bswap32(chunk.chunk_type);
    switch (chunk.chunk_type) {
    case IHDR: { // Header chunk
      png_IHDR ihdr = { 0 };
      memcpy(&ihdr, chunk.data, sizeof(png_IHDR));
      ihdr.width = __builtin_bswap32(ihdr.width);
      ihdr.height = __builtin_bswap32(ihdr.height);
      print_IHDR(&ihdr);
      break;
    }
    case IDAT: {// Data chunk
      // Process compressed image data here
      print_chunk_data(chunk.data, chunk.length);
      //process_zlib_stream(chunk.data, chunk.length);
      break;
    }
    case PLTE: {
      if (chunk.length % 3) {
        fprintf(stderr, "PLTE Palette length not divisible by 3\n");
        fclose(file);
        return;
      }
      break;
    }
    case gAMA: {
      png_gAMA gama = { 0 };
      memcpy(&gama, chunk.data, sizeof(png_gAMA));
      gama.gamma = __builtin_bswap32(gama.gamma);
      print_gAMA(&gama);
      break;
    }
    case sRGB: {
      png_sRGB srgb = { 0 };
      memcpy(&srgb, chunk.data, sizeof(png_sRGB));
      srgb.rendering_intent = __builtin_bswap32(srgb.rendering_intent);
      print_sRGB(&srgb);
      break;
    }
    case pHYs: {
      png_pHYs phys = { 0 };
      memcpy(&phys, chunk.data, sizeof(png_pHYs));
      phys.ppuX = __builtin_bswap32(phys.ppuX);
      phys.ppuY = __builtin_bswap32(phys.ppuY);
      print_pHYs(&phys);
      break;
    }
    case IEND:// End of PNG file
      hasMore = 0;
      break;
    default:
      break;
    }

    // Free allocated memory
    free(chunk.data);
  }

  fclose(file);
}

// Validate crc code
void crcTest() {
  uint8_t data[3] = {'a', 'b', 'c'};
  uint32_t expected = 0x352441C2;
  uint32_t result = crc(data, sizeof(data));
  printf("%X == %X: %s", expected, result, result == expected ? "True" : "False");
}

int main() {
  read_png("your_image.png");
  //crcTest();
  return 0;
}
