#pragma once

#include <stdint.h>

uint32_t update_adler32(uint32_t adler, uint8_t* buf, size_t len);
