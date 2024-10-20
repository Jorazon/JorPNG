#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bitstream.h"
#include "inflate.h"

#define MAX_BITS 15

// Huffman tree node
typedef struct HuffmanNode {
  int symbol;             // The symbol (character or value)
  int is_leaf;            // 1 if this is a leaf node, 0 otherwise
  struct HuffmanNode* left;  // Left child (for bit 0)
  struct HuffmanNode* right; // Right child (for bit 1)
} HuffmanNode;

// Huffman tree
typedef struct HuffmanTree {
  HuffmanNode* root;   // Pointer to the root node of the tree
  int num_symbols;     // Number of symbols in the Huffman tree
} HuffmanTree;

void copy_uncompressed_data(int len, Bitstream* stream, Window* window);
void decode_fixed_huffman_block(Bitstream* stream, Window* window);
void decode_dynamic_huffman_block(Bitstream* stream, Window* window);
