#pragma once

#include <stdint.h>

//                   00000000001111111111222222222233
//                   01234567890123456789012345678901
#define POLYNOMIAL 0b11101101101110001000001100100000UL
//#define POLYNOMIAL 0xedb88320L

uint32_t chunk_crc(uint8_t* type, uint8_t* buf, int len);
uint32_t crc(uint8_t* buf, int len);
