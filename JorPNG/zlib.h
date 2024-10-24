#pragma once

#include <stdint.h>
#include <inttypes.h>

#include "bitstream.h"
#include "window.h"
#include "inflate.h"
#include "adler.h"

// https://www.rfc-editor.org/rfc/rfc1950
// https://www.ietf.org/rfc/rfc1951.txt

typedef union {
  struct {
    uint8_t CM : 4;
    uint8_t CINFO : 4;
  };
  uint8_t byte;
} CMF;

typedef union {
  struct {
    uint8_t FCHECK : 5;
    uint8_t FDICT : 1;
    uint8_t FLEVEL : 2;
  };
  uint8_t byte;
} FLG;

typedef struct zlib_stream_struct {
  CMF CMF; // Compression Method and Flags
  FLG FLG; // FLaGs
  uint32_t DICTID;
  uint32_t ADLER32;
} Zlib_Stream;

void process_zlib_stream(uint8_t* data, uint32_t length, BitStream* output);
void print_stream_info(Zlib_Stream* stream);
