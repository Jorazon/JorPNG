#pragma once

#include <stdint.h>

unsigned long update_adler32(unsigned long adler, unsigned char* buf, size_t len);
