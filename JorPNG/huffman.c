#include "huffman.h"

// Function prototypes

void insert_symbol(HuffmanTree* tree, int symbol, int code, int length);
HuffmanNode* create_node(int symbol, int is_leaf);

void copy_uncompressed_data(int len, BitStream* stream, Window* window) {
  // Read and output 'len' bytes of uncompressed data
  for (int i = 0; i < len; i++) {
    uint8_t byte = read_bytes(1, stream);// stream->buffer[stream->byte_position];
    output_byte(byte, window);  // Output the byte to your decompression buffer
  }
}

// Builds the Huffman tree based on symbol code lengths
void build_huffman_tree(HuffmanTree* tree, int* lengths, int num_symbols) {
  int bl_count[MAX_BITS + 1] = { 0 };  // Number of codes of each length
  int next_code[MAX_BITS + 1] = { 0 }; // Next available code for each length

  // Step 1: Count the number of codes for each code length
  for (int i = 0; i < num_symbols; i++) {
    if (lengths[i] > 0) {
      bl_count[lengths[i]]++;
    }
  }

  // Step 2: Calculate the starting code for each length
  int code = 0;
  next_code[0] = 0;  // No codes with length 0
  for (int bits = 1; bits <= MAX_BITS; bits++) {
    code = (code + bl_count[bits - 1]) << 1;
    next_code[bits] = code;
  }

  // Step 3: Build the tree by inserting symbols based on their lengths and codes
  tree->root = create_node(-1, 0);  // Create an empty root node
  tree->num_symbols = num_symbols;

  for (int i = 0; i < num_symbols; i++) {
    int len = lengths[i];
    if (len > 0) {
      int symbol_code = next_code[len]++;
      insert_symbol(tree, i, symbol_code, len);
    }
  }
}

// Function to print the Huffman tree for debugging
void print_huffman_tree(HuffmanNode* node, int depth) {
  if (node == NULL) return;

  if (node->is_leaf) {
    printf("Symbol: %d Length: %d Code: ", node->symbol, node->Len);
    for (size_t i = 0; i < node->Len; i++) {
      printf("%d", node->Code >> (node->Len - 1 - i) & 1);
    }
    printf(" Depth: %d\n", depth);
  }
  else {
    print_huffman_tree(node->left, depth + 1);
    print_huffman_tree(node->right, depth + 1);
  }
}

// Inserts a symbol into the Huffman tree based on its code and length
void insert_symbol(HuffmanTree* tree, int symbol, int code, int length) {
  HuffmanNode* current = tree->root;

  // Traverse the tree for each bit in the code
  for (int i = length - 1; i >= 0; i--) {
    int bit = (code >> i) & 1;

    if (bit == 0) {
      // Traverse to the left node (create it if it doesn't exist)
      if (!current->left) {
        current->left = create_node(-1, 0);  // Internal node
      }
      current = current->left;
    }
    else {
      // Traverse to the right node (create it if it doesn't exist)
      if (!current->right) {
        current->right = create_node(-1, 0);  // Internal node
      }
      current = current->right;
    }
  }

  // Once at the leaf, assign the symbol
  current->symbol = symbol;
  current->Len = length;
  current->Code = code;
  current->is_leaf = 1;  // Mark as a leaf node
}

// Creates a new Huffman node
HuffmanNode* create_node(int symbol, int is_leaf) {
  HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
  if (!node) {
    fprintf(stderr, "Failed to allocate memory for Huffman tree node");
    return NULL;
  }
  node->symbol = symbol;
  node->is_leaf = is_leaf;
  node->left = NULL;
  node->right = NULL;
  return node;
}

static const int length_base[] = { 3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258 };
static const int length_extra_bits[] = { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0 };

// Decode a length from the Huffman code
int decode_length(int symbol, BitStream* stream) {
  if (symbol < 257 || symbol > 285) {
    fprintf(stderr, "Error: Invalid length symbol %d\n", symbol);
    return -1;
  }

  int length = length_base[symbol - 257];  // Base length for this symbol

  // Read extra bits if needed
  if (length_extra_bits[symbol - 257] > 0) {
    int extra = read_bits_lsb(length_extra_bits[symbol - 257], stream);
    length += extra;
  }

  return length;
}

static const int distance_base[] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577 };
static const int distance_extra_bits[] = { 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13 };

// Decode a distance from the Huffman code
int decode_distance(int symbol, BitStream* stream) {

  if (symbol < 0 || symbol > 29) {
    fprintf(stderr, "Error: Invalid distance symbol %d\n", symbol);
    return -1;
  }

  int distance = distance_base[symbol];  // Base distance for this symbol

  // Read extra bits if needed
  if (distance_extra_bits[symbol] > 0) {
    int extra = read_bits_lsb(distance_extra_bits[symbol], stream);
    distance += extra;
  }

  return distance;
}

// Table for extra bits for length codes
static int length_extra_bits_fixed[] = { 0,0,0,0,1,1,2,2,3,3,4,4,5,5,0 };

// Decode length from Huffman code
int decode_fixed_length(int litlen, BitStream* stream) {
  if (litlen < 257 || litlen > 285) {
    printf("Invalid length code!\n");
    return -1;
  }

  int length = length_base[(litlen - 257)];
  int extra = length_extra_bits_fixed[litlen - 257];
  return length + read_bits_lsb(extra, stream);
}

// Decode distance using fixed Huffman codes
int decode_fixed_distance(BitStream* stream) {
  int dist_code = read_bits_lsb(5, stream);

  int distance = distance_base[dist_code];
  int extra_bits = distance_extra_bits[dist_code];
  distance += read_bits_lsb(extra_bits, stream);

  return distance;
}

int decode_fixed_huffman_literal(BitStream* stream) {
  int code = read_bits_lsb(7, stream);  // Initially, read the first 7 bits

  if (code <= 23) {
    return code + 256;  // Return literal length code (256-279)
  }
  else if (code >= 24 && code <= 255) {
    return code - 48;   // Return literal byte (0-143)
  }
  else if (code >= 280 && code <= 287) {
    return code - 280 + 144;  // Literal byte (144-255)
  }

  printf("Invalid Huffman code!\n");
  return -1;
}

#define num_symbols 288
void decode_fixed_huffman_block(BitStream* stream, Window* window) {
  int lengths[num_symbols] = { 0 };

  for (size_t i = 0; i < num_symbols; i++) {
    int length = 0;
    if ((i >= 0 && i <= 143) || (i >= 280 && i <= 287)) {
      length = 8;
    }
    else if (i >= 144 && i <= 255) {
      length = 9;
    }
    else {
      length = 7;
    }
    lengths[i] = length;
  }

  HuffmanTree fixed_tree;

  build_huffman_tree(&fixed_tree, lengths, num_symbols);

  //print_huffman_tree(fixed_tree.root, 0);

  while (1) {
    // Huffman tree for fixed codes: literals 0-255, end-of-block, lengths 3-258, distances 1-32
    //int litlen = decode_fixed_huffman_literal(stream);  // Decode using fixed Huffman codes

    int litlen = decode_huffman_symbol(&fixed_tree, stream);

    if (litlen < 256) {
      // Literal byte, output it directly
      output_byte(litlen, window);
    }
    else if (litlen == 256) {
      // End of block
      return;
    }
    else {
      // Length-distance pair, decode and copy
      int length = decode_fixed_length(litlen, stream);
      int distance = decode_fixed_distance(stream);
      copy_from_window(length, distance, window);
    }
  }
}

// Decode a symbol from the Huffman tree
int decode_huffman_symbol(HuffmanTree* tree, BitStream* stream) {
  HuffmanNode* node = tree->root;
  int bit_count = 0;  // To track how many bits have been read

  // Traverse the tree bit by bit
  while (!node->is_leaf) { // is_leaf may not be necessary as non leaves have symbol -1
    uint32_t bit = read_bits_lsb(1, stream);
    if (bit == -1) {
      fprintf(stderr, "Error: Failed to read bit from stream.\n");
      return -1;
    }

    bit_count++;
    printf("Read bit: %d\n", bit);

    if (bit == 0) {
      node = node->left;
    }
    else {
      node = node->right;
    }

    if (node == NULL) {
      fprintf(stderr, "Error: Invalid Huffman tree traversal at bit %d.\n", bit_count);
      return -1;
    }
  }

  printf("Decoded symbol: %d (%#X %c) after %d bits\n", node->symbol, node->symbol, node->symbol, bit_count);
  return node->symbol;
}

void decode_compressed_data(HuffmanTree* literal_length_tree, HuffmanTree* distance_tree, BitStream* stream, Window* window) {
  int symbol;

  // Decode each symbol from the literal/length tree
  while ((symbol = decode_huffman_symbol(literal_length_tree, stream)) != 256) {  // 256 is the end-of-block symbol
    if (symbol < 256) {
      // It's a literal byte, output it
      output_byte((uint8_t)symbol, window);
    }
    else {
      // It's a length-distance pair, decode the length and distance
      int length = decode_length(symbol, stream);
      /*
      int distance = decode_huffman_symbol(distance_tree, stream);
      distance = decode_distance(distance, stream);
      */
      int distance = decode_distance(symbol, stream);

      // Copy the previous data from the sliding window
      copy_from_window(length, distance, window);
    }
  }
}

void decode_dynamic_huffman_block(BitStream* stream, Window* window) {
  // Step 1: Read the number of literal/length and distance codes
  int HLIT = read_bits_lsb(5, stream) + 257;  // Number of literal/length codes (257-286)
  int HDIST = read_bits_lsb(5, stream) + 1;   // Number of distance codes (1-32)
  int HCLEN = read_bits_lsb(4, stream) + 4;   // Number of code length codes (4-19)

  // Step 2: Read the code lengths for the code length alphabet
  int code_length_order[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
  int code_length_lengths[19] = { 0 };  // Code lengths for the code length alphabet

  for (int i = 0; i < HCLEN; i++) {
    code_length_lengths[code_length_order[i]] = read_bits_lsb(3, stream);  // Read 3-bit code lengths
  }

  // Step 3: Build Huffman tree for the code length alphabet
  HuffmanTree code_length_tree;
  build_huffman_tree(&code_length_tree, code_length_lengths, 19);

  // Step 4: Decode literal/length and distance code lengths using the code length tree
  int literal_length_lengths[288] = { 0 };  // Array for literal/length code lengths
  int distance_lengths[32] = { 0 };  // Array for distance code lengths

  int i = 0;
  while (i < HLIT + HDIST) {
    int symbol = decode_huffman_symbol(&code_length_tree, stream);  // Decode a symbol from the code length tree
    if (symbol <= 15) {
      // Symbols 0-15 represent literal lengths directly
      if (i < HLIT) {
        literal_length_lengths[i] = symbol;
      }
      else {
        distance_lengths[i - HLIT] = symbol;
      }
      i++;
    }
    else if (symbol == 16) {
      // Repeat the last length 3-6 times
      int repeat_length = 3 + read_bits_lsb(2, stream);  // Read 2 extra bits (3-6 repeats)
      int last_length = (i < HLIT) ? literal_length_lengths[i - 1] : distance_lengths[i - HLIT - 1];

      for (int j = 0; j < repeat_length && i < HLIT + HDIST; j++) {
        if (i < HLIT) {
          literal_length_lengths[i] = last_length;
        }
        else {
          distance_lengths[i - HLIT] = last_length;
        }
        i++;
      }
    }
    else if (symbol == 17) {
      // Repeat a zero length 3-10 times
      int repeat_length = 3 + read_bits_lsb(3, stream);  // Read 3 extra bits (3-10 repeats)
      for (int j = 0; j < repeat_length && i < HLIT + HDIST; j++) {
        if (i < HLIT) {
          literal_length_lengths[i] = 0;
        }
        else {
          distance_lengths[i - HLIT] = 0;
        }
        i++;
      }
    }
    else if (symbol == 18) {
      // Repeat a zero length 11-138 times
      int repeat_length = 11 + read_bits_lsb(7, stream);  // Read 7 extra bits (11-138 repeats)
      for (int j = 0; j < repeat_length && i < HLIT + HDIST; j++) {
        if (i < HLIT) {
          literal_length_lengths[i] = 0;
        }
        else {
          distance_lengths[i - HLIT] = 0;
        }
        i++;
      }
    }
  }

  // Step 5: Build the literal/length and distance Huffman trees
  HuffmanTree literal_length_tree;
  build_huffman_tree(&literal_length_tree, literal_length_lengths, HLIT);

  HuffmanTree distance_tree;
  build_huffman_tree(&distance_tree, distance_lengths, HDIST);

  printf("-- code_length_tree --\n");
  print_huffman_tree(code_length_tree.root, 0);
  printf("-- literal_length_tree --\n");
  print_huffman_tree(literal_length_tree.root, 0);
  printf("-- distance_tree --\n");
  print_huffman_tree(distance_tree.root, 0);

  // Step 6: Decode the actual compressed data
  decode_compressed_data(&literal_length_tree, &distance_tree, stream, window);
}
