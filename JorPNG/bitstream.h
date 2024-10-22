#pragma once

#include <stdint.h>
#include <stdio.h>

#include "adler.h"

typedef struct bitstream_struct {
  uint8_t* buffer;
  uint8_t bit_position;
  size_t byte_position;
  size_t length;
} Bitstream;

void init_bitstream(Bitstream* stream, uint8_t* buffer, size_t length);
uint32_t read_bits(size_t count, Bitstream * stream);
uint32_t read_bytes(size_t count, Bitstream * stream);
void skip_to_next_byte(Bitstream * stream);
void skip_bytes(size_t count, Bitstream * stream);
void put_byte(uint8_t byte, Bitstream* stream);
void print_bitstream(Bitstream* stream);
