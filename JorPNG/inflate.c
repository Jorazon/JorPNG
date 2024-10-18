#include "inflate.h"

uint16_t read_bit(Bitstream* stream) {
  uint16_t bit = stream->stream[stream->byte_position] >> stream->bit_position;
  stream->bit_position++;
  if (stream->bit_position == 8) {
    stream->byte_position++;
    stream->bit_position = 0;
  }
  return bit;
}

uint16_t read_bits(size_t count, Bitstream* stream) {
  uint16_t value = 0;
  for (size_t i = 0; i < count; ++i) {
    value |= (read_bit(stream) << i);
  }
  return value;
}

uint16_t read_bytes(size_t count, Bitstream* stream) {
  uint16_t value = 0;
  for (size_t i = 0; i < count; ++i) {
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

/*
do
  read block header from input stream->
  if stored with no compression
    skip any remaining bits in current partially processed byte
    read LEN and NLEN (see next section)
    copy LEN bytes of data to output
  otherwise
    if compressed with dynamic Huffman codes
      read representation of code trees (see subsection below)
    loop (until end of block code recognized)
      decode literal/length value from input stream
      if value < 256
        copy value (literal byte) to output stream
      otherwise
        if value = end of block (256)
          break from loop
        otherwise (value = 257..285)
          decode distance from input stream
          move backwards distance bytes in the output stream, and copy length bytes from this position to the output stream.
    end loop
while not last block
*/

void decode_fixed_huffman_block(Bitstream* stream, Window* window);
void decode_dynamic_huffman_block(Bitstream* input);
void copy_uncompressed_data(int len, Bitstream* stream, Window* window);
void output_byte(uint8_t byte, Window* window);
void copy_from_window(int length, int distance, Window* window);

int inflate_block(uint8_t* input, uint8_t* output) {
  Bitstream in_stream = { 0, 0, input };
  int bfinal = read_bits(1, &in_stream);  // 1 if this is the final block
  int btype = read_bits(2, &in_stream);   // 2-bit block type

  Window window;

  create_window(&window, WINDOW_SIZE);

  if (btype == 0) {
    // Uncompressed block
    skip_to_next_byte(&in_stream);
    int len = read_bytes(2, &in_stream);  // block length
    int nlen = read_bytes(2, &in_stream); // one's complement of len
    if ((len ^ nlen) != 0xFFFF) {
      printf("Invalid uncompressed block length!\n");
      return -1;
    }
    copy_uncompressed_data(len, &in_stream, &window);
  }
  else if (btype == 1) {
    // Fixed Huffman codes
    decode_fixed_huffman_block(&in_stream, &window);
  }
  else if (btype == 2) {
    // Dynamic Huffman codes
    printf("Dynamic Huffman codes not implemented!\n");
    decode_dynamic_huffman_block(&in_stream);
  }
  else {
    printf("Invalid block type!\n");
    return -1;
  }

  if (bfinal) {
    // This was the last block, exit the loop
    return 0;
  }

  return 1;  // Continue to the next block
}

void decode_fixed_huffman_block(Bitstream* stream, Window* window) {
  // Huffman tree for fixed codes: literals 0-255, end-of-block, lengths 3-258, distances 1-32
  int litlen = decode_fixed_huffman_literal(stream);  // Decode using fixed Huffman codes
  if (litlen < 256) {
    // Literal byte, output it directly
    output_byte(litlen, window);
  }
  else if (litlen == 256) {
    // End of block
    return;
  }
  else {
    // Length-distance pair, decode and copy
    int length = decode_length(litlen, &stream);
    int distance = decode_distance(&stream);
    copy_from_window(length, distance, window);
  }
}

int decode_fixed_huffman_literal(Bitstream* stream) {
  int code = read_bits(7, stream);  // Initially, read the first 7 bits

  if (code <= 23) {
    return code + 256;  // Return literal length code (256-279)
  }
  else if (code >= 24 && code <= 255) {
    return code - 48;   // Return literal byte (0-143)
  }
  else if (code >= 280 && code <= 287) {
    return code - 280 + 144;  // Literal byte (144-255)
  }

  printf("Invalid Huffman code!\n");
  return -1;
}

void decode_dynamic_huffman_block(Bitstream* input) {
  return;
}

// Output a literal byte to the decompressed data
void output_byte(uint8_t byte, Window* window) {
  window->window[window->window_pos] = byte;
  window->window_pos = (window->window_pos + 1) % WINDOW_SIZE;
  putchar(byte);  // Write to stdout or save to buffer
}

// Copy from sliding window based on length and distance
void copy_from_window(int length, int distance, Window* window) {
  for (int i = 0; i < length; i++) {
    int src_pos = (window->window_pos - distance + WINDOW_SIZE) % WINDOW_SIZE;
    uint8_t byte = window->window[src_pos];
    output_byte(byte, window);
  }
}

// Table for extra bits for length codes
int length_extra_bits[] = { 0,0,0,0,1,1,2,2,3,3,4,4,5,5,0 };
int base_lengths[] = { 3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31 };
// Distance code tables (base and extra bits)
int distance_base[] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193 };
int distance_extra_bits[] = { 0,0,0,0,1,1,2,2,3,3,4,4,5,5 };

// Decode length from Huffman code
int decode_length(int litlen, Bitstream* input) {
  if (litlen < 257 || litlen > 285) {
    printf("Invalid length code!\n");
    return -1;
  }

  int length_base = base_lengths[litlen - 257];
  int extra_bits = length_extra_bits[litlen - 257];
  return length_base + read_bits(extra_bits, input);
}

// Decode distance using fixed Huffman codes
int decode_distance(Bitstream* input) {
  int dist_code = read_bits(5, input);

  int distance = distance_base[dist_code];
  int extra_bits = distance_extra_bits[dist_code];
  distance += read_bits(extra_bits, input);

  return distance;
}

void copy_uncompressed_data(int len, Bitstream* stream, Window* window) {
  // Read and output 'len' bytes of uncompressed data
  for (int i = 0; i < len; i++) {
    uint8_t byte = stream->stream[stream->byte_position];
    output_byte(byte, window);  // Output the byte to your decompression buffer
    stream->byte_position++;
  }
}

void create_window(Window* window, size_t size) {
  window->window_pos = 0;
  window->window = (uint8_t*)malloc(size * sizeof(uint8_t));
  if (!window->window) {
    fprintf(stderr, "Failed to create window!\n");
  }
}
