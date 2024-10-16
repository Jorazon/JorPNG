#pragma once

// http://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html
// http://www.libpng.org/pub/png/book/chapter11.html
// https://www.w3.org/TR/png/#11Chunks

#pragma region chunk types
#define typeFromName(a,b,c,d) (((uint32_t)(a) << 24) + ((uint32_t)(b) << 16) + ((uint32_t)(c) << 8) + (uint32_t)(d))
// Critical chunks
#define IHDR typeFromName('I','H','D','R') // Image header
#define PLTE typeFromName('P','L','T','E') // Palette for color type 3
#define IDAT typeFromName('I','D','A','T') // Image data
#define IEND typeFromName('I','E','N','D') // Image trailer (no data)
// Ancillary chunks
// Transparency information
#define tRNS typeFromName('t','R','N','S') // Transparency
// Color space information
#define cHRM typeFromName('c','H','R','M') // Primary chromaticities and white point
#define gAMA typeFromName('g','A','M','A') // Image gamma
#define iCCP typeFromName('i','C','C','P') // Embedded ICC profile
#define sBIT typeFromName('s','B','I','T') // Significant bits
#define sRGB typeFromName('s','R','G','B') // Standard RGB color space
#define cICP typeFromName('c','I','C','P') // Coding-independent code points for video signal type identification
#define mDCv typeFromName('m','D','C','v') // Mastering Display Color Volume
#define cLLi typeFromName('c','L','L','i') // Content Light Level Information
// Textual information
#define tEXt typeFromName('t','E','X','t') // Textual data
#define zTXt typeFromName('z','T','X','t') // Compressed textual data
#define iTXt typeFromName('i','T','X','t') // International textual data
// Miscellaneous information
#define bKGD typeFromName('b','K','G','D') // Background color
#define hIST typeFromName('h','I','S','T') // Image histogram
#define pHYs typeFromName('p','H','Y','s') // Physical pixel dimensions
#define sPLT typeFromName('s','P','L','T') // Suggested palette
#define eXIf typeFromName('e','X','I','f') // Exchangeable Image File (Exif) Profile
// Time stamp information
#define tIME typeFromName('t','I','M','E') // Image last-modification time
//Animation information
#define acTL typeFromName('a','c','T','L') // Animation Control Chunk
#define fcTL typeFromName('f','c','T','L') // Frame Control Chunk
#define fdAT typeFromName('f','d','A','T') // Frame Data Chunk
#pragma endregion chunk types

// https://learn.microsoft.com/en-us/cpp/preprocessor/pack?view=msvc-170
#pragma pack(push, 1)
// http://www.libpng.org/pub/png/spec/1.2/PNG-Structure.html#Chunk-layout
// PNG Chunk structure
typedef struct png_chunk_struct {
  uint32_t length;
  uint32_t chunk_type;
  uint8_t* data;
  uint32_t crc;
} png_chunk;

// IHDR structure
typedef struct png_IHDR_struct {
  uint32_t width;
  uint32_t height;
  uint8_t bit_depth;
  uint8_t color_type;
  uint8_t compression_method;
  uint8_t filter_method;
  uint8_t interlace_method;
} png_IHDR;
// pHYs structure
typedef struct png_pHYs_struct {
  uint32_t ppuX; // Pixels per unit, X axis
  uint32_t ppuY; // Pixels per unit, Y axis
  // Unit specifier
  // 0 Aspect ratio
  // 1 Pixels per metre
  uint8_t unit; 
} png_pHYs;
// sRGB structure
typedef struct png_sRGB_struct {
  // Rendering intent
  // 0 Perceptual
  // 1 Relative colorimetric
  // 2 Saturation
  // 3 Absolute colorimetric
  uint8_t rendering_intent;
  // https://www.w3.org/TR/png/#srgb-standard-colour-space
} png_sRGB;
// cHRM structure
typedef struct png_cHRM_struct {
  uint32_t white_pointX;
  uint32_t white_pointY;
  uint32_t redX;
  uint32_t redY;
  uint32_t greenX;
  uint32_t greenY;
  uint32_t blueX;
  uint32_t blueY;
} png_cHRM;
// gAMA structure
typedef struct png_gAMA_struct {
  uint32_t gamma; // Value of the exponent of a gamma transfer function multiplied by 100000
} png_gAMA;

typedef struct png_color_16_struct {
  uint8_t index;
  uint16_t red;
  uint16_t green;
  uint16_t blue;
  uint16_t gray;
} png_color_16;
#pragma pack(pop)

/*
Naming convention
bLOb  <-- 32 bit chunk type code represented in text form
||||
|||+- Safe-to-copy bit is 1 (lowercase letter; bit 5 is 1)
||+-- Reserved bit is 0     (uppercase letter; bit 5 is 0)
|+--- Private bit is 0      (uppercase letter; bit 5 is 0)
+---- Ancillary bit is 1    (lowercase letter; bit 5 is 1)
*/
