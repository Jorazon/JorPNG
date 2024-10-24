#include "bitstream.h"

void init_bitstream(BitStream* stream, uint8_t* buffer, size_t length) {
  stream->buffer = buffer;
  stream->length = length;
  stream->bit_position = 0;
  stream->byte_position = 0;
}

int check_stream_oob(BitStream* stream) {
  int oob = stream->byte_position >= stream->length;
  if (oob) {
    fprintf(stderr, "Error: Reading past bistream end!\n");
  }
  return oob;
}

// Move to the next bit
void advance_bit(BitStream* stream) {
  stream->bit_position++;
  if (stream->bit_position >= 8) {
    stream->bit_position = 0; // Reset bit position
    stream->byte_position++;  // Move to the next byte
  }
}

// Read a single bit in LSB-first order from the bit stream
uint8_t read_bit_lsb(BitStream* stream) {
  if (check_stream_oob(stream)) {
    return -1;
  }
  // Get the current byte
  uint8_t current_byte = stream->buffer[stream->byte_position];
  // Extract the bit at the current bit position (LSB first)
  uint8_t bit = (current_byte >> stream->bit_position) & 1;
  printf("Reading bit %llu/%llu: %u (LSB)\n", stream->byte_position * 8 + stream->bit_position, stream->length * 8, bit);
  advance_bit(stream);
  return bit;
}

// Read a specified number of bits (LSB-first) from the bit stream
uint32_t read_bits_lsb(size_t num_bits, BitStream* stream) {
  uint32_t result = 0;
  for (size_t i = 0; i < num_bits; i++) {
    result |= (read_bit_lsb(stream) << i);   // LSB first
  }
  return result;
}

// Read a single bit in MSB-first order from the bit stream
uint8_t read_bit_msb(BitStream* stream) {
  if (check_stream_oob(stream)) {
    return -1;
  }
  // Get the current byte
  uint8_t current_byte = stream->buffer[stream->byte_position];
  // Calculate the current bit index (MSB first)
  uint8_t bit = (current_byte >> (7 - stream->bit_position)) & 1;
  printf("Read bit %llu/%llu: %u (MSB)\n", stream->byte_position * 8 + stream->bit_position, stream->length * 8, bit);
  advance_bit(stream);
  return bit;
}

// Read a specified number of bits (MSB-first) from the bit stream
uint32_t read_bits_msb(size_t num_bits, BitStream* stream) {
  uint32_t result = 0;
  for (size_t i = 0; i < num_bits; i++) {
    result = (result << 1) | read_bit_msb(stream);  // MSB first
  }
  return result;
}

// Helper function to reverse bits of a Huffman code (MSB first to LSB first)
uint32_t reverse_bits(uint32_t code, size_t num_bits) {
  uint32_t reversed = 0;
  for (size_t i = 0; i < num_bits; i++) {
    reversed |= ((code >> i) & 1) << (num_bits - 1 - i);
  }
  return reversed;
}

// Read a Huffman code from the bit stream (with MSB-first order)
uint32_t read_huffman_code(size_t code_length, BitStream* stream) {
  if (check_stream_oob(stream)) {
    return -1;
  }
  // Read the Huffman code as MSB-first
  uint32_t code = read_bits_msb(code_length, stream);
  // Reverse the bits of the code to interpret in LSB-first order
  return reverse_bits(code, code_length);
}

uint32_t read_bytes(size_t count, BitStream* stream) {
  printf("Reading byte %llu/%llu\n", stream->byte_position, stream->length);
  if (check_stream_oob(stream)) {
    return -1;
  }
  uint32_t value = 0;
  skip_to_next_byte(stream);
  for (uint32_t i = 0; i < count; ++i) {
    value |= (stream->buffer[stream->byte_position + (count - 1) - i] << (i * 8));
  }
  stream->byte_position += count;
  return value;
}

void skip_to_next_byte(BitStream* stream) {
  if (stream->bit_position > 0) {
    stream->bit_position = 0;
    stream->byte_position++;
  }
}

void put_byte(uint8_t byte, BitStream* stream) {
  printf("Writing byte %llu/%llu\n", stream->byte_position, stream->length);
  if (stream->byte_position >= stream->length) {
    printf("Error: Writing past bistream end!\n");
    return;
  }
  stream->buffer[stream->byte_position] = byte;
  stream->byte_position++;
}

void print_bitstream(BitStream* stream, size_t newline_every_n_bytes) {
  printf("BitStream content (%llu bytes):\n", stream->length);
  for (size_t i = 0; i < stream->length; i++) {
    printf("%02X ", stream->buffer[i]);
    // print newline every n bytes, except on last byte
    if (newline_every_n_bytes > 0 && (i + 1) % newline_every_n_bytes == 0 && i < stream->length - 1) {
      printf("\n");
    }
  }
  uint32_t adler = adler32(stream->buffer, stream->length);
  printf("\nAdler-32: %08X\n", adler);
}
