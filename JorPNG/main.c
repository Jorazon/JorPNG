#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

#define __builtin_bswap32(word) (word & 0xff000000) >> 24 | (word & 0xff0000) >> 8 | (word & 0xff00) << 8 | (word & 0xff) << 24

#include "chunk.h"
#include "crc.h"

// PNG file signature (8 bytes)
// http://www.libpng.org/pub/png/spec/1.2/PNG-Rationale.html#R.PNG-file-signature
const unsigned char png_signature[8] = { 0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};

unsigned char compression_levels[4][8] = { "FASTEST", "FAST", "DEFAULT", "MAXIMUM" };
unsigned char color_types[7][21] = { "Grayscale", "", "RGB", "Palette", "grayscale with alpha", "", "RGBA"};

void read_png(const char* filename) {
  FILE* file = fopen(filename, "rb");
  if (!file) {
    printf("Could not open file\n");
    return;
  }

  // Read and verify PNG signature
  unsigned char signature[8];
  fread(signature, 1, 8, file);
  if (memcmp(signature, png_signature, 8) != 0) {
    printf("Not a PNG file\n");
    fclose(file);
    return;
  }

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
    chunk.data = (unsigned char*)malloc(chunk.length);
    
    if (chunk.length) { // if length is 0, don't do data operations
      if (chunk.data) {
        fread(chunk.data, 1, chunk.length, file);
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

    fprintf(stdout, "%c%c%c%c chunk (%08X)\n", chunk.chunk_type & 0xFF, chunk.chunk_type >> 8 & 0xFF, chunk.chunk_type >> 16 & 0xFF, chunk.chunk_type >> 24 & 0xFF, chunk.crc);

    uint32_t c = chunk_crc(&chunk.chunk_type, chunk.data, chunk.length);

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

     fprintf(stdout, "\
PNG Image\n\
Width: %u\n\
Height: %u\n\
Bit depth: %u\n\
Color type: %s\n\
Compression method: %u\n\
Filter method: %u\n\
Interlace method: %u\n\
",
      ihdr.width, ihdr.height, ihdr.bit_depth, color_types[ihdr.color_type], ihdr.compression_method, ihdr.filter_method, ihdr.interlace_method);
      break;
    }
    case IDAT: {// Data chunk
      // Process compressed image data here
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

void crcTest() {
  uint8_t data[3] = {'a', 'b', 'c'};
  uint32_t expected = 0x352441C2;
  uint32_t result = crc(data, sizeof(data));
  printf("%X %X", expected, result);
}

int main() {
  read_png("your_image.png");
  //crcTest();
  return 0;
}
