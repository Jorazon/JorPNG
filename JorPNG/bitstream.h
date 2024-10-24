#pragma once

#include <stdint.h>
#include <stdio.h>

#include "adler.h"

typedef struct bitstream_struct {
  uint8_t* buffer;
  uint8_t bit_position;
  size_t byte_position;
  size_t length;
} BitStream;

void init_bitstream(BitStream* stream, uint8_t* buffer, size_t length);

uint32_t read_bits_lsb(size_t num_bits, BitStream* stream);
uint32_t read_bits_msb(size_t num_bits, BitStream* stream);
uint32_t read_huffman_code(size_t code_length, BitStream* stream);
uint32_t read_bytes(size_t count, BitStream * stream);

void skip_to_next_byte(BitStream * stream);

void put_byte(uint8_t byte, BitStream* stream);
void print_bitstream(BitStream* stream, size_t newline_evert_n_bytes);
