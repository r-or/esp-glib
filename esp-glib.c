/*
 * esp-glib.c
 * License: MIT / x11
 * Copyright 2016 Tobias Däullary
 *
 */

#include "esp-glib.h"
#include "user_interface.h"
#include "osapi.h"
#include "mem.h"

#define XSTR(x) STR(x)  // convert #define into string
#define STR(x) #x

//#define DBG_SHOW_TEXTBOX_BORDER
//#define DBG_SHOW_TEXT_EXTENT

#pragma message "max char buffer size: " XSTR(GLIB_MAX_CHARS) " qbytes"
#pragma message "pix int32: " XSTR(GLIB_PIX_INT32)
#pragma message "fb size: " XSTR(GLIB_FBSIZE_INT32) " (" XSTR(GLIB_DISP_COLS_INT32) "x" XSTR(GLIB_DISP_ROWS) ")"

static uint32_t volatile framebuffer[GLIB_FBSIZE_INT32] = {0};       // [64][32], row addr x col addr/2 -> 64 x 64*4
static uint32_t background_pattern = 0;
static fnt_id font;

static const struct glib_window region_full = {
    .x_left = GLIB_DISP_COL_LOWER,
    .x_right = GLIB_DISP_COL_UPPER,
    .y_bottom = GLIB_DISP_ROW_UPPER,
    .y_top = GLIB_DISP_ROW_LOWER,
};

static void ICACHE_FLASH_ATTR
dbg_dump_fb(const struct glib_window *region) {
    int16_t row = 0, pix = 0;
    uint8_t pix_id;
    uint32_t *cbuf;
    os_printf("framebuffer dump:\nrow\tpix %d .. %d\n", region->x_left, region->x_right);
    for (row = region->y_top; row <= region->y_bottom; ++row) {
        os_printf("%03d\t", row);
        for (pix = region->x_left; pix <= region->x_right; ++pix) {
            pix_id = pix % GLIB_PIX_INT32;
            cbuf = (uint32_t *)framebuffer
                    + GLIB_DISP_COLS_INT32 * glib_row_log2phys(row) + glib_col_log2phys(pix) / GLIB_PIX_INT32;
            os_printf("%x", glib_getpix(cbuf, pix_id));
        }
        os_printf("\n");
    }
    system_soft_wdt_feed();
}

static void ICACHE_FLASH_ATTR
dbg_print_utf8(const uint32_t *const string) {
    uint8_t idx = 0;
    for (idx = 0; string[idx] != '\0'; ++idx) {
        if (string[idx] < 128)
            os_printf("%c", string[idx]);
        else
            os_printf("(U+%x)", string[idx]);
    }
}

uint8_t ICACHE_FLASH_ATTR
glib_transform(uint32_t *const buf, const uint16_t height, const uint16_t width, const uint8_t dim,
               const glib_draw_args args) {
    uint16_t rowcnt, bufidx;
    uint16_t buflen = (height * width + GLIB_PIX_INT32 - 1) / GLIB_PIX_INT32;
    uint8_t segs = (uint8_t)((width + GLIB_PIX_INT32 - 1) / GLIB_PIX_INT32);
    uint32_t *currbuf;
    uint32_t *cbufl, *cbufr;
    uint16_t pixl, pixr;
    uint8_t segl_pixid, segr_pixid, tmp;
    uint8_t coldelta = 0xF - (dim & 0xF);

#if (VERBOSE > 1)
    os_printf("glib_transform: args: 0x%04x; dim: %d\n"
              " buf @ 0x%08x, height: %d, width: %d\n", args, dim, buf, height, width);
#endif

    for (bufidx = 0; args && args != GLIB_DA_FLIPLR && bufidx < buflen; ++bufidx) {
        currbuf = buf + bufidx;
        if (args & GLIB_DA_SWENDIAN) {
            *currbuf = ((*currbuf >> 24) & 0x000000FF) | ((*currbuf >> 8) & 0x0000FF00)
                     | ((*currbuf << 24) & 0xFF000000) | ((*currbuf << 8) & 0x00FF0000);
        }

        if (args & GLIB_DA_INVERT)
            *currbuf = ~(*currbuf);
    }
    for (rowcnt = 0; (args || dim) && rowcnt < height; ++rowcnt) {
        if (args & GLIB_DA_FLIPLR || dim) {
            for (pixl = 0, pixr = width - 1; pixl < width / 2; ++pixl, --pixr) {
                cbufl = &(buf[rowcnt * segs + pixl / GLIB_PIX_INT32]);
                cbufr = &(buf[rowcnt * segs + pixr / GLIB_PIX_INT32]);
                segl_pixid = pixl % GLIB_PIX_INT32;
                segr_pixid = pixr % GLIB_PIX_INT32;
                //os_printf("pixl: %d[%d], pixr: %d[%d]\n", cbufl - buf, segl_pixid, cbufr - buf, segr_pixid);
                // switch pixr with pixl
                if (args) {
                    tmp = glib_getpix(cbufl, segl_pixid);
                    glib_setpix(cbufl, segl_pixid, glib_getpix(cbufr, segr_pixid));
                    glib_setpix(cbufr, segr_pixid, tmp);
                }
                // substract coldelta if appropriate
                if (dim) {
                    tmp = glib_getpix(cbufl, segl_pixid);
                    glib_setpix(cbufl, segl_pixid, (tmp > coldelta) ? tmp - coldelta : 0);
                    tmp = glib_getpix(cbufr, segr_pixid);
                    glib_setpix(cbufr, segr_pixid, (tmp > coldelta) ? tmp - coldelta : 0);
                }
            }
        }
    }
    return 0;
}

inline void ICACHE_FLASH_ATTR
region_prune(struct glib_window *const region) {
    region->x_left = (region->x_left < 0) ? 0 : region->x_left;
    region->x_right = (region->x_right > GLIB_DISP_COL_UPPER) ? GLIB_DISP_COL_UPPER : region->x_right;
    region->y_top = (region->y_top < 0) ? 0 : region->y_top;
    region->y_bottom = (region->y_bottom > GLIB_DISP_ROW_UPPER) ? GLIB_DISP_ROW_UPPER : region->y_bottom;
}

uint8_t ICACHE_FLASH_ATTR
glib_translate(const struct glib_window *const region, const int16_t x, const int16_t y) {
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
    struct glib_window upd_region;
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
    glib_tag_upd_reg_log(&upd_region);
    uint32_t *cbufr, *cbufw;            // addresses of read / write segments:
    uint8_t segr_pixid, segw_pixid;     // ID of pixel to be read and to be written

    crow = row_s;
    for (crow = row_s; crow != row_e; crow += row_inc) {
        if (crow + y < 0 || crow + y > GLIB_DISP_ROW_UPPER) { // out of y boundary
#if (VERBOSE > 2)
            os_printf("transl: skip row %d\n", crow + y);
#endif
            continue;
        }
        for (ccol = col_s; ccol != col_e; ccol += col_inc) {
            segr_pixid = ccol % GLIB_PIX_INT32;
            segw_pixid = (ccol + x) % GLIB_PIX_INT32;
            if (ccol + x < 0 || ccol + x > GLIB_DISP_COL_UPPER) { // out of x boundary
#if (VERBOSE > 2)
                os_printf("transl: skip col %d\n", ccol + x);
#endif
                continue;
            }
            cbufr = (uint32_t *)framebuffer
                    + GLIB_DISP_COLS_INT32 * glib_row_log2phys(crow) + glib_col_log2phys(ccol) / GLIB_PIX_INT32;
            cbufw = (uint32_t *)framebuffer
                    + GLIB_DISP_COLS_INT32 * glib_row_log2phys(crow + y) + glib_col_log2phys(ccol + x) / GLIB_PIX_INT32;
            glib_setpix(cbufw, segw_pixid, glib_getpix(cbufr, segr_pixid));   // copy to target
            glib_setpix(cbufr, segr_pixid, 0);                           // clear source
        }
    }
#if (VERBOSE > 1)
    system_soft_wdt_feed();
#endif

    return 0;
}

inline int32_t
round_up(const float val) {
    return (int32_t)((double)val + 1.0);
}

inline double
decimal(const float val) {
    return (val < 0 ? -1.0 : 1.0) * (double)val - (double)((int32_t)val);
}

struct quad_pixmap {
    int8_t x;
    int8_t y;
};

struct q16 {
    uint16_t frac;
    int16_t scale;
};

typedef union {
    uint16_t uintrep;
    struct {
        uint16_t sign   : 1,
                 bit14  : 1,
                 bit13  : 1,
                 bit12  : 1,
                 bit11  : 1,
                 bit10  : 1,
                 bit9   : 1,
                 bit8   : 1,
                 bit7   : 1,
                 bit6   : 1,
                 bit5   : 1,
                 bit4   : 1,
                 bit3   : 1,
                 bit2   : 1,
                 bit1   : 1,
                 bit0   : 1;
    } bits;
} int16_custom;

struct q15_16 {
    int16_custom integer;
    uint16_t frac;
};

struct q16 ICACHE_FLASH_ATTR
decfloat2q16(const float *const val) {
    float decval = (float)decimal(*val);
    decval *=  (1 << 16);
    return (struct q16) { // 'good enough' solution: doesn't actually round!
        .frac = (uint16_t)decval,
        .scale = -10,
    };

    int16_custom bla;
    bla.bits.sign = 1;
}

struct q16 ICACHE_FLASH_ATTR
int2q16(int32_t val) {
    uint8_t neg = val < 0;
    if (neg)
        val *= -1;
    int32_t msb = __builtin_clz((uint32_t)val);
    return (struct q16) {
        .frac = val * (1 << 16) / (1 << msb),
        .scale = msb * (neg ? -1 : 1)
    };
}

int16_t
q16quantise(struct q16 val, const int16_t range) {
    int32_t scale = (1 << 16) / range;
    return (val.frac / scale) * val.scale;
}

struct q16
q16mult(const struct q16 *const val1, const struct q16 *const val2) {
    return (struct q16) {
        .frac = val1->frac * val2->frac,
        .scale = val1->scale * val2->scale
    };
}

void
q16match(struct q16 *const val1, struct q16 *const val2) {
    if (val1->scale < val2->scale) {
        int16_t tmp = val2->scale / val1->scale;
        val1->scale *= tmp;
        val1->frac *= tmp;
    } else if (val2->scale < val1->scale) {
        int16_t tmp = val1->scale / val2->scale;
        val2->scale *= tmp;
        val2->frac *= tmp;
    }
}

struct q16
q16add(struct q16 val1, struct q16 val2) {
    q16match(&val1, &val2);
    return (struct q16) {
        .frac = val1.frac + val2.frac,
        .scale = val1.scale,
    };
}

struct q16
q16sub(struct q16 val1, struct q16 val2) {
    q16match(&val1, &val2);
    return (struct q16) {
        .frac = val1.frac - val2.frac,
        .scale = val1.scale,
    };
}

uint8_t ICACHE_FLASH_ATTR
glib_translate_subpix(const struct glib_window *const region, const float x, const float y) {
    uint32_t bench = system_get_time();
#if (VERBOSE > 1)
    os_printf("translate: region\n"
              " .xleft: %d\n"
              " .xright: %d\n"
              " .ytop: %d\n"
              " .ybottom: %d\n"
              "by (%d|%d)\n",
              region->x_left, region->x_right, region->y_top, region->y_bottom, x, y);
#endif

    //if (!x && !y)
    //    return 0;

    // inplace.. RAM is PRECIOUS
    int16_t row_s, row_e, col_s, col_e;
    int8_t row_inc, col_inc;

    // calc all quarters our pixel gets mapped to
    float mult_tl, mult_tr, mult_bl, mult_br;
    double dec_x = decimal(x);
    double dec_y = decimal(y);
    int16_t int_x = (int16_t)x;
    int16_t int_y = (int16_t)y;
    float perm1 = (float)((1.0 - dec_y) * (1.0 - dec_x));
    float perm2 = (float)((1.0 - dec_y) * dec_x);
    float perm3 = (float)(dec_y * (1.0 - dec_x));
    float perm4 = (float)(dec_y * dec_x);
    struct quad_pixmap tl_far, tr_far, bl_far, br_far; // corners which need offset of 1

    // check in which direction we can copy (PHYSICAL layout!! => top/bottom flip)
    if (x >= 0 && y >= 0) {             // phys bottom right -> top left
        row_s = region->y_bottom;
        row_e = region->y_top - 1;
        row_inc = -1;
        col_s = region->x_right;
        col_e = region->x_left - 1;
        col_inc = -1;
        mult_tl = perm1;
        mult_tr = perm2;
        mult_bl = perm3;
        mult_br = perm4;
        tl_far.x = 0; tl_far.y = 0;
        tr_far.x = 1; tr_far.y = 0;
        bl_far.x = 0; bl_far.y = -1;
        br_far.x = 1; br_far.y = -1;
    } else if (x >= 0 && y < 0) {        // tr -> bl
        row_s = region->y_top;
        row_e = region->y_bottom + 1;
        row_inc = 1;
        col_s = region->x_right;
        col_e = region->x_left - 1;
        col_inc = -1;
        mult_tl = perm3;
        mult_tr = perm4;
        mult_bl = perm1;
        mult_br = perm2;
        tl_far.x = 0; tl_far.y = 1;
        tr_far.x = 1; tr_far.y = 1;
        bl_far.x = 0; bl_far.y = 0;
        br_far.x = 1; br_far.y = 0;
    } else if (x <= 0 && y >= 0) {      // bl -> tr
        row_s = region->y_bottom;
        row_e = region->y_top - 1;
        row_inc = -1;
        col_s = region->x_left;
        col_e = region->x_right + 1;
        col_inc = 1;
        mult_tl = perm2;
        mult_tr = perm1;
        mult_bl = perm4;
        mult_br = perm3;
        tl_far.x = -1; tl_far.y = 0;
        tr_far.x = 0; tr_far.y = 0;
        bl_far.x = -1; bl_far.y = -1;
        br_far.x = -1; br_far.y = 0;
    } else if (x < 0 && y < 0) {        // tl -> br
        row_s = region->y_top;
        row_e = region->y_bottom + 1;
        row_inc = 1;
        col_s = region->x_left;
        col_e = region->x_right + 1;
        col_inc = 1;
        mult_tl = perm4;
        mult_tr = perm3;
        mult_bl = perm2;
        mult_br = perm1;
        tl_far.x = -1; tl_far.y = 1;
        tr_far.x = 0; tr_far.y = 1;
        bl_far.x = -1; bl_far.y = 0;
        br_far.x = 0; br_far.y = 0;
    } else {
        return 1;
    }

    struct q16 qmult_tl = decfloat2q16(&mult_tl);
    struct q16 qmult_tr = decfloat2q16(&mult_tr);
    struct q16 qmult_bl = decfloat2q16(&mult_bl);
    struct q16 qmult_br = decfloat2q16(&mult_br);

    // tag 4 update
    struct glib_window upd_region;
    upd_region.x_left = (x >= 0) ? region->x_left : region->x_left + (int16_t)round_up(x);
    upd_region.x_right = (x >= 0) ? region->x_right + (int16_t)round_up(x) : region->x_right;
    upd_region.y_top = (y >= 0) ? region->y_top : region->y_top + (int16_t)round_up(y);
    upd_region.y_bottom = (y >= 0) ? region->y_bottom + (int16_t)round_up(y) : region->y_bottom;
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
    glib_tag_upd_reg_log(&upd_region);
    // addresses of read / write segments:
    uint32_t *cbufr;
    uint32_t *cbufw_tl, *cbufw_tr, *cbufw_bl, *cbufw_br;
    // ID of pixel to be read and to be written
    uint8_t segr_pixid;
    uint8_t segw_pixid_l, segw_pixid_r;
    int16_t crow, ccol;

//    os_printf("dump before:\n");
//    dbg_dump_fb(&region_full);


    os_printf("subpix translation setup: %luus\n", system_get_time() - bench);
    bench = system_get_time();

    crow = row_s;
    for (crow = row_s; crow != row_e; crow += row_inc) {
        if (crow + int_y < 0 || crow + int_y > GLIB_DISP_ROW_UPPER) { // out of y boundary
#if (VERBOSE > 2)
            os_printf("transl: skip row %d\n", crow + y);
#endif
            continue;
        }
        for (ccol = col_s; ccol != col_e; ccol += col_inc) {
            segr_pixid = ccol % GLIB_PIX_INT32;
            // choose segment id of left pixels and right pixels affected
            segw_pixid_l = (ccol + int_x - (x > 0 ? 0 : 1)) % GLIB_PIX_INT32;
            segw_pixid_r = (segw_pixid_l + 1) % GLIB_PIX_INT32;
            if (ccol + int_x < 0 || ccol + int_x > GLIB_DISP_COL_UPPER) { // out of x boundary
#if (VERBOSE > 2)
                os_printf("transl: skip col %d\n", ccol + x);
#endif
                continue;
            }
            cbufr = (uint32_t *)framebuffer
                    + GLIB_DISP_COLS_INT32 * glib_row_log2phys(crow) + glib_col_log2phys(ccol) / GLIB_PIX_INT32;
            if (*cbufr == 0) // qick & dirty speedup
                continue;

            cbufw_tl = (uint32_t *)framebuffer
                    + GLIB_DISP_COLS_INT32 * glib_row_log2phys(crow + int_y + tl_far.y)
                    + glib_col_log2phys(ccol + int_x + tl_far.x) / GLIB_PIX_INT32;
            cbufw_tr = (uint32_t *)framebuffer
                    + GLIB_DISP_COLS_INT32 * glib_row_log2phys(crow + int_y + tr_far.y)
                    + glib_col_log2phys(ccol + int_x + tr_far.x) / GLIB_PIX_INT32;
            cbufw_bl = (uint32_t *)framebuffer
                    + GLIB_DISP_COLS_INT32 * glib_row_log2phys(crow + int_y + bl_far.y)
                    + glib_col_log2phys(ccol + int_x + bl_far.x) / GLIB_PIX_INT32;
            cbufw_br = (uint32_t *)framebuffer
                    + GLIB_DISP_COLS_INT32 * glib_row_log2phys(crow + int_y + br_far.y)
                    + glib_col_log2phys(ccol + int_x + br_far.x) / GLIB_PIX_INT32;


            uint8_t oldval; // to make accumulation possible -> only overwrite 'master' pixel
            if (ccol + int_x + tl_far.x >= 0 && ccol + int_x + tl_far.x <= GLIB_DISP_COL_UPPER
                    && crow + int_y + tl_far.y >= 0 && crow + int_y + tl_far.y<= GLIB_DISP_ROW_UPPER) {
                oldval = (tl_far.x == 0 && tl_far.y == 0) ? 0 : glib_getpix(cbufw_tl, segw_pixid_l);
                glib_setpix(cbufw_tl, segw_pixid_l, (uint8_t)(glib_getpix(cbufr, segr_pixid) * mult_tl + oldval));
            }
            if (ccol + int_x + tr_far.x >= 0 && ccol + int_x + tr_far.x <= GLIB_DISP_COL_UPPER
                    && crow + int_y + tr_far.y >= 0 && crow + int_y + tr_far.y<= GLIB_DISP_ROW_UPPER) {
                oldval = (tr_far.x == 0 && tr_far.y == 0) ? 0 : glib_getpix(cbufw_tr, segw_pixid_r);
                glib_setpix(cbufw_tr, segw_pixid_r, (uint8_t)(glib_getpix(cbufr, segr_pixid) * mult_tr + oldval));
            }
            if (ccol + int_x + bl_far.x >= 0 && ccol + int_x + bl_far.x <= GLIB_DISP_COL_UPPER
                    && crow + int_y + bl_far.y >= 0 && crow + int_y + bl_far.y<= GLIB_DISP_ROW_UPPER) {
                oldval = (bl_far.x == 0 && bl_far.y == 0) ? 0 : glib_getpix(cbufw_bl, segw_pixid_l);
                glib_setpix(cbufw_bl, segw_pixid_l, (uint8_t)(glib_getpix(cbufr, segr_pixid) * mult_bl + oldval));
            }
            if (ccol + int_x + br_far.x >= 0 && ccol + int_x + br_far.x <= GLIB_DISP_COL_UPPER
                    && crow + int_y + br_far.y >= 0 && crow + int_y + br_far.y<= GLIB_DISP_ROW_UPPER) {
                oldval = (br_far.x == 0 && br_far.y == 0) ? 0 : glib_getpix(cbufw_br, segw_pixid_r);
                glib_setpix(cbufw_br, segw_pixid_r, (uint8_t)(glib_getpix(cbufr, segr_pixid) * mult_br + oldval));
            }
            if (cbufr != cbufw_bl && cbufr != cbufw_br && cbufr != cbufw_tl && cbufr != cbufw_tr)
                glib_setpix(cbufr, segr_pixid, 0);                           // clear source
        }
    }
    os_printf("subpix translation loop: %luus\n", system_get_time() - bench);
//    os_printf("dump after:\n");
//    dbg_dump_fb(&region_full);
#if (VERBOSE > 1)
    system_soft_wdt_feed();
#endif

    return 0;
}

void ICACHE_FLASH_ATTR
glib_draw_rect(const struct glib_window *const border, const uint32_t pattern) {
    uint16_t height = border->y_bottom - border->y_top;
    uint16_t width = border->x_right - border->x_left;
    uint16_t size = (width > height) ?
                (width + GLIB_PIX_INT32 - 1) / GLIB_PIX_INT32 : (height + GLIB_PIX_INT32 - 1) / GLIB_PIX_INT32;

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
    glib_draw((uint32_t *)framebuffer, border->x_left, border->y_top, temp, 1, width, GLIB_DA_NONE);
    glib_draw((uint32_t *)framebuffer, border->x_left, border->y_bottom, temp, 1, width, GLIB_DA_NONE);

    // left & right
    glib_draw((uint32_t *)framebuffer, border->x_left, border->y_top, temp, height, 1, GLIB_DA_NONE);
    glib_draw((uint32_t *)framebuffer, border->x_right, border->y_top, temp, height, 1, GLIB_DA_NONE);

    os_free(temp);
}


//static uint32_t bmbuf[SSD1322_FBSIZE_INT32];
// TODO: maybe read line-wise to safe RAM -> TODO^2: 4byte align bmp by width
// FIX#1: dynamic allocation for now

uint8_t ICACHE_FLASH_ATTR
glib_draw_bitmap(const uint16_t x_ul, const uint16_t y_ul, uint32_t address,
                 const uint16_t height, const uint16_t width, const glib_draw_args args) {
    if (width > GLIB_DISP_COLS || height > GLIB_DISP_ROWS) {
#if (VERBOSE > 0)
        os_printf("glib_draw_bitmap: bitmap doesn't fit!\n");
#endif
        return 1;
    }
    const uint16_t buflen = (height * width + GLIB_PIX_INT8 - 1) / GLIB_PIX_INT8;
    uint32_t *const bmtempbuf = (uint32_t *)os_malloc(buflen * sizeof(uint8_t));

    spi_flash_read(BMP_ADDRESS + address, bmtempbuf, buflen * sizeof(uint8_t));
    glib_transform(bmtempbuf, height, width, 0, args);
    glib_draw((uint32_t *)framebuffer, x_ul, y_ul, bmtempbuf, height, width, args);

#if (VERBOSE > 1)
        os_printf("glib_draw_bitmap: drawn @ %d, %d, len: %d\n",
                  glib_col_log2phys(x_ul), glib_row_log2phys(y_ul), height * width);
#endif
#if (VERBOSE > 2)
    dbg_dump_fb(&region_full);
#endif
    os_free(bmtempbuf);
    return 0;
}


static uint32_t chbuf[FNT_MAX_CHAR_SIZE_INT32];

static uint8_t ICACHE_FLASH_ATTR
glib_draw_char(const struct fnt_char_info *const chi, const struct fnt_info *const fnt,
               const uint16_t x_origin, const uint16_t y_ascend, const glib_draw_args args) {
    if (y_ascend - fnt->ascend < 0) { // TODO: more security checks

#if (VERBOSE > 1)
        os_printf("draw_char: not enough top space!\n");
#endif

        return 1;
    }

#if (VERBOSE > 1)
    uint32_t bench_time = system_get_time();
#endif

    // top -> bottom
    uint16_t y_ul = y_ascend - chi->yoffset;
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

    glib_transform(chbuf, chi->height, chi->width, 0, args);
    glib_draw((uint32_t *)framebuffer, (uint16_t)x_ul, y_ul, chbuf, (uint16_t)chi->height, (uint16_t)chi->width, args);
    return 0;
}


// FONT related states
static uint8_t line_count = 0;
static struct fnt_info fnt_current;
static struct fnt_char_info dotinfo;
static struct glib_window textbox;
static glib_draw_mode txt_drawmode = GLIB_DM_TEXT;
static uint16_t txt_xpos = 0, txt_ypos = 0;
static struct glib_chars_in_fb chars_in_fb;

void ICACHE_FLASH_ATTR
glib_clear_tb_txt_state(void) {
    txt_xpos = textbox.x_left;
    txt_ypos = textbox.y_top;
    chars_in_fb.chars[0] = 0;
    chars_in_fb.last_char = chars_in_fb.chars;
}

void ICACHE_FLASH_ATTR
glib_set_textbox(const struct glib_window *const region) {
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

    fnt_get_info(&fnt_current, font);
    fnt_get_char(&dotinfo, '.', font);
    line_count = (textbox.y_bottom - textbox.y_top) / fnt_current.height;
}

void ICACHE_FLASH_ATTR
glib_set_cursor(const uint16_t x_l, const uint16_t y_asc) {
    txt_drawmode = GLIB_DM_TEXT;
    txt_xpos = x_l;
    txt_ypos = y_asc;
}

void ICACHE_FLASH_ATTR
glib_set_mode(const glib_draw_mode dm) {
    txt_drawmode = dm;
}


static inline uint8_t
ishex(uint8_t chr) {
    return (chr >= '0' && chr <= '9') ||
            (chr >= 'A' && chr <= 'F') ||
            (chr >= 'a' && chr <= 'f');
}

static inline uint8_t
hex2int(uint8_t chr) {
    if (chr >= '0' && chr <= '9')
        return chr - '0';
    if (chr >= 'A' && chr <= 'F')
        return chr - 'A' + 10;
    if (chr >= 'a' && chr <= 'f')
        return chr - 'a' + 10;
    return 0;
}

static void
url_unescape(uint8_t *const string_destination, const uint8_t *const string_source, uint16_t dest_len) {
    uint16_t chr_idx, tchr_idx;
    for (chr_idx = 0, tchr_idx = 0; string_source[chr_idx] != '\0' && tchr_idx < dest_len - 1; ++chr_idx, ++tchr_idx) {
        if (string_source[chr_idx] == '%' && ishex(string_source[chr_idx + 1]) && ishex(string_source[chr_idx + 2])) {
            string_destination[tchr_idx] = hex2int(string_source[1]) * 0x10 + hex2int(string_source[2]);
            chr_idx += 2;
        } else {
            string_destination[tchr_idx] = string_source[chr_idx];
        }
    }
    string_destination[tchr_idx] = '\0';
}

static int8_t
utf8_unescape(const uint8_t *string, uint32_t *const target) {
    if ((string[0] == 'u' || string[1] == 'U') && string[1] == '+') {
        int8_t utflen = 0, curr_utfpos;
        uint32_t exp = 1;
        for (; ishex(string[2 + utflen]) && utflen < 8; ++utflen);   // utf-8: 8 hex digits at most
        if (string[0] == 'u' && utflen > 3)                          // u+ is 4 hex
            utflen = 3;
        *target = 0;
        for (curr_utfpos = utflen; curr_utfpos >= 0; --curr_utfpos) {
            *target += hex2int(string[2 + curr_utfpos]) * exp;
            exp *= 0x10;
        }
#if (VERBOSE > 1)
        os_printf("unescaped utf8: U+%x\n", *target);
#endif
        return 2 + utflen;
    }

    *target = (uint32_t)string[0];
    return 0;
}

// string buffer for unescaping, preprocessing, ...
static uint8_t strbuf[GLIB_MAX_CHARS * 2] = {0};
static uint32_t strbuf32[GLIB_MAX_CHARS] = {0};

uint8_t ICACHE_FLASH_ATTR
glib_print(const uint8_t *string, const uint16_t x_l, const uint16_t y_asc, const glib_txt_position txtpos,
           const glib_draw_args args, uint16_t *x_l_re, uint16_t *y_asc_re) {
#if (VERBOSE > 1)
    os_printf("print: '%s'\n", string);
#endif

    // 1st: check for URI escapes (%xx)
    url_unescape(strbuf, string, GLIB_MAX_CHARS);

#if (VERBOSE > 1)
    os_printf("print after uri escape: '%s'\n", strbuf);
#endif

    // 2nd: check for UTF8 escapes (\Uxxxxxxxx or \uxxxx)
    uint16_t chr_idx = 0;
    uint16_t chr_fb_idx = 0;
    for (chr_idx = 0; strbuf[chr_idx] != '\0'; ++chr_idx, ++chr_fb_idx)
        chr_idx += utf8_unescape(&strbuf[chr_idx], &strbuf32[chr_fb_idx]);
    strbuf32[chr_fb_idx] = '\0'; // terminate with zero
    return glib_print_utf8(strbuf32, x_l, y_asc, txtpos, args, x_l_re, y_asc_re);
}

/* TODO
static void ICACHE_FLASH_ATTR
string_preproc_utf8(const uint32_t *utf8string) {
    uint16_t chr_idx;
    uint32_t cchr;

    for (chr_idx = 0; utf8string[chr_idx] != '\0'; ++chr_idx) {
        cchr = utf8string[chr_idx];
        if (cchr == 8) {    // backspace

        }
    }
}
*/

uint8_t ICACHE_FLASH_ATTR
glib_print_utf8(const uint32_t *utf8string, const uint16_t x_l, const uint16_t y_asc, const glib_txt_position txtpos,
                const glib_draw_args args, uint16_t *x_l_re, uint16_t *y_asc_re) {
    uint16_t chr_idx;
    uint16_t currx, curry;
    uint16_t x_l_tb = x_l + textbox.x_left;
    uint16_t y_asc_tb;      // x, y corrected by textbox offsets
    if (!(txtpos & GLIB_TP_APPEND) && y_asc - fnt_current.max_char_ascent < 0) {
        if (txtpos & GLIB_TP_TOPMOST) {
            y_asc_tb = textbox.y_top + fnt_current.max_char_ascent;
        } else {
            os_printf("glib_print: not enough top space for text to fit!\n");
            return 1;
        }
    } else {
        y_asc_tb = y_asc + textbox.y_top;
    }
    struct fnt_char_info chi;
    uint32_t override_last_char = (txt_drawmode != GLIB_DM_FREE) ? *chars_in_fb.last_char : 0;
    uint8_t line_force_tag = 0;
    struct glib_window string_max_extent = {     // struct to get dimension of string (pix)
        .x_left = (int16_t)x_l_tb,                  // TODO: make global, so it works for multiple print(.)
        .x_right = (int16_t)x_l_tb,
        .y_top = (int16_t)y_asc_tb,
        .y_bottom = (int16_t)y_asc_tb
    };
    if ((!txt_xpos && !txt_ypos) || txt_drawmode == GLIB_DM_FREE) {
        // place 'cursor' inside textbox
        currx = x_l_tb;
        curry = y_asc_tb;
    } else {
        currx = txt_xpos;
        curry = txt_ypos;
    }

#if (VERBOSE > 1)
    os_printf("print_utf8: '");
    dbg_print_utf8(utf8string);
    os_printf("' @ (%d,%d) & dm = %d\n", currx, curry, txt_drawmode);
#endif

    for (chr_idx = 0; utf8string[chr_idx] != 0; ++chr_idx) {
        uint32_t currchr = utf8string[chr_idx];   // current character: utf-8-char

#if (VERBOSE > 1)
        os_printf("print: currchr: '%c' (%lu)\n", (currchr < 127) ? currchr : ' ', currchr);
#endif
        uint8_t renderable = !fnt_get_char(&chi, currchr, font);

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
        if (txt_drawmode != GLIB_DM_FREE) {
            if ((renderable && currx + chi.advance > textbox.x_right) || line_force_tag) {  // newline type 1
#if (VERBOSE > 1)
                os_printf("print: newline type 1\n");
#endif
                if (txt_drawmode == GLIB_DM_TEXT_LINE_FORCE) {
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
                    uint8_t maxcharsperline = (textbox.x_right - textbox.x_left) / fnt_current.max_char_width;
                    if (chars_in_fb.last_char > chars_in_fb.last_word
                            && chars_in_fb.last_char - chars_in_fb.last_word < maxcharsperline) {
                        // replace this ' ' with '\n' and rewind
                        ++chars_in_fb.last_char;
                        *chars_in_fb.last_char = 0; // rewind until now
                        *chars_in_fb.last_word = '\n';
                        glib_clear_fb(GLIB_OS_TEXTBOX);
                        glib_clear_tb_txt_state();
                        currx = x_l_tb;
                        curry = y_asc_tb;

                        glib_print_utf8(&(chars_in_fb.chars[1]), x_l_tb, y_asc_tb, txtpos, args, &currx, &curry);
                    } else {
                        override_last_char = 0;
                        curry += fnt_current.height;
                        currx = x_l_tb;
                        // skip next newline / space
                        if (utf8string[chr_idx + 1] == '\n' || utf8string[chr_idx + 1] == ' ') {
                            override_last_char = utf8string[chr_idx + 1];
                            ++chars_in_fb.last_char;
                            *chars_in_fb.last_char = utf8string[chr_idx + 1];
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
                for (; utf8string[chr_idx + 1] == 8; ++chr_idx, ++bsnum);

                // set end of new string -> zero at forward position
                chars_in_fb.last_char -= (bsnum - 1);
                if (chars_in_fb.last_char > chars_in_fb.chars) {
                    *chars_in_fb.last_char = 0;
                } else {    // whole string is gone
                    chars_in_fb.chars[1] = 0;
                }
                glib_clear_fb(GLIB_OS_TEXTBOX);
                glib_clear_tb_txt_state();
                currx = x_l_tb;
                curry = y_asc_tb;

                // print history without last char(s); return new positions
                // NOTE: not really recursive, as it will probably only get called once...
                //       exluding stuff like "a\bb\bc\bd" etc <- but who does that?!
                glib_print_utf8(&(chars_in_fb.chars[1]), x_l_tb, y_asc_tb, txtpos, args, &currx, &curry);

                if (line_force_tag && currx + 3 * dotinfo.advance <= textbox.x_right) {
                    // '...' fits
                    glib_print((uint8_t *)"...", x_l_tb, y_asc_tb, txtpos, args, NULL, NULL);
#if (VERBOSE > 1)
                    os_printf("print: skipping done!\n");
#endif
                    // TODO: test multiline
                    for (; utf8string[chr_idx + 1] != '\0' && utf8string[chr_idx + 1] != '\n'; ++chr_idx);
                    line_force_tag = 0;
                    //return 1;
                }
#if (VERBOSE > 1)
                os_printf("print: backspace; to go: '");
                dbg_print_utf8(&utf8string[chr_idx]);
                os_printf("'\n");
#endif
                continue;
            }
        } // if !DM_FREE

        if (currchr == '\n') {  // newline type 2
#if (VERBOSE > 1)
            os_printf("print: newline type 2\n");
#endif
            override_last_char = currchr;
            if (txt_drawmode != GLIB_DM_FREE) {
                ++chars_in_fb.last_char;
                *chars_in_fb.last_char = currchr;
            }
            curry += fnt_current.height;
            currx = x_l_tb;
            string_max_extent.y_bottom += fnt_current.height;
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

        if (curry - fnt_current.ascend + fnt_current.height > textbox.y_bottom) { // no space left, newline type 3
#if (VERBOSE > 1)
            os_printf("print: newline type 3; no space left (y = %d, need %d)\n",
                      curry, curry - fnt_current.ascend + fnt_current.height);
#endif
            override_last_char = 0;
            glib_clear_tb_txt_state();
            if (txt_drawmode == GLIB_DM_TEXT) {
                txt_xpos = currx;
                txt_ypos = curry;
                return 1;
            } else if (txt_drawmode == GLIB_DM_TEXT_CLR) {
                glib_clear_fb(GLIB_OS_TEXTBOX);
                currx = x_l_tb;
                curry = y_asc_tb;
            } else {
                return 1;
            }
        }

        // only kerning if no new line
        currx += fnt_get_kerning(override_last_char, currchr, font);
        if ((int32_t)currx + (int32_t)chi.xoffset < 0)
            currx -= chi.xoffset;
#if (VERBOSE > 1)
        os_printf("print: draw char @ (%d,%d)\n", currx, curry);
#endif

        if (glib_draw_char(&chi, &fnt_current, currx, curry, args)) {
#if (VERBOSE > 1)
            os_printf("glib_print: skipped '%c' (%lu)\n", currchr < 128 ? currchr : ' ', currchr);
#endif
            continue;
        }
        currx += chi.advance;
        override_last_char = currchr;
        if (txt_drawmode != GLIB_DM_FREE) {
            ++chars_in_fb.last_char;    // chars_in_fb.chars[0] is always 0!
            *chars_in_fb.last_char = currchr;
        }
    }   // for chr_idx in string

    if (txt_drawmode != GLIB_DM_FREE) {
        txt_xpos = currx;
        txt_ypos = curry;
    }

    if (txt_drawmode == GLIB_DM_FREE) {  // TODO: enable for recursive functionality
        int16_t trans_x = 0, trans_y = 0;
        if (txtpos & GLIB_TP_CENTER_X) {
            uint8_t string_width = string_max_extent.x_right - string_max_extent.x_left;
            uint8_t tb_width = textbox.x_right - textbox.x_left;
            trans_x = (tb_width - string_width) / 2;
        }
        if (txtpos & GLIB_TP_CENTER_Y) {
            uint8_t string_height = string_max_extent.y_bottom - string_max_extent.y_top;
            uint8_t tb_heigth = textbox.y_bottom - textbox.y_top;
            trans_y = (tb_heigth - string_height) / 2 - (string_max_extent.y_top - textbox.y_top);
        }
        glib_translate(&string_max_extent, trans_x, trans_y);
        currx += trans_x;
        curry += trans_y;
        string_max_extent.x_left += trans_x;
        string_max_extent.x_right += trans_x;
        string_max_extent.y_bottom += trans_y;
        string_max_extent.y_top += trans_y;
    }

#ifdef DBG_SHOW_TEXT_EXTENT
    glib_draw_rect(&string_max_extent, 0x09090909);
#endif
#ifdef DBG_SHOW_TEXTBOX_BORDER
    glib_draw_rect(&textbox, 0xAAAAAAAA);
#endif

#if (VERBOSE > 1)
    os_printf("print: tracked chars in fb: ");
    uint32_t *cchr = chars_in_fb.chars;
    for (; cchr <= chars_in_fb.last_char; ++cchr)
        os_printf("%c", *cchr < 128 ? *cchr : 'X');
    os_printf("; len (%lu/%d) @ chars + %lu; %lu to go\n", (cchr - chars_in_fb.chars) / sizeof(uint8_t) + 1, SSD1322_MAX_CHARS,
                (chars_in_fb.last_char - chars_in_fb.chars) / sizeof(uint8_t) + 1,
                (&(chars_in_fb.chars[SSD1322_MAX_CHARS - 1]) - cchr) / sizeof(uint8_t));
#endif

    if (x_l_re && y_asc_re) {   // return new position
        *x_l_re = currx;
        *y_asc_re = curry;
    }
#if (VERBOSE > 2)
    dbg_dump_fb(&region_full);
#endif
    return 0;
}


/***********************************************
 * ANIMATIONS
 ***********************************************/

static uint16_t anim_delay;

void ICACHE_FLASH_ATTR
glib_set_anim_delay_ms(const uint16_t delay) {
    anim_delay = (delay < GLIB_ANIM_MIN_DELAY_MS) ? GLIB_ANIM_MIN_DELAY_MS : delay;
}

/*
 * TOSS
 */
static os_timer_t volatile toss_timer;
static uint16_t toss_anim_time = 0;
static glib_anim_direction toss_dir;
static struct glib_window toss_region, toss_curr_region;
static uint16_t toss_hold_frames, toss_acceleration;
static int16_t toss_x = 0, toss_y = 0;
static void (*toss_callback)(void);

inline uint8_t
all_outside_boundaries(const struct glib_window *const region) {
    return ((region->x_left < 0 || region->x_left > GLIB_DISP_COL_UPPER)
            && (region->x_right < 0 || region->x_right > GLIB_DISP_COL_UPPER))
        || ((region->y_bottom < 0 || region->y_bottom > GLIB_DISP_ROW_UPPER)
            && (region->y_top < 0 || region->y_top > GLIB_DISP_ROW_UPPER));
}

static void ICACHE_FLASH_ATTR
toss_func(void *arg) {
    toss_curr_region.x_left += toss_x;
    toss_curr_region.x_right += toss_x;
    toss_curr_region.y_top += toss_y;
    toss_curr_region.y_bottom += toss_y;

    int16_t move_by = 0;
    if (toss_anim_time > toss_hold_frames)
        move_by = (int16_t)(((int32_t)(toss_anim_time - toss_hold_frames)
                             * (int32_t)(toss_anim_time - toss_hold_frames))
                            / (int32_t)toss_acceleration);
#if (VERBOSE > 1)
    os_printf("toss: move by %d towards %d\n", move_by, toss_dir);
#endif
    switch(toss_dir) {
    case GLIB_AD_N:
        toss_y = -move_by; break;
    case GLIB_AD_NW:
        toss_y = -move_by;
        toss_x = -move_by; break;
    case GLIB_AD_W:
        toss_x = -move_by; break;
    case GLIB_AD_SW:
        toss_y = move_by;
        toss_x = -move_by; break;
    case GLIB_AD_S:
        toss_y = move_by; break;
    case GLIB_AD_SE:
        toss_y = move_by;
        toss_x = move_by; break;
    case GLIB_AD_E:
        toss_x = move_by; break;
    case GLIB_AD_NE:
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
              toss_region.x_left, toss_region.x_right, toss_region.y_top, toss_region.y_bottom, toss_anim_time);
#endif
    glib_translate(&toss_region, toss_x, toss_y);
    if (all_outside_boundaries(&toss_curr_region)) { // everything is outside
        os_timer_disarm(&toss_timer);
        toss_anim_time = 0;
        struct glib_window tb_tmp = textbox;
        textbox = toss_region;
        glib_clear_fb(GLIB_OS_TEXTBOX);
        textbox = tb_tmp;
        toss_callback();
    } else if (toss_x || toss_y) {
        glib_update_gram((uint32_t *)framebuffer);
    }

    system_soft_wdt_feed();
    ++toss_anim_time;
}

void ICACHE_FLASH_ATTR
glib_anim_toss_away(const glib_anim_direction dir, const struct glib_window *const region,
                    const uint16_t hold_frames, const uint16_t acceleration, void (*toss_cb)(void)) {
    os_timer_disarm(&toss_timer);
    os_timer_setfn(&toss_timer, (os_timer_func_t *)toss_func, NULL);
    os_timer_arm(&toss_timer, anim_delay, 1);
    toss_region = *region;
    toss_curr_region = *region;
    toss_callback = toss_cb;
    toss_dir = dir;
    toss_hold_frames = hold_frames;
    toss_acceleration = acceleration;
    toss_x = 0;
    toss_y = 0;
}

void ICACHE_FLASH_ATTR
glib_clear_fb_toss_anim(const glib_anim_direction dir, const uint16_t hold_frames, const uint16_t acceleration,
                        void (*toss_cb)(void)) {
    glib_anim_toss_away(dir, &region_full, hold_frames, acceleration, toss_cb);
}


/*
 * FADEOUT
 */
static os_timer_t volatile fadeout_timer;
static uint8_t fadeout_curr_brightness = GLIB_MAX_BRIGHTNESS;
static uint32_t fadeout_bg;
static uint16_t fadeout_hold_frames, fadeout_acceleration;
static void (*fadeout_callback)(void);

static void ICACHE_FLASH_ATTR
fadeout_func(void *arg) {
    int16_t reduce_by = 0;
    if (!fadeout_hold_frames) {
        reduce_by = 1 + (
                    (GLIB_MAX_BRIGHTNESS - fadeout_curr_brightness)
                    * (GLIB_MAX_BRIGHTNESS - fadeout_curr_brightness)
                    / (fadeout_acceleration * 5));
    } else {
        fadeout_hold_frames--;
        return;
    }
#if (VERBOSE > 1)
    os_printf("fadeout_anim: brightness: %d, reduce by: %d\n", fadeout_curr_brightness, reduce_by);
#endif
    if ((int16_t)fadeout_curr_brightness - reduce_by < 0)
        fadeout_curr_brightness = 0;
    else
        fadeout_curr_brightness -= (uint8_t)reduce_by;

    if (fadeout_curr_brightness == 0) {
        os_timer_disarm(&fadeout_timer);
        glib_clear_disp(fadeout_bg);
        fadeout_curr_brightness = GLIB_MAX_BRIGHTNESS;
        fadeout_callback();
    }
    glib_set_brightness(fadeout_curr_brightness);
}

void ICACHE_FLASH_ATTR
glib_clear_disp_fadeout_anim(const uint32_t bg_col, const uint16_t hold_frames, const uint16_t acceleration,
                             void (*fadeout_cb)(void)) {
    os_timer_disarm(&fadeout_timer);
    os_timer_setfn(&fadeout_timer, (os_timer_func_t *)fadeout_func, NULL);
    os_timer_arm(&fadeout_timer, anim_delay, 1);

    fadeout_bg = bg_col;
    fadeout_hold_frames = hold_frames;
    fadeout_acceleration = acceleration;
    fadeout_callback = fadeout_cb;
}


void ICACHE_FLASH_ATTR
glib_clear_fb(const glib_object_specifier obj) {
    uint16_t i;
    switch (obj) {
    case GLIB_OS_ALL:
#if (VERBOSE > 1)
        os_printf("glib_clear_fb: ALL...");
#endif
        glib_tag_upd_reg_log(&region_full);
        for (i = 0; i < GLIB_FBSIZE_INT32; ++i)
            framebuffer[i] = background_pattern;

        break;


    case GLIB_OS_STAGED:                         // TODO: will still clear everything... only not staged!
/*
#if (VERBOSE > 1)
        os_printf("glib_clear_fb: STAGED...");
#endif
        fb_upd_reg = fb_upd_reg_old;                // TODO: TEST!
        uint16_t k;
        for (i = fb_upd_reg.seg_left; i < fb_upd_reg.seg_right; ++i)
            for (k = fb_upd_reg.row_bottom; k < fb_upd_reg.row_top; ++k)
                framebuffer[k * SSD1322_SEGMENTS + i] = background_pattern;
*/
        break;

    case GLIB_OS_TEXTBOX:
#if (VERBOSE > 1)
        os_printf("glib_clear_fb: TEXTBOX...\n");
#endif
        glib_draw((uint32_t *)framebuffer, textbox.x_left, textbox.y_top, &background_pattern, textbox.y_bottom - textbox.y_top + 1,
                     textbox.x_right - textbox.x_left + 1, GLIB_DA_CLR);
        break;
    } // switch (obj)

#if (VERBOSE > 1)
    os_printf("glib_clear_fb: done!\n");
#endif
}


void ICACHE_FLASH_ATTR
glib_set_background(const uint32_t pattern) {
    background_pattern = pattern;
}

void ICACHE_FLASH_ATTR
glib_set_font(const fnt_id new_font) {
    font = new_font;
    fnt_get_info(&fnt_current, font);
}

const struct glib_window * ICACHE_FLASH_ATTR
glib_get_full_region(void) {
    return &region_full;
}

void
glib_fb2gram(void) {
    glib_update_gram((uint32_t *)framebuffer);
}


void ICACHE_FLASH_ATTR
glib_reset(void) {
    glib_reset_display();
    glib_clear_disp(background_pattern);
    glib_set_textbox(NULL);
    glib_clear_tb_txt_state();
    glib_set_mode(GLIB_DM_TEXT);

#if (VERBOSE > 1)
    os_printf("glib_reset: done\n");
#endif
}


void ICACHE_FLASH_ATTR
glib_init(void) {
    glib_reset();
    glib_set_anim_delay_ms(GLIB_ANIM_MIN_DELAY_MS);
    glib_set_font(0);
    glib_set_textbox(NULL);
    glib_clear_tb_txt_state();
    glib_set_mode(GLIB_DM_TEXT);
    glib_init_display();
    glib_set_enable(1);
}
