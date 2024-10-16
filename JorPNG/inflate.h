#pragma once

#include <stdint.h>
§§§§§q1
// https://www.rfc-editor.org/rfc/rfc1950
// // https://www.ietf.org/rfc/rfc1951.txt

typedef struct zlib_stream_struct {
  uint8_t CMF; // Compression Method and Flags
  uint8_t FLG; // FLaGs
  uint32_t DICTID;
  uint8_t* DATA;
  uint32_t ADLER32;
} zlib_stream;

// This identifies the compression method used in the file. (Should be 8)
#define CM(CMF) CMF & 0xF
// CINFO is the base-2 logarithm of the LZ77 window size, minus eight. (CINFO = 7 indicates a 32K window size)
#define CINFO(CMF) (CMF >> 4) & 0xF
// Calculates LZ77 window size
#define LZ77(CINFO) 1 << (CINFO + 8)
// Check bits for CMF and FLG. The FCHECK value must be such that CMF and FLG, when viewed as a 16 - bit unsigned integer stored in MSB order(CMF * 256 + FLG), is a multiple of 31.
#define FCHECK(FLG) (FLG) & 0xFF
// Preset dictionary
#define FDIC(FLG) (CMF >> 5) & 0x1
// Compression level
// 0 - fastest algorithm
// 1 - fast algorithm
// 2 - default algorithm
// 3 - maximum compression, slowest algorithm
#define FLEVEL(FLG) (CMF >> 6) & 0x3
