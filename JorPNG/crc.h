#pragma once

#include <stdint.h>

//                   00000000001111111111222222222233
//                   01234567890123456789012345678901
#define POLYNOMIAL 0b11101101101110001000001100100000UL
//#define POLYNOMIAL 0xedb88320L

unsigned long chunk_crc(unsigned char* type, unsigned char* buf, int len);
unsigned long crc(unsigned char* buf, int len);
