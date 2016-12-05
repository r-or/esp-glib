/*
 * esp-glib.h
 */

#ifndef __ESP_GLIB_H__
#define __ESP_GLIB_H__

#include "stdint.h"
#include "user_config.h"

#ifndef VERBOSE
    #define VERBOSE 0
#endif

/***********************************************
 * CONFIG
 ***********************************************/

// Animation
#define GLIB_ANIM_MIN_DELAY_MS          33


/***********************************************
 * DRIVER SELECTION
 ***********************************************/

#ifdef DISP_SSD1322
    #include "ssd1322.h"

    #define GLIB_DISP_COL_LOWER         0
    #define GLIB_DISP_COL_UPPER         (SSD1322_SEGMENTS * 8 - 1)
    #define GLIB_DISP_ROW_LOWER         0
    #define GLIB_DISP_ROW_UPPER         (SSD1322_ROWS - 1)
    #define GLIB_FBSIZE_INT32           (SSD1322_SEGMENTS * SSD1322_ROWS)
    #define GLIB_PIX_INT32              (32 / SSD1322_PIXDEPTH)
    #define GLIB_PIX_INT8               (8 / SSD1322_PIXDEPTH)
    #define GLIB_MAX_BRIGHTNESS         (SSD1322_MAX_BRIGHTNESS)
    #define GLIB_MAX_CHARS              (SSD1322_MAX_CHARS)

    #define GLIB_DISP_COLS              ((GLIB_DISP_COL_UPPER - GLIB_DISP_COL_LOWER) + 1)
    #define GLIB_DISP_ROWS              ((GLIB_DISP_ROW_UPPER - GLIB_DISP_ROW_LOWER) + 1)
    #define GLIB_DISP_COLS_INT32        (GLIB_DISP_COLS / GLIB_PIX_INT32)
#endif


/**
 * @brief The glib_chars_in_fb struct
 */
struct glib_chars_in_fb {
    uint32_t *last_char;
    uint32_t *last_word;            /// last 'space' character
    uint32_t chars[GLIB_MAX_CHARS];
};

/**
 * @brief The glib_window struct
 * LOGICAL layout (unit: pixels)
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
 * @brief The glib_window_phy struct
 * PHYSICAL layout (unit: segments, or 32 bit wide 'pixel words')
 * SEGS - 1 >= seg_right > seg_left   >= 0
 * ROWS - 1 >= row_top   > row_bottom >= 0
 */
struct glib_window_phy {
    uint8_t seg_left;
    uint8_t seg_right;
    uint8_t row_bottom;
    uint8_t row_top;
};

/**
 * text draw mode
 */
typedef enum {
    GLIB_DM_TEXT,                   /// linebreak on words (delimiter: SPACE), no new page
    GLIB_DM_TEXT_CLR,               ///                                      , clear on new page
    GLIB_DM_TEXT_LINE_FORCE,        /// instead of linebreak, cut remainder & display '...'
    GLIB_DM_FREE                    /// no auto linebreak at all
} glib_draw_mode;

/**
 * specifies objects used in glib
 */
typedef enum {
    GLIB_OS_ALL,                 // tag/clear whole framebuffer
    GLIB_OS_STAGED,              // clear whole fb, only tag part last updated | TODO!
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


/***********************************************
 * DRIVER IF: Implementation in display driver
 ***********************************************/

/**
 * @brief glib_draw read-only transformations + draws to a buffer
 * @param to_buf buffer to draw to, i.e. framebuffer
 * @param x_ul origin: left
 * @param y_ul origin: top
 * @param bitmap the bitmap to draw
 * @param height height of bitmap
 * @param width width of bitmap
 * @param args draw args; only read-only ones are being considered
 * @return zero if success
 */
uint8_t
glib_draw(uint32_t *const to_buf, const uint16_t x_ul, const uint16_t y_ul, uint32_t *const bitmap,
          const uint16_t height, const uint16_t width, const glib_draw_args args);

void
glib_setpix(uint32_t *const seg, uint8_t id, const uint8_t value);

uint8_t
glib_getpix(const uint32_t *const seg, const uint8_t id);

int16_t
glib_row_log2phys(const int16_t log_row);

int16_t
glib_col_log2phys(const int16_t log_col);

struct glib_window_phy
glib_region_log2phys(const struct glib_window *const log_region);

void
glib_tag_upd_reg_log(const struct glib_window *const region);

void
glib_update_gram(uint32_t *const framebuffer);

void
glib_set_brightness(const uint8_t value);

void
glib_set_enable(const uint8_t enable);

void
glib_clear_disp(const uint32_t pattern);

void
glib_reset_display(void);

void
glib_init_display(void);


/***********************************************
 * Implementation in esp-glib
 ***********************************************/

/**
 * @brief glib_clear_fb clear selected fb region only (specified by obj)
 * @param obj specify object to clear
 */
void
glib_clear_fb(const glib_object_specifier obj);

/**
 * @brief glib_clear_fb_toss_anim clear selected fb region only (specified by obj)
 * @param dir direction
 * @param hold_frames # of frames to wait until actual anim starts
 * @param acceleration how fast animation speed will pick up
 */
void
glib_clear_fb_toss_anim(const glib_anim_direction dir, const uint16_t hold_frames, const uint16_t acceleration,
                        void (*toss_cb)(void));

/**
 * @brief glib_clear_disp_fadeout_anim clear display (NOT framebuffer)
 * @param bg_col color to fade to (wip)
 * @param hold_frames
 * @param acceleration
 */
void
glib_clear_disp_fadeout_anim(const uint32_t bg_col, const uint16_t hold_frames, const uint16_t acceleration,
                             void (*fadeout_cb)(void));

/**
 * @brief glib_draw_rect draw rectangle to framebuffer
 * @param border
 * @param pattern color/pattern of frame
 */
void
glib_draw_rect(const struct glib_window *const border, const uint32_t pattern);

/**
 * @brief glib_draw_bitmap see glib_draw(.)
 * @param x_ul
 * @param y_ul
 * @param address address in flash relative to assets address
 * @param height
 * @param width
 * @param args
 * @return zero if success
 */
uint8_t
glib_draw_bitmap(const uint16_t x_ul, const uint16_t y_ul, uint32_t address,
                 const uint16_t height, const uint16_t width, const glib_draw_args args);

/**
 * @brief glib_transform performs transformation ON buf (only uses glib_draw_args which are not read-only)
 * @param buf
 * @param height
 * @param width
 * @param dim dim pixel by this (4bit) value
 * @param args
 * @return zero if success
 */
uint8_t
glib_transform(uint32_t *const buf, const uint16_t height, const uint16_t width, const uint8_t dim,
               const glib_draw_args args);

/**
 * @brief glib_anim_toss_away
 * @param dir
 * @param region
 * @param hold_frames
 * @param acceleration
 */
void
glib_anim_toss_away(const glib_anim_direction dir, const struct glib_window *const region,
                    const uint16_t hold_frames, const uint16_t acceleration, void (*toss_cb)(void));

/**
 * @brief glib_translate translation of region in framebuffer by (x, y)
 * @param region
 * @param x
 * @param y
 * @return zero if success
 */
uint8_t
glib_translate(const struct glib_window *const region, const int16_t x, const int16_t y);

/**
 * @brief glib_print prints a zero-terminated string relative to the origin set with glib_set_textbox(.)
 * Encoding: ASCII; UTF-8-escaping: u+xxxx or U+xxxxxxxx
 * @param string zero-terminated
 * @param x_l origin: top left, x-value
 * @param y_asc char baseline (see https://en.wikipedia.org/wiki/Typeface#Font_metrics)
 * @param args draw-args which can apply to fonts
 * @param x_l_re after rendering, return position of right-most coordinate of last char; x-value
 * @param y_asc_re
 * @return zero if nothing had to be skipped
 */
uint8_t
glib_print(const uint8_t* string, const uint16_t x_l, const uint16_t y_asc,
           const glib_draw_args args, uint16_t *x_l_re, uint16_t *y_asc_re);

/**
 * , utf8-encoding
 * relative to the origin set with ssd1322_set_textbox(.)
 */
/**
 * @brief glib_print_utf8 prints a zero-terminated string
 * Encoding: UTF-8. See glib_print(.)
 * @param utf8string
 * @param x_l
 * @param y_asc
 * @param args
 * @param x_l_re
 * @param y_asc_re
 * @return
 */
uint8_t
glib_print_utf8(const uint32_t* utf8string, const uint16_t x_l, const uint16_t y_asc,
                const glib_draw_args args, uint16_t *x_l_re, uint16_t *y_asc_re);

/**
 * @brief glib_set_textbox select region to print text to
 * @param region
 */
void
glib_set_textbox(const struct glib_window *const region);

/**
 * @brief glib_clear_tb_txt_state reset txt positioning to the ALREADY SELECTED (!) textbox
 */
void
glib_clear_tb_txt_state(void);

/**
 * @brief glib_set_cursor positioning relative to textbox
 * @param x_l
 * @param y_asc
 */
void
glib_set_cursor(const uint16_t x_l, const uint16_t y_asc);

/**
 * @brief glib_set_mode
 * @param dm
 */
void
glib_set_mode(const glib_draw_mode dm);

/**
 * @brief glib_set_background
 * @param pattern
 */
void
glib_set_background(const uint32_t pattern);  // TODO: test

/**
 * @brief glib_set_anim_delay_ms
 * @param delay
 */
void
glib_set_anim_delay_ms(const uint16_t delay);

/**
 * @brief glib_fb2gram glib_update_gram(.)
 */
void
glib_fb2gram(void);

/**
 * @brief glib_reset calls glib_reset_display()
 */
void
glib_reset(void);

/**
 * @brief glib_init calls glib_init_display()
 */
void
glib_init(void);

#endif
