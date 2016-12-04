#ifndef __ESP_GLIB_H__
#define __ESP_GLIB_H__

#include "stdint.h"
#include "user_config.h"

#ifndef VERBOSE
    #define VERBOSE 0
#endif

/*
 * DRIVER SELECTION
 */
#ifdef DISP_SSD1322
    #include "ssd1322.h"

    #define GLIB_DISP_COL_LOWER         0
    #define GLIB_DISP_COL_UPPER         (SSD1322_SEGMENTS * 8 - 1)
    #define GLIB_DISP_ROW_LOWER         0
    #define GLIB_DISP_ROW_UPPER         (SSD1322_ROWS - 1)
    #define GLIB_FBSIZE_INT32           (SSD1322_SEGMENTS * SSD1322_ROWS)
    #define GLIB_PIX_INT32              (32 / SSD1322_PIXDEPTH)
    #define GLIB_PIX_INT8               (8 / SSD1322_PIXDEPTH)
    #define GLIB_MAX_BRIGHTNESS         SSD1322_MAX_BRIGHTNESS
    #define GLIB_MAX_CHARS              SSD1322_MAX_CHARS

    #define GLIB_DISP_COLS              ((GLIB_DISP_COL_UPPER - GLIB_DISP_COL_LOWER) + 1)
    #define GLIB_DISP_ROWS              ((GLIB_DISP_ROW_UPPER - GLIB_DISP_ROW_LOWER) + 1)
    #define GLIB_DISP_COLS_INT32        (GLIB_DISP_COLS / GLIB_PIX_INT32)
#endif

// Animation
#define GLIB_ANIM_DELAY_MS          33

typedef enum {
    GLIB_DM_TEXT,                // linebreak on words (delimiter: SPACE), no new page
    GLIB_DM_TEXT_CLR,            //                                      , clear on new page
    GLIB_DM_TEXT_LINE_FORCE,     // instead of linebreak, cut remainder & display '...'
    GLIB_DM_FREE                 // no auto linebreak at all
} glib_draw_mode;

struct glib_chars_in_fb {
    uint32_t *last_char;
    uint32_t *last_word;     // last 'space'
    uint32_t chars[GLIB_MAX_CHARS];
};

/**
 * logical layout (unit: pixels)
 * COL_UPPER >= x_right  > x_left >= COL_LOWER
 * ROW_UPPER >= y_bottom > y_top  >= ROW_LOWER
 *
 * |---------------------> x
 * | (0,0) (1,0) (2,0) ...
 * | (0,1) (1,1)
 * | (0,2)
 * |  ...
 * v
 * y
 */
struct glib_window {
    int16_t x_left;
    int16_t x_right;
    int16_t y_bottom;
    int16_t y_top;
};

/**
 * physical layout (unit: segments, or 32 bit wide 'pixel words')
 * SEGS - 1 >= seg_right > seg_left   >= 0
 * ROWS - 1 >= row_top   > row_bottom >= 0
 */
struct glib_window_phy {
    uint8_t seg_left;
    uint8_t seg_right;
    uint8_t row_bottom;
    uint8_t row_top;
};

typedef enum {
    GLIB_OS_ALL,                 // tag/clear whole framebuffer
    GLIB_OS_STAGED,              // clear whole fb, only tag part last updated
    GLIB_OS_TEXTBOX              // only clear current textbox region
} glib_object_specifier;

typedef enum {
    GLIB_AD_N,
    GLIB_AD_NW,
    GLIB_AD_W,
    GLIB_AD_SW,
    GLIB_AD_S,
    GLIB_AD_SE,
    GLIB_AD_E,
    GLIB_AD_NE
} glib_anim_direction;


typedef enum {
    GLIB_DA_NONE     = 0x00,
    GLIB_DA_FLIPLR   = 0x01,     // flip bitmap horizontically; NOT read-only
    GLIB_DA_FLIPTB   = 0x02,     // flip bitmap vertically; read-only
    GLIB_DA_INVERT   = 0x04,     // NOT read-only
    GLIB_DA_SWENDIAN = 0x08,     // switch endianness; NOT read-only
    // /* require GLIB_DM_FREE:
    GLIB_DA_CENTER_Y = 0x10,     // center in textbox vertically; read-only
    GLIB_DA_CENTER_X = 0x20,     // center in textbox horizontically; read-only
    // */
    GLIB_DA_CLR      = 0x40      // clear pixels; supply (single) uint32_t * to a clear value
} glib_draw_args;


/*
 * INTERFACE: Implementation in display driver
 */

/**
 * read-only transformations + draws on FB
 */
uint8_t glib_draw(uint32_t *const to_buf, const uint16_t x_ul, const uint16_t y_ul, uint32_t *const bitmap,
                  const uint16_t height, const uint16_t width, const glib_draw_args args);
void glib_setpix(uint32_t *const seg, uint8_t id, const uint8_t value);
uint8_t glib_getpix(const uint32_t *const seg, const uint8_t id);
int16_t glib_row_log2phys(const int16_t log_row);
int16_t glib_col_log2phys(const int16_t log_col);
struct glib_window_phy glib_region_log2phys(const struct glib_window *const log_region);
void glib_tag_upd_reg_log(const struct glib_window *const region);
void glib_update_gram(uint32_t *const framebuffer);
void glib_set_brightness(const uint8_t value);
void glib_set_enable(const uint8_t enable);
void glib_clear_disp(const uint32_t pattern);
void glib_reset_display(void);
void glib_init_display(void);

/*
 * API
 */

/*
#define ssd1322_print_default(string) \
    ssd1322_clear_fb(GLIB_OS_ALL); \
    ssd1322_set_textbox(NULL); \
    ssd1322_clear_tb_txt_state(); \
    ssd1322_set_mode(SSD1422_DM_TEXT); \
    ssd1322_print((uint8_t *)string, 0, _FONT_MAX_CHAR_ASCENT_, \
        GLIB_DA_SWENDIAN, NULL, NULL);

#define ssd1322_print_center(string) \
    ssd1322_clear_fb(GLIB_OS_ALL); \
    ssd1322_set_textbox(NULL); \
    ssd1322_clear_tb_txt_state(); \
    ssd1322_set_mode(SSD1422_DM_FREE); \
    ssd1322_print((uint8_t *)string, 0, _FONT_MAX_CHAR_ASCENT_, \
        (glib_draw_args)(GLIB_DA_SWENDIAN | GLIB_DA_CENTER_Y | GLIB_DA_CENTER_X), NULL, NULL);
*/

/**
 * clear selected fb region only (specified by obj)
 */
void glib_clear_fb(const glib_object_specifier obj);

/**
 * clear selected fb region only (specified by obj)
 */
void glib_clear_fb_toss_anim(const glib_anim_direction dir, void (*toss_cb)(void));

/**
 *
 */
void glib_clear_disp_fadeout_anim(const uint8_t ftype, const uint32_t bg, void (*fadeout_cb)(void));

/**
 * draws rectangle
 */
void glib_draw_rect(const struct glib_window *const border, const uint32_t pattern);

/**
 * fetches bmp from flash + prepares for drawing
 */
uint8_t glib_draw_bitmap(const uint16_t x_ul, const uint16_t y_ul, uint32_t address,
                         const uint16_t height, const uint16_t width, const glib_draw_args args);

/**
 * performs transformation ON buf
 * (only uses glib_draw_args which are not read-only)
 * dim: dim pixel by this (4bit) value
 */
uint8_t glib_transform(uint32_t *const buf, const uint16_t height, const uint16_t width, const uint8_t dim,
                       const glib_draw_args args);

/**
 * throws object towards dir
 */
void glib_anim_toss_away(const glib_anim_direction dir, const struct glib_window *const region,
                         void (*toss_cb)(void));

/**
 * translation of region in framebuffer by (x, y)
 */
uint8_t glib_translate(const struct glib_window *const region, const int16_t x, const int16_t y);

/**
 * prints a zero-terminated string, ascii encoding
 * UTF-8-escaping: u+xxxx or U+xxxxxxxx
 * relative to the origin set with ssd1322_set_textbox(.)
 */
uint8_t glib_print(const uint8_t* string, const uint16_t x_l, const uint16_t y_asc,
                   const glib_draw_args args, uint16_t *x_l_re, uint16_t *y_asc_re);

/**
 * prints a zero-terminated string, utf8-encoding
 * relative to the origin set with ssd1322_set_textbox(.)
 */
uint8_t glib_print_utf8(const uint32_t* utf8string, const uint16_t x_l, const uint16_t y_asc,
                        const glib_draw_args args, uint16_t *x_l_re, uint16_t *y_asc_re);

/**
 * select region to print text to
 */
void glib_set_textbox(const struct glib_window *const region);

/**
 * reset txt positioning to the ALREADY SELECTED (!) textbox
 */
void glib_clear_tb_txt_state(void);

/**
 * positioning relative to textbox
 */
void glib_set_cursor(const uint16_t x_l, const uint16_t y_asc);

/**
 * select draw mode
 */
void glib_set_mode(const glib_draw_mode dm);

/**
 *
 */
void glib_set_background(const uint32_t pattern);  // TODO: test

/**
 * calls glib_update_gram(.)
 */
void glib_fb2gram(void);

/**
 * calls glib_reset_display()
 */
void glib_reset(void);

/**
 * calls glib_init_display()
 */
void glib_init(void);

#endif
