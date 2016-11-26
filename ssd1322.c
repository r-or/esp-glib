#include "user_config.h"
#include "ssd1322.h"
#include "driver/spi.h"
#include "mem.h"

#define XSTR(x) STR(x)  // convert #define into string
#define STR(x) #x

//#define DBG_SHOW_TEXTBOX_BORDER
//#define DBG_SHOW_TEXT_EXTENT

struct ssd1322_chars_in_fb {
    uint8_t *last_char;
    uint8_t *last_word;     // last 'space'
    uint8_t chars[SSD1322_MAX_CHARS];
};

#pragma message "ssd1322 max char buffer size: " XSTR(SSD1322_MAX_CHARS) " bytes"

static uint32_t volatile framebuffer[SSD1322_FBSIZE_INT32] = {0};       // [64][32], row addr x col addr/2 -> 64 x 64*4
static uint32_t background_pattern = 0;
static struct ssd1322_window_phy volatile fb_upd_reg;                   // region to upload with update_gram function
static struct ssd1322_window_phy volatile fb_upd_reg_old;
static uint8_t volatile write_to_gram = 0;

static const struct ssd1322_window_phy default_region_phy = {
    .seg_left = SSD1322_SEGMENTS - 1,
    .seg_right = 0,
    .row_bottom = SSD1322_ROWS - 1,
    .row_top = 0,
};

static const struct ssd1322_window_phy max_region_phy = {
    .seg_left = 0,
    .seg_right = SSD1322_SEGMENTS - 1,
    .row_bottom = 0,
    .row_top = SSD1322_ROWS - 1,
};

// send stuff to GRAM
void ssd1322_send_data(const uint32_t *const qbytes, const uint16_t qbytes_no) {
    uint16_t i = 0;
#if VERBOSE > 1
    os_printf("ssd1322_send_data: sending %lu bytes...\n", qbytes_no * 4);
#endif
#if (_SSD1322_IO_ == SSD1322_SPI4WIRE)
    if (!write_to_gram) {
        uint8_t cmd = SSD1322_WRITE;
        ssd1322_send_command(&cmd, 0);
        write_to_gram = 1;
        gpio_output_set(_SSD1322_DC_PIN_, 0, _SSD1322_DC_PIN_, 0); // set D/C line high
    }
    for (; i < qbytes_no; ++i) {
        spi_txd(HSPI, 32, qbytes[i]);
    }
#elif (_SSD1322_IO_ == SSD1322_SPI3WIRE)
    spi_txd(HSPI, 8, SSD1322_WRITE);
    for (; i < bytes_no; ++i)d
        spi_transaction(HSPI, 9, (uint16_t)(bytes[i]) | 0x100, 0, 0, 0, 0, 0, 0);
#endif
}

// send stuff to cmd register
void ICACHE_FLASH_ATTR ssd1322_send_command(const uint8_t *const cmd, const uint8_t cmd_len) {
    write_to_gram = 0;
#if (_SSD1322_IO_ == SSD1322_SPI4WIRE)
    gpio_output_set(0, _SSD1322_DC_PIN_, _SSD1322_DC_PIN_, 0);  // set D/C line low
    spi_transaction(HSPI, 8, cmd[0], 0, 0, 0, 0, 0, 0);
    os_delay_us(1000);
#if VERBOSE > 1
    os_printf("ssd1322_send_command: sent %x[%d]", cmd[0], cmd_len);
#endif

    uint8_t i = 1;
    if (cmd_len > 1) {
        gpio_output_set(_SSD1322_DC_PIN_, 0, _SSD1322_DC_PIN_, 0); // set D/C line high
        for (; i < cmd_len; ++i) {
            spi_transaction(HSPI, 8, cmd[i], 0, 0, 0, 0, 0, 0);

#if VERBOSE > 1
            os_printf(":%x", cmd[i]);
#endif
        }
    }
#elif (_SSD1322_IO_ == SSD1322_SPI3WIRE)
    spi_transaction(HSPI, 9, (uint16_t)cmd[0], 0, 0, 0, 0, 0, 0);
    if (cmd_len > 1 && cmd_len[1]) {
        for (; i < cmd_len; ++i)
            spi_transaction(HSPI, 9, (uint16_t)cmd[i] | 0x100, 0, 0, 0, 0, 0, 0);
    }
#endif
#if VERBOSE > 1
    os_printf("\n");
#endif
}

void ICACHE_FLASH_ATTR ssd1322_send_command_list(const uint8_t *const cmd_list, const uint8_t list_len) {
    write_to_gram = 0;
    uint8_t i = 0;
#if VERBOSE > 1
    os_printf("ssd1322_send_command_list: sent ");
#endif

#if (_SSD1322_IO_ == SSD1322_SPI4WIRE)
    for (; i < list_len; ++i) {
        gpio_output_set(0, _SSD1322_DC_PIN_, _SSD1322_DC_PIN_, 0);  // set D/C line low
        spi_transaction(HSPI, 8, cmd_list[i], 0, 0, 0, 0, 0, 0);
        os_delay_us(1000);
#if VERBOSE > 1
        os_printf("|%x", cmd_list[i], cmd_list[i + 1]);
#endif

        if (i + 1 < list_len && cmd_list[i + 1] && i + 1 + cmd_list[i + 1] < list_len) {

#if VERBOSE > 1
            os_printf("[%d]", cmd_list[i + 1]);
#endif

            gpio_output_set(_SSD1322_DC_PIN_, 0, _SSD1322_DC_PIN_, 0); // set D/C line high
            uint8_t k = 0;
            for (; k < cmd_list[i + 1]; ++k) {
                spi_transaction(HSPI, 8, cmd_list[i + 1 + k + 1], 0, 0, 0, 0, 0, 0);
                os_delay_us(1000);
#if VERBOSE > 1
                os_printf(":%x", cmd_list[i + 1 + k + 1]);
#endif
            }
            i += cmd_list[i + 1];
        }
        i++;
    }
#elif (_SSD1322_IO_ == SSD1322_SPI3WIRE)
    for (; i < list_len; ++i) {
        spi_transaction(HSPI, 9, cmd[i], 0, 0, 0, 0, 0, 0);
        if (i + 1 < cmd_len && cmd_list[i + 1] && i + 1 + cmd_list[i + 1] < list_len) {
#ifdef VERBOSE
            os_printf("[%d]", cmd_list[i + 1]);
#endif
            uint8_t k = 0;
            for (; k < cmd_list[i + 1]; ++k) {
                spi_transaction(HSPI, 9, cmd[i + 1 + k + 1] | 0x100, 0, 0, 0, 0, 0, 0);
#ifdef VERBOSE
                os_printf(":%x", cmd_list[i + 1 + k + 1]);
#endif
            }
            i += 1 + cmd_list[i + 1];
        }
    }
#endif
#if VERBOSE > 1
    os_printf("\n");
#endif
}

void ICACHE_FLASH_ATTR ssd1322_set_area_phy(const struct ssd1322_window_phy *const region) {
    if (region != NULL) {
        if (SSD1322_ROW_START + region->row_top > SSD1322_ROW_END
                || region->row_bottom > region->row_top
                || SSD1322_COL_SEG_START + region->seg_right * 2 + 1 > SSD1322_COL_SEG_END
                || region->seg_left > region->seg_right) {
            // region outside boundaries
            os_printf("ssd1322_set_area: critical error... area outside boundaries!\n");
#if (VERBOSE > 0)
            os_printf(" ROW_START + area.row_top: %d !<= ROW_END: %d\n"
                      " !>= ROW_START + area.row_bottom: %d\n"
                      " COL_SEG_START + area.segright: %d !<= COL_SEG_END: %d\n"
                      " !>= COL_SEG_START + area.segleft: %d\n",
                      SSD1322_ROW_START + region->row_top, SSD1322_ROW_END,
                      SSD1322_ROW_START + region->row_bottom,
                      SSD1322_COL_SEG_START + region->seg_right * 2 + 1, SSD1322_COL_SEG_END,
                      SSD1322_COL_SEG_START + region->seg_left * 2);
#endif
            return;
        }
        // check if window changed
        if (region->row_bottom == fb_upd_reg_old.row_bottom && region->row_top == fb_upd_reg_old.row_top
                && region->seg_left == fb_upd_reg_old.seg_left && region->seg_right == fb_upd_reg_old.seg_right)
            return;
        fb_upd_reg_old = *region;
        uint8_t commands[] = {
            // col address is segmented in 2bytes or 4 pixels for whatever reason
            SSD1322_COL_ADDR,   2,  SSD1322_COL_SEG_START + region->seg_left * 2,
                                    SSD1322_COL_SEG_START + region->seg_right * 2 + 1,
            SSD1322_ROW_ADDR,   2,  SSD1322_ROW_START + region->row_bottom,
                                    SSD1322_ROW_START + region->row_top
        };
        ssd1322_send_command_list(commands, sizeof(commands) / sizeof(uint8_t));
    } else {
        fb_upd_reg_old = (struct ssd1322_window_phy) {
            .seg_left = 0,
            .seg_right = SSD1322_SEGMENTS - 1,
            .row_bottom = 0,
            .row_top = SSD1322_ROWS - 1,
        };
        uint8_t commands[] = {
            SSD1322_COL_ADDR,   2,  SSD1322_COL_SEG_START, SSD1322_COL_SEG_END,
            SSD1322_ROW_ADDR,   2,  SSD1322_ROW_START, SSD1322_ROW_END
        };
        ssd1322_send_command_list(commands, sizeof(commands) / sizeof(uint8_t));
    }
#if VERBOSE > 1
    os_printf("ssd1322_set_area: col %d ... %d, row %d ... %d\n", SSD1322_COL_SEG_START + fb_upd_reg_old.seg_left * 2,
              SSD1322_COL_SEG_START + fb_upd_reg_old.seg_right * 2 + 1, SSD1322_ROW_START + fb_upd_reg_old.row_bottom,
              SSD1322_ROW_START + fb_upd_reg_old.row_top);
#endif
}


static inline void tag_upd_reg(const struct ssd1322_window_phy *const region) {
    if (region->seg_left < fb_upd_reg.seg_left)
        fb_upd_reg.seg_left = region->seg_left;
    if (region->seg_right > fb_upd_reg.seg_right)
        fb_upd_reg.seg_right = region->seg_right;
    if (region->row_top > fb_upd_reg.row_top)
        fb_upd_reg.row_top = region->row_top;
    if (region->row_bottom < fb_upd_reg.row_bottom)
        fb_upd_reg.row_bottom = region->row_bottom;

#if (VERBOSE > 1)
    os_printf("tag updatereg: NEW => result\n"
              " %d\t=> .segleft = %d\n"
              " %d\t=> .segright = %d\n"
              " %d\t=> .rowtop = %d\n"
              " %d\t=> .rowbottom = %d\n",
              region->seg_left, fb_upd_reg.seg_left, region->seg_right, fb_upd_reg.seg_right,
              region->row_top, fb_upd_reg.row_top, region->row_bottom, fb_upd_reg.row_bottom);
#endif
}

uint8_t ICACHE_FLASH_ATTR ssd1322_draw(const uint16_t x_ul, const uint16_t y_ul, uint32_t *const bitmap,
                     const uint16_t height, const uint16_t width, const ssd1322_draw_args args) {
    if (!height || !width)
        return 0;

    /* SEGMENTATION of SSD1322:
     * seg ->                     1                         2    ... 32
     * pix      1   2    3    4     5     6     7     8
     * bits    3:0|7:4|11:8|15:12|19:16|23:20|27:24|31:28
     * row 1
     *     2
     *    ...
     *     64
     */

    /* TODO: right now it works, but could be way nicer with unions / bitfields:
     *  typedef union {
     *      struct {
     *          uint8_t pixel1: 4;
     *          uint8_t pixel2: 4;
     *          ...
     *          uint8_t pixel8: 4;
     *      } pixels;
     *      uint32_t seg_whole;
     *  } segment;
     */

    // => segmentation decode:
    uint8_t seg_l = (uint8_t)(x_ul / 8);  // floor
    uint8_t seg_l_pix = x_ul % 8;
    uint8_t seg_r = (uint8_t)((x_ul + (width - (width ? 1 : 0))) / 8);
    uint8_t seg_r_pix = (x_ul + width) % 8;

    uint8_t seg_num = seg_r - seg_l + 1;   // segments affected per scanline
    uint16_t pix_idx_y;

    uint32_t *currfb;
    uint32_t *currbuf = bitmap;
    uint8_t chbuf_pix_pad = 0;
    uint8_t curr_seg;
    uint8_t clr = args & SSD1322_DA_CLR;
    uint32_t cmask;

    // tag segments / rows for update
    struct ssd1322_window_phy new_reg = {
       .seg_left = seg_l,
       .seg_right = seg_r,
       .row_top = (uint8_t)y_ul,
       .row_bottom = (uint8_t)(y_ul - height + 1)
    };
    tag_upd_reg(&new_reg);

#if (VERBOSE == 3)
    os_printf("buffer info: @(%d|%d); segl: %d, segr: %d, segs: %d, wid: %d, hei: %d\n",
              x_ul, y_ul, seg_l, seg_r, seg_num, width, height);
    if (!clr) {
        os_printf("bindump:\n");
        uint32_t i;
        for (i = 0; i < (height * width + 15) / 16; ++i)
            os_printf(" 0x%08x\n", bitmap[i]);
        os_printf("\n");
    }
    system_soft_wdt_feed();
    os_printf("fb: 0x%08x; lpad: %d, rpad: %d\n", framebuffer, seg_l_pix, seg_r_pix);
#endif
    pix_idx_y = (args & SSD1322_DA_FLIPTB) ? height - 1 : 0;
    while (1) {
        if (pix_idx_y < 0 || pix_idx_y > SSD1322_ROWS - 1)  // out of y boundaries
            break;
#if (VERBOSE == 4)
        os_printf("y=%d; currbuf (*currbuf) -> currfb\n cseg:", pix_idx_y);
#endif
        for (curr_seg = 0; curr_seg < seg_num; ++curr_seg) {
            if (curr_seg > SSD1322_SEGMENTS - 1) // out of x boundaries
                continue;

            currfb = (uint32_t *)framebuffer + SSD1322_SEGMENTS * (y_ul - pix_idx_y) + seg_l + curr_seg;
#if (VERBOSE == 4)
            os_printf(" 0x%08x (0x%08x) -> 0x%08x |", currbuf, *currbuf, currfb);
#endif

            if (!curr_seg) {                        // 1st segment
                cmask = 0xFFFFFFFF >> (seg_l_pix * SSD1322_PIXDEPTH);
                if (seg_num == 1 && width != 8)
                    cmask &= 0xFFFFFFFF << (32 - seg_r_pix * SSD1322_PIXDEPTH);

                if (clr)
                    *currfb &= ~cmask;        // clear last few pixel

                *currfb |= ((*currbuf << (chbuf_pix_pad * SSD1322_PIXDEPTH)) >> (seg_l_pix * SSD1322_PIXDEPTH)) & cmask;
                if (seg_num == 1 && width != 8)
                    chbuf_pix_pad += ((seg_r_pix ? seg_r_pix : 8) - seg_l_pix);
                else
                    chbuf_pix_pad += 8 - seg_l_pix;
                if (chbuf_pix_pad >= 8) { // increment pointer to buffer, write remainder
#if (VERBOSE == 4)
                    os_printf("^f");
#endif
                    chbuf_pix_pad -= 8;
                    if (!clr)
                        ++currbuf;
                    if (chbuf_pix_pad)
                        *currfb |= (*currbuf >> ((((seg_num == 1) ? seg_r_pix : 32) - chbuf_pix_pad) * SSD1322_PIXDEPTH)) & cmask;
                }
            } else if (curr_seg && curr_seg == seg_num - 1) {   // last segment
                if (clr)
                    *currfb &= (~(0xFFFFFFFF << (32 - seg_r_pix * 4)));   // clear 1st few pixel

                *currfb |= (*currbuf << (chbuf_pix_pad * SSD1322_PIXDEPTH)) & (0xFFFFFFFF << (32 - seg_r_pix * SSD1322_PIXDEPTH));
                chbuf_pix_pad += (seg_r_pix ? seg_r_pix : 8);
                if (chbuf_pix_pad >= 8) { // increment pointer to char buffer, write remainder
#if (VERBOSE == 4)
                    os_printf("^l");
#endif
                    chbuf_pix_pad -= 8;
                    if (!clr)
                        ++currbuf;
                    if (chbuf_pix_pad)
                        *currfb |= (*currbuf >> ((seg_r_pix - chbuf_pix_pad) * SSD1322_PIXDEPTH))
                            & (0xFFFFFFFF << (32 - seg_r_pix * SSD1322_PIXDEPTH));
                }

            } else {    // segment in between
                if (clr)
                    *currfb = 0;
#if (VERBOSE == 4)
                os_printf("^m");
#endif
                *currfb |= (*currbuf << (chbuf_pix_pad * 4));
                if (!clr)
                    ++currbuf;
                if (chbuf_pix_pad)
                    *currfb |= (*currbuf >> (32 - chbuf_pix_pad * 4));
            }
#if (VERBOSE == 3)
            os_printf(" 0x%08x @ 0x%08x", *currfb, currfb);
#endif
        }

#if (VERBOSE == 3 || VERBOSE == 4)
        os_printf("\n" );
        system_soft_wdt_feed();
#endif

        if (args & SSD1322_DA_FLIPTB) {
            if (pix_idx_y == 0)
                break;
            --pix_idx_y;
        } else {
            if (pix_idx_y == height - 1)
                break;
            ++pix_idx_y;
        }

    }
    return 0;
}


inline void setpix(uint32_t *seg, uint8_t id, uint8_t value) {
    // clear pix
    *seg &= ~((uint32_t)0xF << ((32 - SSD1322_PIXDEPTH) - id * SSD1322_PIXDEPTH));
    // set new val
    *seg |= (uint32_t)value << ((32 - SSD1322_PIXDEPTH) - id * SSD1322_PIXDEPTH);
}

inline uint8_t getpix(uint32_t *seg, uint8_t id) {
    return (uint8_t)((*seg << id * SSD1322_PIXDEPTH) >> (32 - SSD1322_PIXDEPTH));
}

// convert logical row to physical row
inline int16_t row_log2phys(int16_t log_row) {
    return SSD1322_ROWS - 1 - log_row;
}

// convert whole logical window
inline struct ssd1322_window_phy region_log2phys(const struct ssd1322_window *const log_region) {
    return (struct ssd1322_window_phy) {
        .seg_left = (uint8_t)(log_region->x_left / 8),
        .seg_right = (uint8_t)(log_region->x_right / 8),
        .row_top = (uint8_t)row_log2phys(log_region->y_top),
        .row_bottom = (uint8_t)row_log2phys(log_region->y_bottom),
    };
}

static inline void dbg_dump_fb(const struct ssd1322_window *region) {
    int16_t row, pix;
    uint8_t pix_id;
    uint32_t *cbuf;
    os_printf("framebuffer dump:\nrow\tpix %d .. %d\n", region->x_left, region->x_right);
    for (row = region->y_top; row <= region->y_bottom; ++row) {
        os_printf("%03d\t", row);
        for (pix = region->x_left; pix <= region->x_right; ++pix) {
            pix_id = pix % 8;
            cbuf = (uint32_t *)framebuffer + SSD1322_SEGMENTS * row_log2phys(row) + pix / 8;
            os_printf("%x", getpix(cbuf, pix_id));
        }
        os_printf("\n");
    }
}

uint8_t ICACHE_FLASH_ATTR ssd1322_transform(uint32_t *const buf, const uint16_t height, const uint16_t width, const uint8_t dim,
                          const ssd1322_draw_args args) {
    uint16_t rowcnt, bufidx;
    uint16_t buflen = (height * width + 7) / 8;
    uint8_t segs = (uint8_t)((width + 7) / 8);
    uint32_t *currbuf;
    uint32_t *cbufl, *cbufr;
    uint16_t pixl, pixr;
    uint8_t segl_pixid, segr_pixid, tmp;
    uint8_t coldelta = 0xF - (dim & 0xF);

#if (VERBOSE > 1)
    os_printf("ssd1322_transform: args: 0x%04x; dim: %d\n"
              " buf @ 0x%08x, height: %d, width: %d\n", args, dim, buf, height, width);
#endif

    for (bufidx = 0; args && args != SSD1322_DA_FLIPLR && bufidx < buflen; ++bufidx) {
        currbuf = buf + bufidx;
        if (args & SSD1322_DA_SWENDIAN) {
            *currbuf = ((*currbuf >> 24) & 0x000000FF) | ((*currbuf >> 8) & 0x0000FF00)
                     | ((*currbuf << 24) & 0xFF000000) | ((*currbuf << 8) & 0x00FF0000);
        }

        if (args & SSD1322_DA_INVERT)
            *currbuf = ~(*currbuf);
    }
    for (rowcnt = 0; (args || dim) && rowcnt < height; ++rowcnt) {
        if (args & SSD1322_DA_FLIPLR || dim) {
            for (pixl = 0, pixr = width - 1; pixl < width / 2; ++pixl, --pixr) {
                cbufl = &(buf[rowcnt * segs + pixl / 8]);
                cbufr = &(buf[rowcnt * segs + pixr / 8]);
                segl_pixid = pixl % 8;
                segr_pixid = pixr % 8;
                //os_printf("pixl: %d[%d], pixr: %d[%d]\n", cbufl - buf, segl_pixid, cbufr - buf, segr_pixid);
                // switch pixr with pixl
                if (args) {
                    tmp = getpix(cbufl, segl_pixid);
                    setpix(cbufl, segl_pixid, getpix(cbufr, segr_pixid));
                    setpix(cbufr, segr_pixid, tmp);
                }
                // substract coldelta if appropriate
                if (dim) {
                    tmp = getpix(cbufl, segl_pixid);
                    setpix(cbufl, segl_pixid, (tmp > coldelta) ? tmp - coldelta : 0);
                    tmp = getpix(cbufr, segr_pixid);
                    setpix(cbufr, segr_pixid, (tmp > coldelta) ? tmp - coldelta : 0);
                }
            }
        }
    }
    return 0;
}

inline void region_prune(struct ssd1322_window *const region) {
    region->x_left = (region->x_left < 0) ? 0 : region->x_left;
    region->x_right = (region->x_right > SSD1322_SEGMENTS * 8 - 1) ? SSD1322_SEGMENTS * 8 - 1 : region->x_right;
    region->y_top = (region->y_top < 0) ? 0 : region->y_top;
    region->y_bottom = (region->y_bottom > SSD1322_ROWS - 1) ? SSD1322_ROWS - 1 : region->y_bottom;
}

uint8_t ssd1322_translate(const struct ssd1322_window *const region, const int16_t x, const int16_t y) {
#if (VERBOSE > 1)
    os_printf("translate: region\n"
              " .xleft: %d\n"
              " .xright: %d\n"
              " .ytop: %d\n"
              " .ybottom: %d\n"
              "by (%d|%d)\n",
              region->x_left, region->x_right, region->y_top, region->y_bottom, x, y);
#endif

    if (!x && !y)
        return 0;

    // inplace.. RAM is PRECIOUS
    int16_t crow, ccol;
    int16_t row_s, row_e, col_s, col_e;
    int8_t row_inc, col_inc;

    // check in which direction we can copy (PHYSICAL layout!! => top/bottom flip)
    if (x >= 0 && y >= 0) {             // phys bottom right -> top left
        row_s = region->y_bottom;
        row_e = region->y_top - 1;
        row_inc = -1;
        col_s = region->x_right;
        col_e = region->x_left - 1;
        col_inc = -1;
    } else if (x >= 0 && y < 0) {        // tr -> bl
        row_s = region->y_top;
        row_e = region->y_bottom + 1;
        row_inc = 1;
        col_s = region->x_right;
        col_e = region->x_left - 1;
        col_inc = -1;
    } else if (x <= 0 && y >= 0) {      // bl -> tr
        row_s = region->y_bottom;
        row_e = region->y_top - 1;
        row_inc = -1;
        col_s = region->x_left;
        col_e = region->x_right + 1;
        col_inc = 1;
    } else if (x < 0 && y < 0) {        // tl -> br
        row_s = region->y_top;
        row_e = region->y_bottom + 1;
        row_inc = 1;
        col_s = region->x_left;
        col_e = region->x_right + 1;
        col_inc = 1;
    } else {
        return 1;
    }

    // tag 4 update
    struct ssd1322_window upd_region;
    upd_region.x_left = (x >= 0) ? region->x_left : region->x_left + x;
    upd_region.x_right = (x >= 0) ? region->x_right + x : region->x_right;
    upd_region.y_top = (y >= 0) ? region->y_top : region->y_top + y;
    upd_region.y_bottom = (y >= 0) ? region->y_bottom + y : region->y_bottom;
#if (VERBOSE > 1)
    os_printf("translate: update_region\n"
              " .xleft: %d\n"
              " .xright: %d\n"
              " .ytop: %d\n"
              " .ybottom: %d\n",
              upd_region.x_left, upd_region.x_right, upd_region.y_top, upd_region.y_bottom);
#endif
    region_prune(&upd_region);
#if (VERBOSE > 1)
    os_printf("translate: pruned update_region\n"
              " .xleft: %d\n"
              " .xright: %d\n"
              " .ytop: %d\n"
              " .ybottom: %d\n",
              upd_region.x_left, upd_region.x_right, upd_region.y_top, upd_region.y_bottom);
#endif
    struct ssd1322_window_phy upd_region_phys = region_log2phys(&upd_region);
    tag_upd_reg(&upd_region_phys);
    uint32_t *cbufr, *cbufw;            // addresses of read / write segments:
    uint8_t segr_pixid, segw_pixid;     // ID of pixel to be read and to be written

    crow = row_s;
    for (crow = row_s; crow != row_e; crow += row_inc) {
        if (crow + y < 0 || crow + y > SSD1322_ROWS - 1) { // out of y boundary
#if (VERBOSE > 1)
            os_printf("transl: skip row %d\n", crow + y);
#endif
            continue;
        }
        for (ccol = col_s; ccol != col_e; ccol += col_inc) {
            segr_pixid = ccol % 8;
            segw_pixid = (ccol + x) % 8;
            if (ccol + x < 0 || ccol + x > SSD1322_SEGMENTS * 8 - 1) { // out of x boundary
#if (VERBOSE > 1)
                os_printf("transl: skip col %d\n", ccol + x);
#endif
                continue;
            }
            cbufr = (uint32_t *)framebuffer + SSD1322_SEGMENTS * row_log2phys(crow) + ccol / 8;
            cbufw = (uint32_t *)framebuffer + SSD1322_SEGMENTS * row_log2phys(crow + y) + (ccol + x) / 8;
            setpix(cbufw, segw_pixid, getpix(cbufr, segr_pixid));   // copy to target
            setpix(cbufr, segr_pixid, 0);                           // clear source
        }
    }
#if (VERBOSE > 1)
    system_soft_wdt_feed();
#endif

    return 0;
}


void ICACHE_FLASH_ATTR ssd1322_draw_rect(const struct ssd1322_window *const border, const uint32_t pattern) {
    uint16_t height = border->y_bottom - border->y_top;
    uint16_t width = border->x_right - border->x_left;
    uint16_t size = (width > height) ? (width + 7) / 8 : (height + 7) / 8;

    uint32_t *const temp = (uint32_t *)os_malloc(size * sizeof(uint32_t));
    uint16_t cidx;
    for (cidx = 0; cidx < size; ++cidx)
        temp[cidx] = pattern;

#if (VERBOSE > 1)
    os_printf("draw_rect:\n"
              " .ybottom: %d\n"
              " .ytop: %d\n"
              " .xright: %d\n"
              " .xleft: %d\n",
              border->y_bottom, border->y_top, border->x_right, border->x_left);
#endif

    // top & bottom
    ssd1322_draw(border->x_left, row_log2phys(border->y_top), temp, 1, width, SSD1322_DA_NONE);
    ssd1322_draw(border->x_left, row_log2phys(border->y_bottom), temp, 1, width, SSD1322_DA_NONE);

    // left & right
    ssd1322_draw(border->x_left, row_log2phys(border->y_top), temp, height, 1, SSD1322_DA_NONE);
    ssd1322_draw(border->x_right,  row_log2phys(border->y_top), temp, height, 1, SSD1322_DA_NONE);

    os_free(temp);
}


//static uint32_t bmbuf[SSD1322_FBSIZE_INT32];
// TODO: maybe read line-wise to safe RAM -> TODO^2: 4byte align bmp by width
// FIX#1: dynamic allocation for now

uint8_t ICACHE_FLASH_ATTR ssd1322_draw_bitmap(const uint16_t x_ul, const uint16_t y_ul, uint32_t address,
                            const uint16_t height, const uint16_t width, const ssd1322_draw_args args) {
    if (width > SSD1322_SEGMENTS * 8 || height > SSD1322_ROWS) {
#if (VERBOSE > 0)
        os_printf("ssd1322_draw_bitmap: bitmap doesn't fit!\n");
#endif
        return 1;
    }
    const uint16_t buflen = (height * width + 1) / 2;
    uint32_t *const bmtempbuf = (uint32_t *)os_malloc(buflen * sizeof(uint8_t));

    spi_flash_read(BMP_ADDRESS + address, bmtempbuf, buflen * sizeof(uint8_t));
    ssd1322_transform(bmtempbuf, height, width, 0, args);
    ssd1322_draw(x_ul, row_log2phys(y_ul), bmtempbuf, height, width, args);

#if (VERBOSE > 1)
        os_printf("ssd1322_draw_bitmap: drawn @ %d, %d, len: %d\n", x_ul, row_log2phys(y_ul), height * width);
#endif

    os_free(bmtempbuf);
    return 0;
}


static uint32_t chbuf[_FONT_MAX_CHAR_SIZE_INT32_];

uint8_t ICACHE_FLASH_ATTR ssd1322_draw_char(const struct char_info *const chi, const struct font_info *const fnt,
                          const uint16_t x_origin, const uint16_t y_ascend, const ssd1322_draw_args args) {
    if (y_ascend - fnt->font_ascend < 0) { // TODO: more security checks

#if (VERBOSE > 1)
        os_printf("draw_char: not enough top space!\n");
#endif

        return 1;
    }

#if (VERBOSE > 1)
    uint32_t bench_time = system_get_time();
#endif

    // top -> bottom
    uint16_t y_ul = row_log2phys(y_ascend) + chi->yoffset;
    uint8_t x_ul = x_origin + chi->xoffset;

#if (VERBOSE > 1)
    os_printf("draw_char: fetch char info: %luus\n", system_get_time() - bench_time);
    bench_time = system_get_time();
#endif

    spi_flash_read(FONT_ADDRESS + chi->address, chbuf, chi->length * sizeof(uint8_t));

#if (VERBOSE > 1)
    os_printf("draw_char: fetch char bin: %luus\n", system_get_time() - bench_time);
#endif

    // sort by pixels
#if (FONT_SORT_PIXELS == 1)
    uint16_t bufidx;
    for (bufidx = 0; bufidx < _FONT_MAX_CHAR_SIZE_INT32_; ++bufidx)
        chbuf[bufidx] = ((chbuf[bufidx] << 28) & 0xF0000000) | ((chbuf[bufidx] << 20) & 0x0F000000)
                      | ((chbuf[bufidx] << 12) & 0x00F00000) | ((chbuf[bufidx] << 4)  & 0x000F0000)
                      | ((chbuf[bufidx] >> 4)  & 0x0000F000) | ((chbuf[bufidx] >> 12) & 0x00000F00)
                      | ((chbuf[bufidx] >> 20) & 0x000000F0) | ((chbuf[bufidx] >> 28) & 0x0000000F);
#elif (FONT_SORT_PIXELS == 2)
    for (bufidx = 0; bufidx < _FONT_MAX_CHAR_SIZE_INT32_; ++bufidx)
        chbuf[bufidx] = ((chbuf[bufidx] << 4)  & 0xF0F0F0F0) | ((chbuf[bufidx] >> 4) & 0x0F0F0F0F);
#endif

#if (VERBOSE > 1)
        os_printf("charinfo: addr: 0x%08x, len: %d, wid: %d, hei: %d, adv: %d\n",
                  chi->address, chi->length, chi->width, chi->height, chi->advance);
#endif

    ssd1322_transform(chbuf, chi->height, chi->width, 0, args);
    ssd1322_draw((uint16_t)x_ul, y_ul, chbuf, (uint16_t)chi->height, (uint16_t)chi->width, args);
    return 0;
}


inline uint8_t ishex(uint8_t chr) {
    return (chr >= '0' && chr <= '9') ||
            (chr >= 'A' && chr <= 'F') ||
            (chr >= 'a' && chr <= 'f');
}


inline uint8_t hex2int(uint8_t chr) {
    if (chr >= '0' && chr <= '9')
        return chr - '0';
    if (chr >= 'A' && chr <= 'F')
        return chr - 'A' + 10;
    if (chr >= 'a' && chr <= 'f')
        return chr - 'a' + 10;
    return 0;
}


inline uint8_t ssd1322_unescape(const uint8_t *string, uint8_t *const target) {
    if (string[0] == '%' && ishex(string[1]) && ishex(string[2])) {
        *target = hex2int(string[1]) * 0x10 + hex2int(string[2]);
        return 0;
    }

    return 1;
}


// FONT related states
static const struct ssd1322_window region_full = {
    .x_left = 0,
    .x_right = SSD1322_SEGMENTS * 8 - 1,
    .y_bottom = SSD1322_ROWS - 1,
    .y_top = 0,
};

static uint8_t line_count = 0;
static struct font_info fnt_current;
static struct char_info dotinfo;
static struct ssd1322_window textbox;
static ssd1322_draw_mode txt_drawmode = SSD1322_DM_TEXT;
static uint16_t txt_xpos = 0, txt_ypos = 0;
static struct ssd1322_chars_in_fb chars_in_fb;

void ICACHE_FLASH_ATTR ssd1322_clear_tb_txt_state(void) {
    txt_xpos = textbox.x_left;
    txt_ypos = textbox.y_top;
    chars_in_fb.chars[0] = 0;
    chars_in_fb.last_char = chars_in_fb.chars;
}

void ICACHE_FLASH_ATTR ssd1322_set_textbox(const struct ssd1322_window *const region) {
    if (region)
        textbox = *region;
    else
        textbox = region_full;

#if (VERBOSE > 1)
    os_printf("set_textbox:\n"
              " .ybottom: %d\n"
              " .ytop: %d\n"
              " .xright: %d\n"
              " .xleft: %d\n",
              textbox.y_bottom, textbox.y_top, textbox.x_right, textbox.x_left);
#endif

    get_font_info(&fnt_current);
    get_char(&dotinfo, '.');
    line_count = (textbox.y_bottom - textbox.y_top) / fnt_current.font_height;
}

void ICACHE_FLASH_ATTR ssd1322_set_cursor(const uint16_t x_l, const uint16_t y_asc) {
    txt_drawmode = SSD1322_DM_TEXT;
    txt_xpos = x_l;
    txt_ypos = y_asc;
}

void ICACHE_FLASH_ATTR ssd1322_set_mode(const ssd1322_draw_mode dm) {
    txt_drawmode = dm;
}

// draw string inside selected area with offsets
uint8_t ICACHE_FLASH_ATTR ssd1322_print(const uint8_t* string, const uint16_t x_l, const uint16_t y_asc,
                                        const ssd1322_draw_args args, uint16_t *x_l_re, uint16_t *y_asc_re) {
    uint16_t chr_idx;
    uint16_t currx, curry;
    uint16_t x_l_tb = x_l + textbox.x_left;
    uint16_t y_asc_tb = y_asc + textbox.y_top;      // x, y corrected by textbox offsets
    struct char_info chi;
    uint8_t override_last_char = (txt_drawmode != SSD1322_DM_FREE) ? *chars_in_fb.last_char : 0;
    uint8_t line_force_tag = 0;
    struct ssd1322_window string_max_extent = {     // struct to get dimension of string (pix)
        .x_left = (int16_t)x_l_tb,                  // TODO: make global, so it works for multiple print(.)
        .x_right = (int16_t)x_l_tb,
        .y_top = (int16_t)y_asc_tb,
        .y_bottom = (int16_t)y_asc_tb
    };
    if (!txt_xpos && !txt_ypos || txt_drawmode == SSD1322_DM_FREE) {
        // place 'cursor' inside textbox
        currx = x_l_tb;
        curry = y_asc_tb;
    } else {
        currx = txt_xpos;
        curry = txt_ypos;
    }

#if (VERBOSE > 1)
    os_printf("print: '%s' @ (%d,%d) & dm = %d\n", string, currx, curry, txt_drawmode);
#endif

    for (chr_idx = 0; string[chr_idx] != '\0'; ++chr_idx) {
        // check for HTML escape
        uint8_t currchr;
        if (!ssd1322_unescape(&(string[chr_idx]), &currchr))
            chr_idx += 2;
        else
            currchr = string[chr_idx];
#if (VERBOSE > 1)
        os_printf("print: currchr: '%c' (%d)\n", currchr, currchr);
#endif
        uint8_t renderable = !get_char(&chi, currchr);

        // update max extents
        if (renderable) {
            if (curry - chi.yoffset < string_max_extent.y_top)
                string_max_extent.y_top = curry - chi.yoffset;
            if (curry + chi.height - chi.yoffset > string_max_extent.y_bottom)
                string_max_extent.y_bottom = curry + chi.height - chi.yoffset;
            if (currx + chi.width + chi.xoffset > string_max_extent.x_right)
                string_max_extent.x_right = currx + chi.width + chi.xoffset;
        }

        // boundary checks
        if (txt_drawmode != SSD1322_DM_FREE) {
            if ((renderable && currx + chi.advance > textbox.x_right) || line_force_tag) {  // newline type 1
#if (VERBOSE > 1)
                os_printf("print: newline type 1\n");
#endif
                if (txt_drawmode == SSD1322_DM_TEXT_LINE_FORCE) {
                    // draw '...'
#if (VERBOSE > 1)
                    os_printf("print: line force: skipping...\n");
#endif
                    if (currx + 3 * dotinfo.advance > textbox.x_right) {
                        currchr = 8; // backspace -> del last char
                        line_force_tag = 1;
                    }
                } else {
                    // linebreak on SPACE: check for last word
                    uint8_t maxcharsperline = (textbox.x_right - textbox.x_left) / _FONT_MAX_CHAR_WIDTH_;
                    if (chars_in_fb.last_char > chars_in_fb.last_word
                            && chars_in_fb.last_char - chars_in_fb.last_word < maxcharsperline) {
                        os_printf("lastchar: %08x; lastword: %08x; max: %d\n", chars_in_fb.last_char, chars_in_fb.last_word, maxcharsperline);
                        // replace this ' ' with '\n' and rewind
                        ++chars_in_fb.last_char;
                        *chars_in_fb.last_char = 0; // rewind until now
                        *chars_in_fb.last_word = '\n';
                        ssd1322_clear_fb(SSD1322_OS_TEXTBOX);
                        ssd1322_clear_tb_txt_state();
                        currx = x_l_tb;
                        curry = y_asc_tb;

                        ssd1322_print(&(chars_in_fb.chars[1]), x_l_tb, y_asc_tb, args, &currx, &curry);
                    } else {
                        override_last_char = 0;
                        curry += fnt_current.font_height;
                        currx = x_l_tb;
                        // skip next newline / space
                        if (string[chr_idx + 1] == '\n' || string[chr_idx + 1] == ' ') {
                            override_last_char = string[chr_idx + 1];
                            ++chars_in_fb.last_char;
                            *chars_in_fb.last_char = string[chr_idx + 1];
                            ++chr_idx;
                        }
                    }
                }
            }
            // new word!
            if (*chars_in_fb.last_char == ' ' || *chars_in_fb.last_char == '\n')
                chars_in_fb.last_word = chars_in_fb.last_char;

            if (currchr == 8) {     // backspace -- well fuck.
                uint8_t bsnum = 1;  // number of consecutive backspaces
                uint8_t unesc;
                for (; string[chr_idx + 1] != '\0'; ++chr_idx) {
                    if (!ssd1322_unescape(&(string[chr_idx + 1]), &unesc))
                        chr_idx += 2;
                    else
                        unesc = string[chr_idx + 1];

                    if (unesc == 8)
                        ++bsnum;
                    else
                        break;
                }
                // set end of new string -> zero at forward position
                chars_in_fb.last_char -= (bsnum - 1);
                if (chars_in_fb.last_char > chars_in_fb.chars) {
                    *chars_in_fb.last_char = 0;
                } else {    // whole string is gone
                    chars_in_fb.chars[1] = 0;
                }
                ssd1322_clear_fb(SSD1322_OS_TEXTBOX);
                ssd1322_clear_tb_txt_state();
                currx = x_l_tb;
                curry = y_asc_tb;

                // print history without last char(s); return new positions
                // NOTE: not really recursive, as it will probably only get called once...
                //       exluding stuff like "a\bb\bc\bd" etc <- but who does that?!
                ssd1322_print(&(chars_in_fb.chars[1]), x_l_tb, y_asc_tb, args, &currx, &curry);

                if (line_force_tag && currx + 3 * dotinfo.advance <= textbox.x_right) {
                    // '...' fits
                    ssd1322_print((uint8_t *)"...", x_l_tb, y_asc_tb, args, NULL, NULL);
#if (VERBOSE > 1)
                    os_printf("print: skipping done!\n");
#endif
                    // TODO: test multiline
                    for (; string[chr_idx + 1] != '\0' && string[chr_idx + 1] != '\n'; ++chr_idx);
                    line_force_tag = 0;
                    //return 1;
                }
#if (VERBOSE > 1)
                os_printf("print: backspace; to go: '%s'\n", &string[chr_idx]);
#endif
                continue;
            }
        } // if !DM_FREE

        if (currchr == '\n') {  // newline type 2
#if (VERBOSE > 1)
            os_printf("print: newline type 2\n");
#endif
            override_last_char = currchr;
            if (txt_drawmode != SSD1322_DM_FREE) {
                ++chars_in_fb.last_char;
                *chars_in_fb.last_char = currchr;
            }
            curry += fnt_current.font_height;
            currx = x_l_tb;
            string_max_extent.y_bottom += fnt_current.font_height;
            continue;
        }
        if (currchr == '\r') {  // carriage return
            override_last_char = 0;
            currx = x_l_tb;
            continue;
        }

        // skip this char
        if (!renderable)
            continue;

        if (curry - fnt_current.font_ascend + fnt_current.font_height > textbox.y_bottom) { // no space left, newline type 3
#if (VERBOSE > 1)
            os_printf("print: newline type 3; no space left (y = %d, need %d)\n",
                      curry, curry - fnt_current.font_ascend + fnt_current.font_height);
#endif
            override_last_char = 0;
            ssd1322_clear_tb_txt_state();
            if (txt_drawmode == SSD1322_DM_TEXT) {
                txt_xpos = currx;
                txt_ypos = curry;
                return 1;
            } else if (txt_drawmode == SSD1322_DM_TEXT_CLR) {
                ssd1322_clear_fb(SSD1322_OS_TEXTBOX);
                currx = x_l_tb;
                curry = y_asc_tb;
            } else {
                return 1;
            }
        }

        // only kerning if no new line
        currx += get_kerning(override_last_char, currchr);
        if ((int32_t)currx + (int32_t)chi.xoffset < 0)
            currx -= chi.xoffset;
#if (VERBOSE > 1)
        os_printf("print: draw char @ (%d,%d)\n", currx, curry);
#endif

        if (ssd1322_draw_char(&chi, &fnt_current, currx, curry, args)) {
#if (VERBOSE > 1)
            os_printf("ssd1322_print: skipped '%c'\n", currchr);
#endif
            continue;
        }
        currx += chi.advance;
        override_last_char = currchr;
        if (txt_drawmode != SSD1322_DM_FREE) {
            ++chars_in_fb.last_char;    // chars_in_fb.chars[0] is always 0!
            *chars_in_fb.last_char = currchr;
        }
    }   // for chr_idx in string

    if (txt_drawmode != SSD1322_DM_FREE) {
        txt_xpos = currx;
        txt_ypos = curry;
    }

    if (txt_drawmode == SSD1322_DM_FREE) {  // TODO: enable for recursive functionality
        int16_t trans_x = 0, trans_y = 0;
        if (args & SSD1322_DA_CENTER_X) {
            uint8_t string_width = string_max_extent.x_right - string_max_extent.x_left;
            uint8_t tb_width = textbox.x_right - textbox.x_left;
            trans_x = (tb_width - string_width) / 2;
        }
        if (args & SSD1322_DA_CENTER_Y) {
            uint8_t string_height = string_max_extent.y_bottom - string_max_extent.y_top;
            uint8_t tb_heigth = textbox.y_bottom - textbox.y_top;
            trans_y = (tb_heigth - string_height) / 2 - (string_max_extent.y_top - textbox.y_top);
        }
        ssd1322_translate(&string_max_extent, trans_x, trans_y);
        currx += trans_x;
        curry += trans_y;
        string_max_extent.x_left += trans_x;
        string_max_extent.x_right += trans_x;
        string_max_extent.y_bottom += trans_y;
        string_max_extent.y_top += trans_y;
    }

#ifdef DBG_SHOW_TEXT_EXTENT
    ssd1322_draw_rect(&string_max_extent, 0x09090909);
#endif
#ifdef DBG_SHOW_TEXTBOX_BORDER
    ssd1322_draw_rect(&textbox, 0xAAAAAAAA);
#endif

#if (VERBOSE > 1)
    os_printf("print: tracked chars in fb: ");
    uint8_t *cchr = chars_in_fb.chars;
    for (; cchr <= chars_in_fb.last_char; ++cchr)
        os_printf("%c", *cchr);
    os_printf("; len (%lu/%d) @ chars + %lu; %lu to go\n", (cchr - chars_in_fb.chars) / sizeof(uint8_t) + 1, SSD1322_MAX_CHARS,
                (chars_in_fb.last_char - chars_in_fb.chars) / sizeof(uint8_t) + 1,
                (&(chars_in_fb.chars[SSD1322_MAX_CHARS - 1]) - cchr) / sizeof(uint8_t));
#endif

    if (x_l_re && y_asc_re) {   // return new position
        *x_l_re = currx;
        *y_asc_re = curry;
    }

    return 0;
}

void ICACHE_FLASH_ATTR ssd1322_update_gram(void) {
#if (VERBOSE > 1)
    os_printf("update_gram... segl: %d ... segr: %d; rowb: %d ... rowt: %d\n", fb_upd_reg.seg_left, fb_upd_reg.seg_right,
              fb_upd_reg.row_bottom, fb_upd_reg.row_top);
#endif
#if (VERBOSE > 0)
    uint32_t bench_stime = system_get_time();
#endif

    // set window
    ssd1322_set_area_phy((struct ssd1322_window_phy *)&fb_upd_reg);

    // check how much stuff changed
    if ((fb_upd_reg.seg_right - fb_upd_reg.seg_left) * (fb_upd_reg.row_bottom - fb_upd_reg.row_top)
            > (SSD1322_SEGMENTS * SSD1322_ROWS) / 2) {
        // write whole framebuffer
        ssd1322_send_data((uint32_t *)framebuffer, SSD1322_SEGMENTS * SSD1322_ROWS);
    }
    else { // write partial framebuffer
        uint8_t row_idx;
        for (row_idx = fb_upd_reg.row_bottom; row_idx <= fb_upd_reg.row_top; ++row_idx) {
#if (VERBOSE == 3)
            os_printf("partial fb: 0x%08x (0x%08x), len: %d\n", (uint32_t *)&framebuffer[SSD1322_SEGMENTS * row_idx + fb_upd_reg.seg_left],
                    framebuffer[SSD1322_SEGMENTS * row_idx + fb_upd_reg.seg_left], fb_upd_reg.seg_right - fb_upd_reg.seg_left + 1);
#endif
            ssd1322_send_data((uint32_t *)&(framebuffer[SSD1322_SEGMENTS * row_idx + fb_upd_reg.seg_left]),
                    fb_upd_reg.seg_right - fb_upd_reg.seg_left + 1);
        }
    }

    // reset window
    fb_upd_reg = default_region_phy;

#if (VERBOSE > 0)
    os_printf("send fb: %luus\n", system_get_time() - bench_stime);
#endif
}


static os_timer_t volatile fadeout_timer;
static uint8_t brightness = SSD1322_MAX_BRIGHTNESS;
static uint8_t fadeout_type;
static void (*fadeout_callback)(void);

void ICACHE_FLASH_ATTR fadeout_func(void *arg) {
    int16_t reduce_by = 1 + (SSD1322_MAX_BRIGHTNESS - brightness) * (SSD1322_MAX_BRIGHTNESS - brightness) / 5000;
    if ((int16_t)brightness - reduce_by < 0)
        brightness = 0;
    else
        brightness -= (uint8_t)reduce_by;

    if (brightness == 0) {
        os_timer_disarm(&fadeout_timer);
        ssd1322_clear();
        brightness = SSD1322_MAX_BRIGHTNESS;
        if (fadeout_type == 2)
            ssd1322_update_gram();
        fadeout_callback();
    }
    //os_printf("brightness: %d\n", brightness);
    uint8_t cmd[] = {
        SSD1322_CONTRAST, brightness,
    };
    ssd1322_send_command(cmd, 2);
}

void ICACHE_FLASH_ATTR ssd1322_clear_with_fadeout_anim(const uint8_t ftype, void (*fadeout_cb)(void)) {
    os_timer_disarm(&fadeout_timer);
    os_timer_setfn(&fadeout_timer, (os_timer_func_t *)fadeout_func, NULL);
    os_timer_arm(&fadeout_timer, SSD1322_ANIM_DELAY_MS, 1);

    fadeout_type = ftype;
    fadeout_callback = fadeout_cb;
    return;
}

static os_timer_t volatile toss_timer;
static uint16_t anim_toss_time = 0;
static ssd1322_anim_direction toss_dir;
static struct ssd1322_window toss_region, ctoss_region;
static int16_t toss_x = 0, toss_y = 0;
static void (*toss_callback)(void);

inline uint8_t check_boundaries(const struct ssd1322_window *const region) {
    return ((region->x_left < 0 || region->x_left > SSD1322_SEGMENTS * 8 - 1)
            && (region->x_right < 0 || region->x_right > SSD1322_SEGMENTS * 8 - 1))
        || ((region->y_bottom < 0 || region->y_bottom > SSD1322_ROWS - 1)
            && (region->y_top < 0 || region->y_top > SSD1322_ROWS - 1));
}

static void ICACHE_FLASH_ATTR toss_func(void *arg) {
    ctoss_region.x_left += toss_x;
    ctoss_region.x_right += toss_x;
    ctoss_region.y_top += toss_y;
    ctoss_region.y_bottom += toss_y;

    int16_t move_by = 0;
    if (anim_toss_time > 100)
        move_by = (int16_t)(((int32_t)(anim_toss_time - 100) * (int32_t)(anim_toss_time - 100)) / (int32_t)1000);
#if (VERBOSE > 1)
    os_printf("toss: move by %d towards %d\n", move_by, toss_dir);
#endif
    switch(toss_dir) {
    case SSD1322_AD_N:
        toss_y = -move_by; break;
    case SSD1322_AD_NW:
        toss_y = -move_by;
        toss_x = -move_by; break;
    case SSD1322_AD_W:
        toss_x = -move_by; break;
    case SSD1322_AD_SW:
        toss_y = move_by;
        toss_x = -move_by; break;
    case SSD1322_AD_S:
        toss_y = move_by; break;
    case SSD1322_AD_SE:
        toss_y = move_by;
        toss_x = move_by; break;
    case SSD1322_AD_E:
        toss_x = move_by; break;
    case SSD1322_AD_NE:
        toss_y = -move_by;
        toss_x = move_by; break;
    }

#if (VERBOSE > 1)
    os_printf("toss_region:\n"
              " .xleft: %d\n"
              " .xright: %d\n"
              " .ytop: %d\n"
              " .ybottom: %d\n"
              "animtime: %d\n",
              toss_region.x_left, toss_region.x_right, toss_region.y_top, toss_region.y_bottom, anim_toss_time);
#endif
    ssd1322_translate(&toss_region, toss_x, toss_y);
    if (check_boundaries(&ctoss_region)) { // everything is outside
        os_timer_disarm(&toss_timer);
        anim_toss_time = 0;
        struct ssd1322_window tb_tmp = textbox;
        textbox = toss_region;
        ssd1322_clear_fb(SSD1322_OS_TEXTBOX);
        textbox = tb_tmp;

        toss_callback();

#if (VERBOSE > 1)
        dbg_dump_fb(&region_full);
#endif
    }
    if (toss_x || toss_y)
        ssd1322_update_gram();

    ++anim_toss_time;
}

void ICACHE_FLASH_ATTR ssd1322_anim_toss_away(const ssd1322_anim_direction dir, const struct ssd1322_window *const region,
                                              void (*toss_cb)(void)) {
    os_timer_disarm(&toss_timer);
    os_timer_setfn(&toss_timer, (os_timer_func_t *)toss_func, NULL);
    os_timer_arm(&toss_timer, SSD1322_ANIM_DELAY_MS, 1);
    toss_region = *region;
    ctoss_region = *region;
    toss_callback = toss_cb;
    toss_dir = dir;
    toss_x = 0;
    toss_y = 0;
}

void ICACHE_FLASH_ATTR ssd1322_clear_fb_with_toss_anim(const ssd1322_anim_direction dir, void (*toss_cb)(void)) {
    ssd1322_anim_toss_away(dir, &region_full, toss_cb);
}

void ICACHE_FLASH_ATTR ssd1322_clear(void) {
#if (_SSD1322_MODE_ == 256*64)
    uint16_t i = 0;
    ssd1322_set_area_phy(NULL); // reset area
    if (!write_to_gram) {
        uint8_t cmd = SSD1322_WRITE;
        ssd1322_send_command(&cmd, 0);
        write_to_gram = 1;
        gpio_output_set(_SSD1322_DC_PIN_, 0, _SSD1322_DC_PIN_, 0); // set D/C line high
    }
    for (; i < SSD1322_ROWS * SSD1322_SEGMENTS; ++i)
        spi_txd(HSPI, 32, background_pattern);
    os_delay_us(1000);
#endif
#if (VERBOSE > 1)
    os_printf("ssd1322_clear: done\n");
#endif
}

void ICACHE_FLASH_ATTR ssd1322_clear_fb(const ssd1322_object_specifier obj) {
    uint16_t i;
    switch (obj) {
    case SSD1322_OS_ALL:
#if (VERBOSE > 1)
        os_printf("ssd1322_clear_fb: ALL...");
#endif
        tag_upd_reg(&max_region_phy);
        for (i = 0; i < SSD1322_FBSIZE_INT32; ++i)
            framebuffer[i] = background_pattern;

        break;

    case SSD1322_OS_STAGED:                         // TODO: will still clear everything... only not staged!
#if (VERBOSE > 1)
        os_printf("ssd1322_clear_fb: STAGED...");
#endif
        fb_upd_reg = fb_upd_reg_old;                // TODO: TEST!
        uint16_t k;
        for (i = fb_upd_reg.seg_left; i < fb_upd_reg.seg_right; ++i)
            for (k = fb_upd_reg.row_bottom; k < fb_upd_reg.row_top; ++k)
                framebuffer[k * SSD1322_SEGMENTS + i] = background_pattern;

        break;

    case SSD1322_OS_TEXTBOX:
#if (VERBOSE > 1)
        os_printf("ssd1322_clear_fb: TEXTBOX...\n");
#endif
        ssd1322_draw(textbox.x_left, row_log2phys(textbox.y_top), &background_pattern, textbox.y_bottom - textbox.y_top + 1,
                     textbox.x_right - textbox.x_left + 1, SSD1322_DA_CLR);
        break;
    } // switch (obj)

#if (VERBOSE > 1)
    os_printf("ssd1322_clear_fb: done!\n");
#endif
}


void ICACHE_FLASH_ATTR ssd1322_reset(void) {
    fb_upd_reg = default_region_phy;

    ssd1322_set_textbox(NULL);
    ssd1322_clear_tb_txt_state();
    ssd1322_set_mode(SSD1322_DM_TEXT);

    gpio_output_set(0, _SSD1322_RESET_PIN_, _SSD1322_RESET_PIN_, 0);
    os_delay_us(5000);
    gpio_output_set(_SSD1322_RESET_PIN_, 0, _SSD1322_RESET_PIN_, 0);
    os_delay_us(5000);
    ssd1322_clear();
#if (VERBOSE > 1)
    os_printf("ssd1322_reset: done\n");
#endif
}

void ICACHE_FLASH_ATTR ssd1322_set_background(uint32_t pattern) {
    background_pattern = pattern;
}


// init SPI & SSD1322_
void ICACHE_FLASH_ATTR ssd1322_init(void) {
    // SPI config
    spi_init(HSPI);
    spi_mode(HSPI, 1, 0); // 'trailing edge': positive edge
    spi_init_gpio(HSPI, SPI_CLK_USE_DIV);
    // SPI_CLK_FREQ = CPU_CLK_FREQ/(SPI_CLK_PREDIV*SPI_CLK_CNTDIV) -> 80MHz / 8 = 10MHz -> 2 / 4
    // (CPU_CLK_FREQ is actually peripheral clock and ALWAYS 80MHz, maybe different from cpu clock, eg. 160MHz)
    spi_clock(HSPI, 2, 4);
    spi_tx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);

    // GPIO config... TODO: expand GPIO2 into macro
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
    gpio_output_set(0, _SSD1322_DC_PIN_, _SSD1322_DC_PIN_, 0);          // set C/R low
    gpio_output_set(_SSD1322_RESET_PIN_, 0, _SSD1322_RESET_PIN_, 0);    // set reset high

    ssd1322_reset();

    uint8_t init_commands[] = {
    #if (_SSD1322_MODE_ == 256*64)
        SSD1322_COM_LOCK,       1,  SSD1322_COM_UNLOCK,
        SSD1322_DISP_OFF,       0,
        SSD1322_COL_ADDR,       2,  0x1C, 0x5B,             // col 28 .. 91
        SSD1322_ROW_ADDR,       2,  0x00, 0x3F,             // row 0 .. 63
        SSD1322_DISP_CLK_DIV,   1,  0b0101 << 4 | 0b0001,
        SSD1322_MUX_RATIO,      1,  0x3F,                   // 1/64
        SSD1322_DISP_OFFSET,    1,  0x00,
        SSD1322_DISP_SLINE,     1,  0x00,
        SSD1322_COM_MODE,       2,  SSD1322_COM_HOR_INCREM | SSD1322_COM_NIB_REMAP, SSD1322_LMODE_DUAL_EN,
        SSD1322_GPIO,           1,  0x00,
        SSD1322_VDD,            1,  SSD1322_VDD_INT,
        SSD1322_DISP_CFG1,      2,  SSD1322_SEG_VREF_INT, SSD1322_ENH_LOW_GS_DIS,
        SSD1322_CONTRAST,       1,  0x7F,
        SSD1322_CONTRAST_MASTER,    1,  0x0F,
        SSD1322_PRECHARGE,      1,  0b1110 << 4 | 0b0111,
        SSD1322_VPRECHARGE,     1,  0x08,
        SSD1322_PRECHARGE2,     1,  0x02,
        SSD1322_VCOMH,          1,  0x04,
        SSD1322_ALL_NORMAL,     0
    #endif
    };
    ssd1322_send_command_list(&(init_commands[0]), sizeof(init_commands) / sizeof(uint8_t));

    ssd1322_clear();
}
