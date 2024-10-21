#include "inflate.h"
#include "bitstream.h"

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

uint8_t btypes[][39] = {
  "Non-compressed block",
  "Compression with fixed Huffman codes",
  "Compression with dynamic Huffman codes"
};

int inflate_block(Bitstream* stream, Window* window) {
  int bfinal = read_bits(1, stream);  // 1 if this is the final block
  int btype = read_bits(2, stream);   // 2-bit block type
  
  printf("Block type: %s (BTYPE=%d%d)\n", btypes[btype], (btype >> 1) & 1, btype & 1);

  if (btype == 0) {
    // Uncompressed block
    skip_to_next_byte(stream);
    int len = read_bytes(2, stream);  // block length
    int nlen = read_bytes(2, stream); // one's complement of len
    if ((len ^ nlen) != 0xFFFF) {
      printf("Invalid uncompressed block length!\n");
      return -1;
    }
    copy_uncompressed_data(len, stream, window);
  }
  else if (btype == 1) {
    // Fixed Huffman codes
    decode_fixed_huffman_block(stream, window);
  }
  else if (btype == 2) {
    // Dynamic Huffman codes
    printf("Dynamic Huffman codes not implemented!\n");
    decode_dynamic_huffman_block(stream, window);
  }
  else {
    printf("Invalid block type!\n");
    return -1;
  }

  // Return continue to the next block if this was not the last block
  return (bfinal == 0);
}
