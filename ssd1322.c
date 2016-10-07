#include "user_config.h"
#include "ssd1322.h"
#include "driver/spi.h"
#include "user_interface.h"

#define XSTR(x) STR(x)  // convert #define into string
#define STR(x) #x

struct ssd1322_chars_in_fb {
    uint8_t chars[SSD1322_MAX_CHARS];
    uint8_t *last_char;
};

#pragma message "ssd1322 max char buffer size: " XSTR(SSD1322_MAX_CHARS) " bytes"

static uint32_t volatile framebuffer[SSD1322_FBSIZE_INT32] = {0};       // [64][32], row addr x col addr/2 -> 64 x 64*4
static struct ssd1322_window volatile fb_upd_reg;                       // region to upload with update_gram function
static struct ssd1322_window volatile fb_upd_reg_old;
static ssd1322_draw_mode volatile drawmode = SSD1322_DM_TEXT;
static uint16_t volatile xpos = 0, ypos = 0;
static uint8_t volatile write_to_gram = 0;
static struct ssd1322_chars_in_fb chars_in_fb;

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
        gpio_output_set(BIT2, 0, BIT2, 0); // set D/C line high
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
void ssd1322_send_command(const uint8_t *const cmd, const uint8_t cmd_len) {
    write_to_gram = 0;
#if (_SSD1322_IO_ == SSD1322_SPI4WIRE)
    gpio_output_set(0, BIT2, BIT2, 0);  // set D/C line low
    spi_transaction(HSPI, 8, cmd[0], 0, 0, 0, 0, 0, 0);
    os_delay_us(1000);
#if VERBOSE > 1
    os_printf("ssd1322_send_command: sent %x[%d]", cmd[0], cmd_len);
#endif

    uint8_t i = 1;
    if (cmd_len > 1) {
        gpio_output_set(BIT2, 0, BIT2, 0); // set D/C line high
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

void ssd1322_send_command_list(const uint8_t *const cmd_list, const uint8_t list_len) {
    write_to_gram = 0;
    uint8_t i = 0;
#if VERBOSE > 1
    os_printf("ssd1322_send_command_list: sent ");
#endif

#if (_SSD1322_IO_ == SSD1322_SPI4WIRE)
    for (; i < list_len; ++i) {
        gpio_output_set(0, BIT2, BIT2, 0);  // set D/C line low
        spi_transaction(HSPI, 8, cmd_list[i], 0, 0, 0, 0, 0, 0);
        os_delay_us(1000);
#if VERBOSE > 1
        os_printf("|%x", cmd_list[i], cmd_list[i + 1]);
#endif

        if (i + 1 < list_len && cmd_list[i + 1] && i + 1 + cmd_list[i + 1] < list_len) {

#if VERBOSE > 1
            os_printf("[%d]", cmd_list[i + 1]);
#endif

            gpio_output_set(BIT2, 0, BIT2, 0); // set D/C line high
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

void ssd1322_set_area(const struct ssd1322_window *const region) {
    if (region != NULL) {
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
        fb_upd_reg_old = (struct ssd1322_window) {
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

void ssd1322_clear(const uint32_t seg_value) {
#if (_SSD1322_MODE_ == 256*64)
    uint16_t i = 0;
    ssd1322_set_area(NULL); // reset area
    if (!write_to_gram) {
        uint8_t cmd = SSD1322_WRITE;
        ssd1322_send_command(&cmd, 0);
        write_to_gram = 1;
        gpio_output_set(BIT2, 0, BIT2, 0); // set D/C line high
    }
    for (; i < SSD1322_ROWS * SSD1322_SEGMENTS; ++i)
        spi_txd(HSPI, 32, seg_value);
    os_delay_us(1000);
#endif
#if (VERBOSE > 1)
    os_printf("ssd1322_clear: done\n");
#endif
}

void ssd1322_clear_fb(const uint32_t seg_value) {
    fb_upd_reg = (struct ssd1322_window) {
        .seg_left = 0,
        .seg_right = SSD1322_SEGMENTS - 1,
        .row_bottom = 0,
        .row_top = SSD1322_ROWS - 1,
    };
    uint16_t i;
    for (i = 0; i < SSD1322_FBSIZE_INT32; ++i)
        framebuffer[i] = seg_value;
}

void ssd1322_reset(void) {
    xpos = 0;
    ypos = 0;
    fb_upd_reg = (struct ssd1322_window) {
            .seg_left = SSD1322_SEGMENTS - 1,
            .seg_right = 0,
            .row_bottom = SSD1322_ROWS - 1,
            .row_top = 0,
    };

    chars_in_fb.chars[0] = 0;
    chars_in_fb.last_char = chars_in_fb.chars;

    drawmode = SSD1322_DM_TEXT;
    gpio_output_set(0, BIT12, BIT12, 0);
    os_delay_us(5000);
    gpio_output_set(BIT12, 0, BIT12, 0);
    os_delay_us(5000);
    ssd1322_clear(0);
#if (VERBOSE > 1)
    os_printf("ssd1322_reset: done\n");
#endif
}

uint8_t ssd1322_draw(const uint16_t x_ul, const uint16_t y_ul,
             uint32_t *const bitmap, const uint16_t height, const uint16_t width) {

    /* SEGMENTATION of SSD1322:
     * seg ->                     1                         2    ... 32
     * pix      1   2    3    4     5     6     7     8
     * bits    3:0|7:4|11:8|15:12|19:16|23:20|27:24|31:28
     * row 1
     *     2
     *    ...
     *     64
     */

    // => segmentation decode:
    uint8_t seg_l = (uint8_t)(x_ul / 8);  // floor
    uint8_t seg_l_pix = x_ul % 8;
    uint8_t seg_r = (uint8_t)((x_ul + (width - (width ? 1 : 0))) / 8);
    uint8_t seg_r_pix = (x_ul + width) % 8;

    uint8_t seg_num = seg_r - seg_l + 1;   // segments affected per scanline
    uint8_t pix_idx_y = 0;

    uint32_t *currfb;
    uint32_t *currbuf = bitmap;
    uint8_t chbuf_pix_pad = 0;
    uint8_t curr_seg;
    uint32_t cmask;

    // tag segments / rows for update
    if (seg_l < fb_upd_reg.seg_left)
        fb_upd_reg.seg_left = seg_l;
    if (seg_r > fb_upd_reg.seg_right)
        fb_upd_reg.seg_right = seg_r;
    if (y_ul > fb_upd_reg.row_top)
        fb_upd_reg.row_top = y_ul;
    if (y_ul - height < fb_upd_reg.row_bottom)
        fb_upd_reg.row_bottom = y_ul - height + 1;

#if (VERBOSE == 3)
    os_printf("buffer info: @(%d|%d); segl: %d, segr: %d, segs: %d, wid: %d, hei: %d\n",
              x_ul, y_ul, seg_l, seg_r, seg_num, width, height);
    os_printf("bindump:\n");
    uint8_t i;
    for (i = 0; i < (height * width + 15) / 16; ++i)
        os_printf(" 0x%08x\n", bitmap[i]);
    os_printf("\n");
    os_printf("fb: 0x%08x; lpad: %d, rpad: %d\n", framebuffer, seg_l_pix, seg_r_pix);
#endif

    for (pix_idx_y = 0; pix_idx_y < height; ++pix_idx_y) {
#if (VERBOSE == 4)
        os_printf("y=%d\n cseg:", pix_idx_y);
#endif
        for (curr_seg = 0; curr_seg < seg_num; ++curr_seg) {
            currfb = (uint32_t *)framebuffer + SSD1322_SEGMENTS * (y_ul - pix_idx_y) + seg_l + curr_seg;
#if (VERBOSE == 4)
            os_printf(" 0x%08x (0x%08x) -> 0x%08x |", currbuf, *currbuf, currfb);
#endif

            if (!curr_seg) {                        // 1st segment
                cmask = 0xFFFFFFFF >> (seg_l_pix * 4);
                if (seg_num == 1 && width != 8)
                    cmask &= 0xFFFFFFFF << (32 - seg_r_pix * 4);

                //*currfb &= ~cmask;        // clear last few pixel
                *currfb |= ((*currbuf << (chbuf_pix_pad * 4)) >> (seg_l_pix * 4)) & cmask;
                if (seg_num == 1 && width != 8)
                    chbuf_pix_pad += ((seg_r_pix ? seg_r_pix : 8) - seg_l_pix);
                else
                    chbuf_pix_pad += 8 - seg_l_pix;
                if (chbuf_pix_pad >= 8) { // increment pointer to buffer, write remainder
#if (VERBOSE == 4)
                    os_printf("^f");
#endif
                    chbuf_pix_pad -= 8;
                    ++currbuf;
                    if (chbuf_pix_pad)
                        *currfb |= (*currbuf >> ((((seg_num == 1) ? seg_r_pix : 32) - chbuf_pix_pad) * 4)) & cmask;
                }
            } else if (curr_seg && curr_seg == seg_num - 1) {   // last segment
                //*currfb &= (~(0xFFFFFFFF << (32 - seg_r_pix * 4)));   // clear 1st few pixel
                *currfb |= (*currbuf << (chbuf_pix_pad * 4)) & (0xFFFFFFFF << (32 - seg_r_pix * 4));
                chbuf_pix_pad += (seg_r_pix ? seg_r_pix : 8);
                if (chbuf_pix_pad >= 8) { // increment pointer to char buffer, write remainder
#if (VERBOSE == 4)
                    os_printf("^l");
#endif
                    chbuf_pix_pad -= 8;
                    ++currbuf;
                    if (chbuf_pix_pad)
                        *currfb |= (*currbuf >> ((seg_r_pix - chbuf_pix_pad) * 4)) & (0xFFFFFFFF << (32 - seg_r_pix * 4));
                }

            } else {    // segment in between
                //*currfb = 0;
#if (VERBOSE == 4)
                os_printf("^m");
#endif
                *currfb = (*currbuf << (chbuf_pix_pad * 4));
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
#endif
    }
    return 0;
}


static uint32_t chbuf[_FONT_MAX_CHAR_SIZE_INT32_];

uint8_t ssd1322_draw_char(const struct char_info *const chi, const struct font_info *const fnt,
                          const uint16_t x_origin, const uint16_t y_ascend, uint8_t clear_flag) {
    if (y_ascend - fnt->font_ascend < 0) {

#if (VERBOSE > 1)
        os_printf("draw_char: not enough top space!\n");
#endif

        return 1;
    }

#if (VERBOSE > 0)
    uint32_t bench_time = system_get_time();
#endif

    // top -> bottom
    uint16_t y_ul = SSD1322_ROWS - 1 - y_ascend + chi->yoffset;
    uint8_t x_ul = x_origin + chi->xoffset;

#if (VERBOSE > 0)
    os_printf("draw_char: fetch char info of %c: %luus\n", ch, system_get_time() - bench_time);
    bench_time = system_get_time();
#endif
    if (!clear_flag) {
        spi_flash_read(FONT_ADDRESS + chi->address, chbuf, chi->length * sizeof(uint8_t));

#if (VERBOSE > 0)
        os_printf("draw_char: fetch char bin: %luus\n", system_get_time() - bench_time);
#endif

        // switch endianness
        uint16_t bufidx;
#ifdef FONT_SWAP_ENDIANNESS
        for (bufidx = 0; bufidx < _FONT_MAX_CHAR_SIZE_INT32_; ++bufidx)
            chbuf[bufidx] = ((chbuf[bufidx] >> 24) & 0x000000FF) | ((chbuf[bufidx] >> 8) & 0x0000FF00)
                          | ((chbuf[bufidx] << 24) & 0xFF000000) | ((chbuf[bufidx] << 8) & 0x00FF0000);
#endif

    // sort by pixels
#if (FONT_SORT_PIXELS == 1)
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
        os_printf("charinfo: addr: 0x%08x, len: %d, wid: %d, hei: %d\n",
                  chi.address, chi.length, chi.width, chi.height);
#endif
    } else {
        uint8_t i = 0;
        uint8_t fntlen32 = (chi->length + 3) / 4;
        for (; i < fntlen32; ++i)
            chbuf[i] = 0;
#if (VERBOSE > 1)
        os_printf("del charinfo: addr: 0x%08x, len: %d, wid: %d, hei: %d\n",
                  chi.address, chi.length, chi.width, chi.height);
#endif
    }
    ssd1322_draw((uint16_t)x_ul, y_ul, chbuf, (uint16_t)chi->height, (uint16_t)chi->width);

    return 0;
}

// draw string inside selected area with offsets
uint8_t ssd1322_print(const uint8_t* string, const uint16_t x_l, const uint16_t y_asc) {
    uint16_t chr_idx;
    uint16_t currx, curry;
    if (!xpos && !ypos) {
        currx = x_l;
        curry = y_asc;
    } else {
        currx = xpos;
        curry = ypos;
    }
    struct font_info fnti;
    get_font_info(&fnti);
    struct char_info chi;
    uint8_t override_last_char = *chars_in_fb.last_char;
    os_printf("charsinfb: ");
    uint8_t i = 0;
    for (; i < SSD1322_MAX_CHARS; ++i)
        os_printf("%c", chars_in_fb.chars[i]);
    os_printf("\n");

    for (chr_idx = 0; string[chr_idx] != '\0'; ++chr_idx) {
        get_char(&chi, string[chr_idx]);

        // boundary checks
        if (currx + chi.advance >= SSD1322_SEGMENTS * 8) {  // newline type 1
            override_last_char = 0;
            curry += fnti.font_height;
            currx = x_l;
            // skip next newline / space
            if (string[chr_idx + 1] == '\n' || string[chr_idx + 1] == ' ')
                ++chr_idx;
        }
        if (string[chr_idx] == '\n') {  // newline type 2
            override_last_char = 0;
            curry += fnti.font_height;
            currx = x_l;
            continue;
        }
        if (string[chr_idx] == '\r') {  // carriage return
            override_last_char = 0;
            currx = x_l;
            continue;
        }
        if (string[chr_idx] == 8) {     // backspace
            if (chars_in_fb.last_char > chars_in_fb.chars) {
                --chars_in_fb.last_char;
                override_last_char = *chars_in_fb.last_char;
                struct char_info lchi;
                get_char(&lchi, *chars_in_fb.last_char);
                currx -= lchi.advance;
                ssd1322_draw_char(&lchi, &fnti, currx, curry, 1);
            }
            continue;
        }
        if (curry - fnti.font_ascend + fnti.font_height >= SSD1322_ROWS) { // no space left, newline type 3
#if (VERBOSE > 1)
            os_printf("ssd1322_print: no space left (y = %d, need %d)\n", curry, curry - fnti.font_ascend + fnti.font_height);
#endif
            override_last_char = 0;
            if (drawmode == SSD1322_DM_TEXT) {
                xpos = currx;
                ypos = curry;
                return 1;
            } else if (drawmode == SSD1322_DM_TEXT_CLR) {
                ssd1322_clear_fb(0);
                xpos = 0;
                ypos = 0;
                currx = x_l;
                curry = y_asc;
            } else {
                return 1;
            }
        }

        // only kerning if no new line
        currx += get_kerning(override_last_char, string[chr_idx]);
        if ((int32_t)currx + (int32_t)chi.xoffset < 0)
            currx -= chi.xoffset;

        if (ssd1322_draw_char(&chi, &fnti, currx, curry, 0)) {
#if (VERBOSE > 1)
            os_printf("ssd1322_print: skipped '%c'\n", string[chr_idx]);
#endif
            continue;
        }
        currx += chi.advance;
        override_last_char = string[chr_idx];
//        ++chars_in_fb.last_char;
//        *chars_in_fb.last_char = string[chr_idx];
    }
    if (drawmode != SSD1322_DM_FREE) {
        xpos = currx;
        ypos = curry;
    }
    return 0;
}

void ssd1322_set_cursor(const uint16_t x_l, const uint16_t y_asc) {
    drawmode = SSD1322_DM_TEXT;
    xpos = x_l;
    ypos = y_asc;
}

void ssd1322_set_mode(const ssd1322_draw_mode dm) {
    drawmode = dm;
}

void ssd1322_update_gram() {
#if (VERBOSE > 1)
    os_printf("update_gram... segl: %d ... segr: %d; rowb: %d ... rowt: %d\n", fb_upd_reg.seg_left, fb_upd_reg.seg_right,
              fb_upd_reg.row_bottom, fb_upd_reg.row_top);
#endif
#if (VERBOSE > 0)
    uint32_t bench_stime = system_get_time();
#endif

    // set window
    ssd1322_set_area((struct ssd1322_window *)&fb_upd_reg);

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
            ssd1322_send_data((uint32_t *)&framebuffer[SSD1322_SEGMENTS * row_idx + fb_upd_reg.seg_left],
                    fb_upd_reg.seg_right - fb_upd_reg.seg_left + 1);
        }
    }

    // reset window
    fb_upd_reg = (struct ssd1322_window) {
            .seg_left = SSD1322_SEGMENTS - 1,
            .seg_right = 0,
            .row_bottom = SSD1322_ROWS - 1,
            .row_top = 0,
    };

#if (VERBOSE > 0)
    os_printf("send fb: %luus\n", system_get_time() - bench_stime);
#endif
}


// init SPI & SSD1322_
void ssd1322_init(void) {
    // SPI config
    spi_init(HSPI);
    spi_mode(HSPI, 1, 0); // 'trailing edge': positive edge
    spi_init_gpio(HSPI, SPI_CLK_USE_DIV);
    // SPI_CLK_FREQ = CPU_CLK_FREQ/(SPI_CLK_PREDIV*SPI_CLK_CNTDIV) -> 80MHz / 8 = 10MHz -> 2 / 4
    // (CPU_CLK_FREQ is actually peripheral clock and ALWAYS 80MHz, maybe different from cpu clock, eg. 160MHz)
    spi_clock(HSPI, 2, 4);
    spi_tx_byte_order(HSPI, SPI_BYTE_ORDER_HIGH_TO_LOW);

    // GPIO config
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
    gpio_output_set(0, BIT2, BIT2, 0);      // set C/R low
    gpio_output_set(BIT12, 0, BIT12, 0);    // set reset high

    ssd1322_reset();
    ssd1322_set_mode(SSD1322_DM_TEXT);

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
    ssd1322_send_command_list(&init_commands[0], sizeof(init_commands) / sizeof(uint8_t));

    ssd1322_clear(0);
}
