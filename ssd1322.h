// DEFAULTS
// see end of file

// HEADER
#ifndef __SSD1322_H__
#define __SSD1322_H__

#include "stdint.h"
#include "font.h"
#include "assets.h"
#include "user_interface.h"

typedef enum {
    SSD1322_DM_TEXT,                // linebreak on words (delimiter: SPACE), no new page
    SSD1322_DM_TEXT_CLR,            //                                      , clear on new page
    SSD1322_DM_TEXT_LINE_FORCE,     // instead of linebreak, cut remainder & display '...'
    SSD1322_DM_FREE                 // no auto linebreak at all
} ssd1322_draw_mode;

typedef enum {
    SSD1322_DA_NONE     = 0x00,
    SSD1322_DA_FLIPLR   = 0x01,     // flip bitmap horizontically; NOT read-only
    SSD1322_DA_FLIPTB   = 0x02,     // flip bitmap vertically; read-only
    SSD1322_DA_INVERT   = 0x04,     // NOT read-only
    SSD1322_DA_SWENDIAN = 0x08,     // switch endianness; NOT read-only
    // /* require SSD1322_DM_FREE:
    SSD1322_DA_CENTER_Y = 0x10,     // center in textbox vertically; read-only
    SSD1322_DA_CENTER_X = 0x20,     // center in textbox horizontically; read-only
    // */
    SSD1322_DA_CLR      = 0x40,     // clear pixels; supply (single) uint32_t * to a clear value
} ssd1322_draw_args;

typedef enum {
    SSD1322_OS_ALL,                 // tag/clear whole framebuffer
    SSD1322_OS_STAGED,              // clear whole fb, only tag part last updated
    SSD1322_OS_TEXTBOX              // only clear current textbox region
} ssd1322_object_specifier;

/** physical layout
 * SEGS * 8 - 1 >= seg_right > seg_left   >= 0
 * ROWS - 1     >= row_top   > row_bottom >= 0
 */
struct ssd1322_window_phy {
    uint8_t seg_left;
    uint8_t seg_right;
    uint8_t row_bottom;
    uint8_t row_top;
};

/** logical layout
 * SEGS * 8 - 1 >= x_right  > x_left >= 0
 * ROWS - 1     >= y_bottom > y_top  >= 0
 *
 * |---------------------> x
 * | (0,0) (1,0) ...
 * | (0,1) (1,1)
 * |  ...
 * |
 * v
 * y
 */
struct ssd1322_window {
    uint8_t x_left;
    uint8_t x_right;
    uint8_t y_bottom;
    uint8_t y_top;
};

struct ssd1322_chars_in_fb;

void ssd1322_init(void);
void ssd1322_reset(void);
void ssd1322_clear(const uint32_t seg_value, const uint8_t fade_out);
void ssd1322_clear_fb(const uint32_t seg_value, const ssd1322_object_specifier obj);
void ssd1322_clear_txt_state(void);
void ssd1322_send_data(const uint32_t *const qbytes, const uint16_t qbytes_no);
void ssd1322_send_command(const uint8_t *const cmd, const uint8_t cmd_len);
void ssd1322_send_command_list(const uint8_t *const cmd_list, const uint8_t list_len);
void ssd1322_set_area_phy(const struct ssd1322_window_phy *const region);
/**
 * performs transformation ON buf
 * (only uses ssd1322_draw_args which are not read-only)
 * dim: dim pixel by this (4bit) value
 */
uint8_t ssd1322_transform(uint32_t *const buf, const uint16_t height, const uint16_t width, const uint8_t dim,
                          const ssd1322_draw_args args);
/**
 * translation of region in framebuffer by (x, y)
 */
uint8_t ssd1322_translate(const struct ssd1322_window *const region, const int16_t x, const int16_t y);
/**
 * read-only transformations + draws on FB
 */
uint8_t ssd1322_draw(const uint16_t x_ul, const uint16_t y_ul, uint32_t *const bitmap,
                     const uint16_t height, const uint16_t width, const ssd1322_draw_args args);
/**
 * fetches bmp from flash + prepares for drawing
 */
uint8_t ssd1322_draw_bitmap(const uint16_t x_ul, const uint16_t y_ul, uint32_t address,
                            const uint16_t height, const uint16_t width, const ssd1322_draw_args args);
/**
 * draws rectangle
 */
void ssd1322_draw_border(const struct ssd1322_window *const border, const uint32_t pattern);
/**
 * fetches char + prepares for drawing
 */
uint8_t ssd1322_draw_char(const struct char_info *const chi, const struct font_info *const fnt,
                          const uint16_t x_origin, const uint16_t y_ascend, const ssd1322_draw_args args);
/**
 * prints a zero-terminated string, latin1 encoding
 */
uint8_t ssd1322_print(const uint8_t* string, const uint16_t x_l, const uint16_t y_asc,
                      const ssd1322_draw_args args, uint16_t *x_l_re, uint16_t *y_asc_re);
void ssd1322_set_textbox(const struct ssd1322_window *const region);
uint8_t ssd1322_unescape(const uint8_t *string, uint8_t *const target);
void ssd1322_set_cursor(const uint16_t x_l, const uint16_t y_asc);
void ssd1322_set_mode(const ssd1322_draw_mode dm);
void ssd1322_update_gram(void);

/**************
 * CONFIG REGS
 **************/
// general
#define SSD1322_GSCALE_TABLE    0xB8    // 16byte, (0 <= pulse width GSx <= 180); 1 <= x <= 15

#define SSD1322_ENABLE_GST      0x00

#define SSD1322_DEFAULT_GST     0xB9

#define SSD1322_CONTRAST        0xC1    // 2byte command, 0 <= contrast <= 255  //* 127

#define SSD1322_CONTRAST_MASTER 0xC7    // 2byte, 0 <= mcontrast <= 15  //* 15

#define SSD1322_ALL_OFF         0xA4
#define SSD1322_ALL_ON          0xA5
#define SSD1322_ALL_NORMAL      0xA6    //*
#define SSD1322_ALL_INVERSE     0xA7

#define SSD1322_DISP_OFF        0xAE
#define SSD1322_DISP_ON         0xAF

#define SSD1322_WRITE           0x5C
#define SSD1322_READ            0x5D

#define SSD1322_COM_LOCK        0xFD    // 2byte,
    #define SSD1322_COM_LOCK_EN     0x16
    #define SSD1322_COM_UNLOCK      0x12


// hardware config
#define SSD1322_MUX_RATIO       0xCA    // 2byte, 15 <= mux <= 127  //* 127

#define SSD1322_DISP_SLINE      0xA1    // 2byte, 0 <= start line <= 127    //* 0

#define SSD1322_COM_MODE        0xA0    // 3byte, (COM1 | COM2 | ...); (LMODE_DUAL_DIS or LMODE_DUAL_EN)
    #define SSD1322_COM_HOR_INCREM  0x00    //*
    #define SSD1322_COM_VERT_INCREM 0x01
    #define SSD1322_COM_COL_REMAP   0x02
    #define SSD1322_COM_NIB_REMAP   0x04
    #define SSD1322_COM_SCANDIR_REV 0x10
    #define SSD1322_COM_SOE         0x20

    #define SSD1322_LMODE_DUAL_DIS  0x01    //*
    #define SSD1322_LMODE_DUAL_EN   0x11

#define SSD1322_DISP_OFFSET     0xA2    // 2byte, 0 <= vertical offset <= 127   //* 0

#define SSD1322_DISP_CFG1       0xB4    // 3byte, SEG_VREF; ENH_LOW_GS
    #define SSD1322_SEG_VREF_EXT    0xA0
    #define SSD1322_SEG_VREF_INT    0xA2    //*

    #define SSD1322_ENH_LOW_GS_EN   0xFD
    #define SSD1322_ENH_LOW_GS_DIS  0xB5    //*

#define SSD1322_GPIO            0xB5    // 2byte, GPIO0 cfg | GPIO1 cfg
    #define SSD1322_GPIO0_I_DIS     0x00
    #define SSD1322_GPIO0_I_EN      0x01
    #define SSD1322_GPIO0_O_LOW     0x02    //*
    #define SSD1322_GPIO0_O_HIGH    0x03

    #define SSD1322_GPIO1_I_DIS     0x00
    #define SSD1322_GPIO1_I_EN      0x05
    #define SSD1322_GPIO1_O_LOW     0x06    //*
    #define SSD1322_GPIO1_O_HIGH    0x07

#define SSD1322_VPRECHARGE      0xBB    // 2byte, 0 <= vprec <= 31  //* 23

#define SSD1322_VCOMH           0xBE    // 2byte, 0 <= vcomh <= 7   //* 4

#define SSD1322_VDD             0xAB    // 2byte,
    #define SSD1322_VDD_EXT         0x00
    #define SSD1322_VDD_INT         0x01    //*


// timing
#define SSD1322_DISP_CLK_DIV    0xB3    // 2byte, (0 <= div <= 10) | (0 <= Fosc <= 15) << 4

#define SSD1322_PRECHARGE       0xB1    // 2byte, (2 <= phase1 <= 15) | (3 <= phase2 <= 15) << 4

#define SSD1322_PRECHARGE2      0xB6    // 2byte, 0 <= precharge2 <= 15


// addressing
// partial mode
#define SSD1322_PARTIAL         0xA8    // 3byte, 0 <= start row <= 127; 0 <= end row <= 127
#define SSD1322_PARTIAL_OFF     0xA9

#define SSD1322_COL_ADDR        0x15    // 3byte, 0 <= start col <= 127; 0 <= end col <= 119    //* 0;119

#define SSD1322_ROW_ADDR        0x75    // 3byte, 0 <= start row <= 127; 0 <= end row <= 127    //* 0;127

/**************
 * CUSTOM
 **************/
// IO
#define SSD1322_SPI4WIRE        4
#define SSD1322_SPI3WIRE        3

/*
 * OPT: use object (multiple displays)
 *
    typedef struct {
        uint8_t id;
        uint32_t dc_line;
        uint32_t cs_line;
    } display;

*/

#endif

// resolution
#ifndef _SSD1322_MODE_
#define _SSD1322_MODE_          256*64
#define SSD1322_PIXDEPTH        4
#define SSD1322_COL_SEG_START   0x1C    // 28
#define SSD1322_COL_SEG_END     0x5B    // 91
#define SSD1322_ROW_START       0x00    // 0
#define SSD1322_ROW_END         0x3F    // 63
#define SSD1322_SEGMENTS        32
#define SSD1322_ROWS            64
#define SSD1322_MAX_BRIGHTNESS  127
#define SSD1322_FBSIZE_INT32    SSD1322_SEGMENTS * SSD1322_ROWS                             // 2048
#define SSD1322_MAX_CHARS       (((SSD1322_SEGMENTS * 8) / _FONT_MIN_CHAR_WIDTH_) * \
                                    (SSD1322_ROWS / _FONT_MAX_CHAR_HEIGHT_)) + 2            // floor
#endif

// IO modes
#ifndef _SSD1322_IO_        // do not change right now!
#define _SSD1322_IO_            SSD1322_SPI4WIRE
#define _SSD1322_DC_PIN_        BIT2
#define _SSD1322_RESET_PIN_     BIT12
#endif
