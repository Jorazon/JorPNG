#pragma once

#include <stdint.h>
#include <stdio.h>

typedef struct bitstream_struct {
  uint8_t* stream;
  uint8_t bit_position;
  uint32_t byte_position;
  uint32_t length;
} Bitstream;

uint32_t read_bits(uint32_t count, Bitstream * stream);
uint32_t read_bytes(uint32_t count, Bitstream * stream);
void skip_to_next_byte(Bitstream * stream);
void skip_bytes(uint32_t count, Bitstream * stream);
