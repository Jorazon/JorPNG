#include "zlib.h"

int FCHECK(CMF cmf, FLG flg) {
  uint16_t value = (cmf.byte * 256 + flg.byte);
  return value % 31 == 0;
}

uint64_t LZ77_window_size(CMF cmf) {
  return 1ULL << (cmf.CINFO + 8);
}

void process_zlib_stream(uint8_t* data, uint32_t length, Bitstream* output) {
  
  BitStream bitstream;
  init_bitstream(&bitstream, data, length);

  Zlib_Stream zlib_stream = { 0 };
  zlib_stream.CMF.byte = read_bytes(sizeof(zlib_stream.CMF), &bitstream);
  zlib_stream.FLG.byte = read_bytes(sizeof(zlib_stream.FLG), &bitstream);
  if (zlib_stream.FLG.FDICT) {
    zlib_stream.DICTID = read_bytes(sizeof(zlib_stream.DICTID), &bitstream);
  }
  
  Window window;
  size_t size = LZ77_window_size(zlib_stream.CMF);
  create_window(&window, size);
  window.output = output;

  int block = 1;
  do {
    printf("Processing Zlib block %d\n", block++);
  } while (inflate_block(&bitstream, &window));

  free(window.window);

  skip_to_next_byte(&bitstream); // is this needed? Yes!

  zlib_stream.ADLER32 = read_bytes(sizeof(zlib_stream.ADLER32), &bitstream);

  uint32_t bitcount = (bitstream.length - bitstream.byte_position) * 8 - bitstream.bit_position;
  printf("%u/%u bits processed (%u left)\n", ((bitstream.length * 8) - bitcount), bitstream.length * 8, bitcount);

  print_stream_info(&zlib_stream);
}

uint8_t zlib_compression_levels[][39] = {
  "Fastest algorithm",
  "Fast algorithm",
  "Default algorithm",
  "Maximum compression, slowest algorithm"
};

void print_stream_info(Zlib_Stream* stream) {
  fprintf(stdout, "\
Compression method: %s\n\
Window size: %llu\n\
Compression level: %s\n\
Adler-32: %08X\n\
",
    stream->CMF.CM == 8 ? "Deflate" : "ERROR",
    LZ77_window_size(stream->CMF),
    zlib_compression_levels[stream->FLG.FLEVEL],
    stream->ADLER32
  );
}
