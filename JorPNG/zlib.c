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
  zlib_stream.DICTID = read_bytes(sizeof(zlib_stream.DICTID), &bitstream);
  zlib_stream.DATA = data + (sizeof(zlib_stream.CMF) + sizeof(zlib_stream.FLG) + sizeof(zlib_stream.DICTID));
  zlib_stream.ADLER32 = 0;
  
  print_stream_info(&zlib_stream);

  Window window;
  size_t size = LZ77(CINFO(zlib_stream.CMF));
  create_window(&window, size);

  inflate_block(&bitstream, &window);

  free(window.window);
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
",
CM(stream->CMF) == 8 ? "Deflate" : "ERROR",
LZ77(CINFO(stream->CMF)),
zlib_compression_levels[FLEVEL(stream->FLG)]
// todo add adler
);
}
