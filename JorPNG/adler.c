#include "adler.h"

/* https://www.ietf.org/rfc/rfc1950.txt */

#define BASE 65521 /* largest prime smaller than 65536 */

/*
   Update a running Adler-32 checksum with the bytes buf[0..len-1]
 and return the updated checksum. The Adler-32 checksum should be
 initialized to 1.

 Usage example:

   uint32_t adler = 1L;

   while (read_buffer(buffer, length) != EOF) {
     adler = update_adler32(adler, buffer, length);
   }
   if (adler != original_adler) error();
*/
uint32_t update_adler32(uint32_t adler, uint8_t* buf, size_t len) {
  uint32_t s1 = adler & 0xffff;
  uint32_t s2 = (adler >> 16) & 0xffff;

  for (size_t n = 0; n < len; n++) {
    s1 = (s1 + buf[n]) % BASE;
    s2 = (s2 + s1) % BASE;
  }
  return (s2 << 16) + s1;
}
