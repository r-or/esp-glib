#ifndef _HANKEN_LIGHT_19_
#define _HANKEN_LIGHT_19_

#include "stdint.h"

#define _FONT_MAX_CHAR_SIZE_INT8_ 	268
#define _FONT_MAX_CHAR_SIZE_INT32_ 	67
#define _FONT_MAX_CHAR_WIDTH_ 	28
#define _FONT_MAX_CHAR_HEIGHT_ 	25
#define _FONT_MAX_CHAR_ASCENT_ 	24

struct font_info {
    uint32_t font_size;		// size in bytes
    uint8_t pix_depth;		// bits per pixel
    const char *font_name;
    uint8_t font_height;		// height including line spacing
    uint8_t font_ascend;		// usual ascent excluding accents
};

struct char_info {
    int8_t xoffset;		// pixel offset to top left corner
    int8_t yoffset;
    uint8_t advance;
    uint32_t address;
    uint16_t length;		// length in bytes
    uint8_t width;
    uint8_t height;
};

// latin-1 encoding!
uint8_t get_char(struct char_info *const ch_info, const uint8_t ch);

int8_t get_kerning(const uint8_t ch1, const uint8_t ch2);

void get_font_info(struct font_info *const finfo);

#endif