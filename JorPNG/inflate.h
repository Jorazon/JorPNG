#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitstream.h"
#include "window.h"
#include "huffman.h"

int inflate_block(BitStream* stream, Window* window);
void test_inflate();
