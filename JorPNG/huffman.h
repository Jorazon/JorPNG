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
  int Len;             // The code length in bits
  int Code;               // The code
  int is_leaf;            // 1 if this is a leaf node, 0 otherwise
  struct HuffmanNode* left;  // Left child (for bit 0)
  struct HuffmanNode* right; // Right child (for bit 1)
} HuffmanNode;

// Huffman tree
typedef struct HuffmanTree {
  HuffmanNode* root;   // Pointer to the root node of the tree
  int num_symbols;     // Number of symbols in the Huffman tree
} HuffmanTree;

void copy_uncompressed_data(int len, BitStream* stream, Window* window);
void decode_fixed_huffman_block(BitStream* stream, Window* window);
void decode_dynamic_huffman_block(BitStream* stream, Window* window);
int decode_huffman_symbol(HuffmanTree* tree, BitStream* stream);
