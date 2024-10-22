#include "bitstream.h"

void init_bitstream(Bitstream* stream, uint8_t* buffer, size_t length) {
  stream->buffer = buffer;
  stream->length = length;
  stream->bit_position = 0;
  stream->byte_position = 0;
}

uint8_t read_bit(Bitstream* stream) {
  size_t pos = stream->byte_position * 8 + stream->bit_position;
  printf("Reading bit %llu/%llu\n", pos, stream->length * 8);
  if (stream->byte_position >= stream->length) {
    printf("Error: Reading past bistream end!\n");
    return -1;
  }
  uint8_t bit = stream->buffer[stream->byte_position] >> stream->bit_position & 1;
  stream->bit_position++;
  if (stream->bit_position == 8) {
    stream->byte_position++;
    stream->bit_position = 0;
  }
  return bit;
}

uint32_t read_bits(size_t count, Bitstream* stream) {
  uint32_t value = 0;
  for (uint32_t i = 0; i < count; ++i) {
    value |= (read_bit(stream) << i);
  }
  return value;
}

uint32_t read_bytes(size_t count, Bitstream* stream) {
  printf("Reading byte %llu/%llu\n", stream->byte_position, stream->length);
  if (stream->byte_position >= stream->length) {
    printf("Error: Reading past bistream end!\n");
    return -1;
  }
  uint32_t value = 0;
  skip_to_next_byte(stream);
  for (uint32_t i = 0; i < count; ++i) {
    value |= (stream->buffer[stream->byte_position + (count - 1) - i] << (i * 8));
  }
  return value;
}

void skip_to_next_byte(Bitstream* stream) {
  if (stream->bit_position > 0) {
    stream->bit_position = 0;
    stream->byte_position++;
  }
}

void skip_bytes(size_t count, Bitstream* stream) {
  stream->bit_position = 0;
  stream->byte_position += count;
}

void put_byte(uint8_t byte, Bitstream* stream) {
  printf("Writing byte %llu/%llu\n", stream->byte_position, stream->length);
  if (stream->byte_position >= stream->length) {
    printf("Error: Writing past bistream end!\n");
    return;
  }
  stream->buffer[stream->byte_position] = byte;
  stream->byte_position++;
}

void print_bitstream(Bitstream* stream) {
  printf("Bitstream content (%llu bytes):\n", stream->length);
  for (size_t i = 0; i < stream->length; i++) {
    printf("%02X ", stream->buffer[i]);
  }
  uint32_t adler = adler32(stream->buffer, stream->length);
  printf("\nAdler-32: %08X\n", adler);
}
