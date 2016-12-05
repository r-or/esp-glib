/*
 *
 * License: MIT / x11
 * Copyright 2016 Tobias Däullary
 *
 */


#include "user_config.h"
#include "ssd1322.h"
#include "driver/spi.h"
#include "mem.h"

static struct glib_window_phy volatile fb_upd_reg;                   // region to upload with update_gram function
static struct glib_window_phy volatile fb_upd_reg_old;
static uint8_t volatile write_to_gram = 0;

static const struct glib_window_phy default_region_phy = {
    .seg_left = SSD1322_SEGMENTS - 1,
    .seg_right = 0,
    .row_bottom = SSD1322_ROWS - 1,
    .row_top = 0,
};

static const struct glib_window_phy max_region_phy = {
    .seg_left = 0,
    .seg_right = SSD1322_SEGMENTS - 1,
    .row_bottom = 0,
    .row_top = SSD1322_ROWS - 1,
};

// send stuff to GRAM
void ICACHE_FLASH_ATTR ssd1322_send_data(const uint32_t *const qbytes, const uint16_t qbytes_no) {
    uint16_t i = 0;
#if (VERBOSE > 1)
    os_printf("ssd1322_send_data: sending %lu bytes...\n", qbytes_no * 4);
#endif
#if (SSD1322_IO == SSD1322_SPI4WIRE)
    if (!write_to_gram) {
        uint8_t cmd = SSD1322_WRITE;
        ssd1322_send_command(&cmd, 0);
        write_to_gram = 1;
        gpio_output_set(SSD1322_DC_PIN, 0, SSD1322_DC_PIN, 0); // set D/C line high
        os_delay_us(1000);
    }

#if (VERBOSE > 2)
    for (i = 0; i < qbytes_no; ++i) {
        os_printf("%08x", qbytes[i]);
        if (!((i + 1) % (fb_upd_reg.seg_right - fb_upd_reg.seg_left + 1)))
            os_printf("\n");
    }
#endif

    for (i = 0; i < qbytes_no; ++i) {
        spi_txd(HSPI, 32, qbytes[i]);
    }
#elif (SSD1322_IO == SSD1322_SPI3WIRE)
    spi_txd(HSPI, 8, SSD1322_WRITE);
    for (i = 0; i < bytes_no; ++i)d
        spi_transaction(HSPI, 9, (uint16_t)(bytes[i]) | 0x100, 0, 0, 0, 0, 0, 0);
#endif
}

// send stuff to cmd register
void ICACHE_FLASH_ATTR ssd1322_send_command(const uint8_t *const cmd, const uint8_t cmd_len) {
    write_to_gram = 0;
#if (SSD1322_IO == SSD1322_SPI4WIRE)
    gpio_output_set(0, SSD1322_DC_PIN, SSD1322_DC_PIN, 0);  // set D/C line low
    os_delay_us(1000);
    spi_transaction(HSPI, 8, cmd[0], 0, 0, 0, 0, 0, 0);
    os_delay_us(1000);
#if VERBOSE > 1
    os_printf("ssd1322_send_command: sent %x[%d]", cmd[0], cmd_len);
#endif

    uint8_t i = 1;
    if (cmd_len > 1) {
        gpio_output_set(SSD1322_DC_PIN, 0, SSD1322_DC_PIN, 0); // set D/C line high
        for (; i < cmd_len; ++i) {
            spi_transaction(HSPI, 8, cmd[i], 0, 0, 0, 0, 0, 0);

#if VERBOSE > 1
            os_printf(":%x", cmd[i]);
#endif
        }
    }
#elif (SSD1322_IO == SSD1322_SPI3WIRE)
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

#if (SSD1322_IO == SSD1322_SPI4WIRE)
    for (; i < list_len; ++i) {
        gpio_output_set(0, SSD1322_DC_PIN, SSD1322_DC_PIN, 0);  // set D/C line low
        spi_transaction(HSPI, 8, cmd_list[i], 0, 0, 0, 0, 0, 0);
        os_delay_us(1000);
#if VERBOSE > 1
        os_printf("|%x", cmd_list[i], cmd_list[i + 1]);
#endif

        if (i + 1 < list_len && cmd_list[i + 1] && i + 1 + cmd_list[i + 1] < list_len) {

#if VERBOSE > 1
            os_printf("[%d]", cmd_list[i + 1]);
#endif

            gpio_output_set(SSD1322_DC_PIN, 0, SSD1322_DC_PIN, 0); // set D/C line high
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
#elif (SSD1322_IO == SSD1322_SPI3WIRE)
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

static void ICACHE_FLASH_ATTR ssd1322_set_area_phy(const struct glib_window_phy *const region) {
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
                && region->seg_left == fb_upd_reg_old.seg_left && region->seg_right == fb_upd_reg_old.seg_right) {
#if (VERBOSE > 1)
            os_printf("ssd1322_set_area: nothing to be done!\n");
#endif
            return;
        }
        fb_upd_reg_old = *region;
        uint8_t commands[] = {
            // col address is segmented in 2bytes or 4 pixels for whatever reason
            SSD1322_COL_ADDR,   2,  SSD1322_COL_SEG_START + region->seg_left * 2,
                                    SSD1322_COL_SEG_START + region->seg_right * 2 + 1,
            SSD1322_ROW_ADDR,   2,  SSD1322_ROW_START + region->row_bottom,
                                    SSD1322_ROW_START + region->row_top
        };
#if (VERBOSE > 1)
        os_printf("ssd1322_set_area: setting to\n"
                  " .left: %d\n"
                  " .right: %d\n"
                  " .bottom: %d\n"
                  " .top: %d\n",
                  commands[2], commands[3], commands[6], commands[7]);
#endif
        ssd1322_send_command_list(commands, sizeof(commands) / sizeof(uint8_t));
    } else {
        fb_upd_reg_old = (struct glib_window_phy) {
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
}



/*
 * IMPLEMENTATION OF GLIB FUNCTIONALITY
 */

void ICACHE_FLASH_ATTR glib_set_brightness(const uint8_t value) {
    uint8_t cmd[] = {
        SSD1322_CONTRAST, value,
    };
    ssd1322_send_command(cmd, 2);
}


void ICACHE_FLASH_ATTR glib_set_enable(const uint8_t enable) {
    uint8_t cmd;
    if (enable)
        cmd = SSD1322_DISP_ON;
    else
        cmd = SSD1322_DISP_OFF;
    ssd1322_send_command(&cmd, 1);
}


void ICACHE_FLASH_ATTR glib_clear_disp(const uint32_t pattern) {
#if (SSD1322_MODE == 256*64)
    uint16_t i = 0;
    ssd1322_set_area_phy(NULL); // reset area
    if (!write_to_gram) {
        uint8_t cmd = SSD1322_WRITE;
        ssd1322_send_command(&cmd, 0);
        write_to_gram = 1;
        gpio_output_set(SSD1322_DC_PIN, 0, SSD1322_DC_PIN, 0); // set D/C line high
    }
    for (; i < SSD1322_ROWS * SSD1322_SEGMENTS; ++i)
        spi_txd(HSPI, 32, pattern);
    os_delay_us(1000);
#endif
#if (VERBOSE > 1)
    os_printf("ssd1322_clear: done\n");
#endif
}


void ICACHE_FLASH_ATTR glib_update_gram(uint32_t *const framebuffer) {
#if (VERBOSE > 1)
    os_printf("update_gram... segl: %d ... segr: %d; rowb: %d ... rowt: %d\n", fb_upd_reg.seg_left, fb_upd_reg.seg_right,
              fb_upd_reg.row_bottom, fb_upd_reg.row_top);
#endif
#if (VERBOSE > 0)
    uint32_t bench_stime = system_get_time();
#endif

    // check how much stuff changed
    if ((fb_upd_reg.seg_right - fb_upd_reg.seg_left) * (fb_upd_reg.row_top - fb_upd_reg.row_bottom)
            > (SSD1322_SEGMENTS * SSD1322_ROWS) / 2) {
        // set window
        ssd1322_set_area_phy(&max_region_phy);
        // write whole framebuffer
        ssd1322_send_data(framebuffer, SSD1322_SEGMENTS * SSD1322_ROWS);
    }
    else { // write partial framebuffer
        // set window
        ssd1322_set_area_phy((struct glib_window_phy *)&fb_upd_reg);
        uint8_t row_idx;
        for (row_idx = fb_upd_reg.row_bottom; row_idx <= fb_upd_reg.row_top; ++row_idx) {
#if (VERBOSE == 3)
            os_printf("partial fb: 0x%08x (0x%08x), len: %d\n", (uint32_t *)&framebuffer[SSD1322_SEGMENTS * row_idx + fb_upd_reg.seg_left],
                    framebuffer[SSD1322_SEGMENTS * row_idx + fb_upd_reg.seg_left], fb_upd_reg.seg_right - fb_upd_reg.seg_left + 1);
#endif
            ssd1322_send_data(&(framebuffer[SSD1322_SEGMENTS * row_idx + fb_upd_reg.seg_left]),
                    fb_upd_reg.seg_right - fb_upd_reg.seg_left + 1);
        }
    }

    // reset window
    fb_upd_reg = default_region_phy;

#if (VERBOSE > 0)
    os_printf("send fb: %luus\n", system_get_time() - bench_stime);
#endif
}

// convert logical row to physical row
inline int16_t glib_row_log2phys(const int16_t log_row) {
    return SSD1322_ROWS - 1 - log_row;
}

inline int16_t glib_col_log2phys(const int16_t log_col) {
    return log_col;
}

// convert whole logical window
inline struct glib_window_phy glib_region_log2phys(const struct glib_window *const log_region) {
    return (struct glib_window_phy) {
        .seg_left = (uint8_t)glib_col_log2phys(log_region->x_left) / 8,
        .seg_right = (uint8_t)glib_col_log2phys(log_region->x_right) / 8,
        .row_top = (uint8_t)glib_row_log2phys(log_region->y_top),
        .row_bottom = (uint8_t)glib_row_log2phys(log_region->y_bottom),
    };
}

static inline void tag_upd_reg(const struct glib_window_phy *const region) {
#if (VERBOSE > 1)
    struct glib_window_phy old = fb_upd_reg;
#endif
    if (region->seg_left < fb_upd_reg.seg_left)
        fb_upd_reg.seg_left = region->seg_left;
    if (region->seg_right > fb_upd_reg.seg_right)
        fb_upd_reg.seg_right = region->seg_right;
    if (region->row_top > fb_upd_reg.row_top)
        fb_upd_reg.row_top = region->row_top;
    if (region->row_bottom < fb_upd_reg.row_bottom)
        fb_upd_reg.row_bottom = region->row_bottom;

#if (VERBOSE > 1)
    os_printf("tag updatereg: OLD|NEW => result\n"
              " .segleft:   %2d|%2d => %2d\n"
              " .segright:  %2d|%2d => %2d\n"
              " .rowtop:    %2d|%2d => %2d\n"
              " .rowbottom: %2d|%2d => %2d\n",
              old.seg_left, region->seg_left, fb_upd_reg.seg_left,
              old.seg_right, region->seg_right, fb_upd_reg.seg_right,
              old.row_top, region->row_top, fb_upd_reg.row_top,
              old.row_bottom, region->row_bottom, fb_upd_reg.row_bottom);
#endif
}

void glib_tag_upd_reg_log(const struct glib_window *const region) {
    struct glib_window_phy tmp = glib_region_log2phys(region);
    tag_upd_reg(&tmp);
}

uint8_t ICACHE_FLASH_ATTR glib_draw(uint32_t *const to_buf, const uint16_t x_ul, const uint16_t y_ul, uint32_t *const bitmap,
                                    const uint16_t height, const uint16_t width, const glib_draw_args args) {
    if (!height || !width)
        return 0;

    // convert to actual physical coordinates
    uint16_t x_ul_ph = glib_col_log2phys(x_ul);
    uint16_t y_ul_ph = glib_row_log2phys(y_ul);

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
    uint8_t seg_l = (uint8_t)(x_ul_ph / 8);  // floor
    uint8_t seg_l_pix = x_ul_ph % 8;
    uint8_t seg_r = (uint8_t)((x_ul_ph + (width - (width ? 1 : 0))) / 8);
    uint8_t seg_r_pix = (x_ul_ph + width) % 8;

    uint8_t seg_num = seg_r - seg_l + 1;   // segments affected per scanline
    uint16_t pix_idx_y;

    uint32_t *currfb;
    uint32_t *currbuf = bitmap;
    uint8_t chbuf_pix_pad = 0;
    uint8_t curr_seg;
    uint8_t clr = args & GLIB_DA_CLR;
    uint32_t cmask;

    // tag segments / rows for update
    struct glib_window_phy new_reg = {
       .seg_left = seg_l,
       .seg_right = seg_r,
       .row_top = (uint8_t)y_ul_ph,
       .row_bottom = (uint8_t)(y_ul_ph - height + 1)
    };
    tag_upd_reg(&new_reg);

#if (VERBOSE == 3)
    os_printf("buffer info: @(%d|%d); segl: %d, segr: %d, segs: %d, wid: %d, hei: %d\n",
              x_ul_ph, y_ul_ph, seg_l, seg_r, seg_num, width, height);
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
    pix_idx_y = (args & GLIB_DA_FLIPTB) ? height - 1 : 0;
    while (1) {
        if (pix_idx_y < 0 || pix_idx_y > SSD1322_ROWS - 1)  // out of y boundaries
            break;
#if (VERBOSE == 4)
        os_printf("y=%d; currbuf (*currbuf) -> currfb\n cseg:", pix_idx_y);
#endif
        for (curr_seg = 0; curr_seg < seg_num; ++curr_seg) {
            if (curr_seg > SSD1322_SEGMENTS - 1) // out of x boundaries
                continue;

            currfb = to_buf + SSD1322_SEGMENTS * (y_ul_ph - pix_idx_y) + seg_l + curr_seg;
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

        if (args & GLIB_DA_FLIPTB) {
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


inline void glib_setpix(uint32_t *const seg, uint8_t id, const uint8_t value) {
    // clear pix
    *seg &= ~((uint32_t)0xF << ((32 - SSD1322_PIXDEPTH) - id * SSD1322_PIXDEPTH));
    // set new val
    *seg |= (uint32_t)value << ((32 - SSD1322_PIXDEPTH) - id * SSD1322_PIXDEPTH);
}

inline uint8_t glib_getpix(const uint32_t* const seg, const uint8_t id) {
    return (uint8_t)((*seg << id * SSD1322_PIXDEPTH) >> (32 - SSD1322_PIXDEPTH));
}


void ICACHE_FLASH_ATTR glib_reset_display(void) {
    fb_upd_reg = default_region_phy;
    os_delay_us(5000);
    gpio_output_set(0, SSD1322_RESET_PIN, SSD1322_RESET_PIN, 0);
    os_delay_us(5000);
    gpio_output_set(SSD1322_RESET_PIN, 0, SSD1322_RESET_PIN, 0);
    os_delay_us(5000);
#if (VERBOSE > 1)
    os_printf("ssd1322_reset: done\n");
#endif
}

// init SPI & SSD1322_
void ICACHE_FLASH_ATTR glib_init_display(void) {
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
    gpio_output_set(0, SSD1322_DC_PIN, SSD1322_DC_PIN, 0);          // set C/R low
    gpio_output_set(SSD1322_RESET_PIN, 0, SSD1322_RESET_PIN, 0);    // set reset high

    glib_reset_display();

    uint8_t init_commands[] = {
    #if (SSD1322_MODE == 256*64)
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
    glib_clear_disp(0);
}
