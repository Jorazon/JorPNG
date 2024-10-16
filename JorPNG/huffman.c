#include "huffman.h"

#define MAX_BITS 32
#define max_code 8

typedef struct node_struct {
  size_t Len;
  size_t Code;
  struct Node* Parent;
  struct Node* Left;
  struct Node* Right;
} Node;

Node tree[max_code];

void generateHuffmanCode() {
  uint32_t bl_count[MAX_BITS];
  uint32_t next_code[MAX_BITS];
    
  uint32_t code = 0;
  bl_count[0] = 0;
  for (size_t bits = 1; bits <= MAX_BITS; bits++) {
    code = (code + bl_count[bits - 1]) << 1;
    next_code[bits - 1] = code;
  }
  for (size_t n = 0; n <= max_code; n++) {
    size_t len = tree[n].Len;
    if (len != 0) {
      tree[n].Code = next_code[len];
      next_code[len]++;
    }
  }
}

/*
do
  read block header from input stream.
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

typedef struct header_struct {
  uint16_t LEN; // number of data bytes in the bloc
  uint16_t NLEN; // LEN ^ 0xFFFF
} Header;

void processBlock(uint8_t* input, uint8_t* output) {
  Header header;
  memcpy(&header, input, sizeof(Header));

}
