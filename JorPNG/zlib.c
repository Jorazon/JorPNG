#include "zlib.h"

void process_zlib_stream(uint8_t* data, uint32_t length) {
  
  Bitstream bitstream = { 0 };
  bitstream.bit_position = 0;
  bitstream.byte_position = 0;
  bitstream.stream = data;
  bitstream.length = length;

  Zlib_Stream zlib_stream = { 0 };
  zlib_stream.CMF = read_bytes(sizeof(zlib_stream.CMF), &bitstream);
  zlib_stream.FLG = read_bytes(sizeof(zlib_stream.FLG), &bitstream);
  if (FDICT(zlib_stream.FLG)) {
    zlib_stream.DICTID = read_bytes(sizeof(zlib_stream.DICTID), &bitstream);
  }
  
  Window window;
  size_t size = LZ77(CINFO(zlib_stream.CMF));
  create_window(&window, size);

  int has_more_blocks = 1;

  while (has_more_blocks) {
    has_more_blocks = inflate_block(&bitstream, &window);
  }

  free(window.window);

  zlib_stream.ADLER32 = read_bytes(sizeof(zlib_stream.ADLER32), &bitstream);

  uint32_t bitcount = (bitstream.length - bitstream.byte_position) * 8 - bitstream.bit_position;
  printf("%u/%u bits processed (%u left)\n", ((bitstream.length * 8) - bitcount), bitstream.length * 8, bitcount);

  //uint32_t adler = update_adler32(1L, uncompressed_buffer, length);

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
Adler-32: %X\n\
",
    CM(stream->CMF) == 8 ? "Deflate" : "ERROR",
    LZ77(CINFO(stream->CMF)),
    zlib_compression_levels[FLEVEL(stream->FLG)],
    stream->ADLER32
  );
}
