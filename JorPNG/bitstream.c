#include "bitstream.h"

uint8_t read_bit(Bitstream* stream) {
  int pos = stream->byte_position * 8 + stream->bit_position;
  printf("Reading bit %lu/%lu\n", pos, stream->length * 8);
  if (pos > 80) {
    int yamom = 69;
  }
  if (stream->byte_position >= stream->length) {
    printf("Error: Reading past bistream end!\n");
    return -1;
  }
  uint8_t bit = stream->stream[stream->byte_position] >> stream->bit_position & 1;
  stream->bit_position++;
  if (stream->bit_position == 8) {
    stream->byte_position++;
    stream->bit_position = 0;
  }
  return bit;
}

uint32_t read_bits(uint32_t count, Bitstream* stream) {
  uint32_t value = 0;
  for (uint32_t i = 0; i < count; ++i) {
    value |= (read_bit(stream) << i);
  }
  return value;
}

uint32_t read_bytes(uint32_t count, Bitstream* stream) {
  uint32_t value = 0;
  for (uint32_t i = 0; i < count; ++i) {
    value |= (stream->stream[stream->byte_position] << (i * 8));
    stream->byte_position++;
  }
  return value;
}

void skip_to_next_byte(Bitstream* stream) {
  if (stream->bit_position > 0) {
    stream->bit_position = 0;
    stream->byte_position++;
  }
}

void skip_bytes(uint32_t count, Bitstream* stream) {
  stream->bit_position = 0;
  stream->byte_position += count;
}

void put_byte(uint8_t byte, Bitstream* stream) {
  printf("Writing byte %u/%u\n", stream->byte_position, stream->length);
  stream->stream[stream->byte_position] = byte;
  stream->byte_position++;
}

void print_bitstream(Bitstream* stream) {
  printf("Bitstream content:\n");
  for (size_t i = 0; i < stream->length; i++) {
    printf("%02X ", stream->stream[i]);
  }
  uint32_t adler = adler32(stream->stream, stream->length);
  printf("\nAdler-32: %08X\n", adler);
}
