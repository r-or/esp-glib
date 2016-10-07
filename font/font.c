#include "fonth"

#include "user_interface.h"

void ICACHE_FLASH_ATTR get_font_info(struct font_info *const finfo) {
    *finfo = (struct font_info) {
        .font_size = 8652,
        .pix_depth = 4,
        .font_name = "Hanken_Light_13",
        .font_height = 19,
        .font_ascend = 13,
    };
}

uint8_t ICACHE_FLASH_ATTR get_char(struct char_info *const ch_info, const uint8_t ch) {
    switch(ch) {
    case 32:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 0,
            .advance = 6,
            .width = 0,
            .height = 0,
            .address = 0x00000000,
            .length = 0,
        }; break;
    case 33:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 5,
            .width = 3,
            .height = 13,
            .address = 0x00000000,
            .length = 20,
        }; break;
    case 34:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 5,
            .width = 3,
            .height = 3,
            .address = 0x00000014,
            .length = 8,
        }; break;
    case 35:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 14,
            .advance = 14,
            .width = 15,
            .height = 14,
            .address = 0x0000001c,
            .length = 108,
        }; break;
    case 36:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 14,
            .advance = 9,
            .width = 7,
            .height = 16,
            .address = 0x00000088,
            .length = 56,
        }; break;
    case 37:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 11,
            .width = 9,
            .height = 13,
            .address = 0x000000c0,
            .length = 60,
        }; break;
    case 38:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 12,
            .width = 11,
            .height = 13,
            .address = 0x000000fc,
            .length = 72,
        }; break;
    case 39:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 3,
            .width = 1,
            .height = 3,
            .address = 0x00000144,
            .length = 4,
        }; break;
    case 40:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 5,
            .width = 4,
            .height = 17,
            .address = 0x00000148,
            .length = 36,
        }; break;
    case 41:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 5,
            .width = 4,
            .height = 17,
            .address = 0x0000016c,
            .length = 36,
        }; break;
    case 42:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 5,
            .width = 5,
            .height = 4,
            .address = 0x00000190,
            .length = 12,
        }; break;
    case 43:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 8,
            .advance = 9,
            .width = 9,
            .height = 8,
            .address = 0x0000019c,
            .length = 36,
        }; break;
    case 44:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 2,
            .advance = 3,
            .width = 2,
            .height = 4,
            .address = 0x000001c0,
            .length = 4,
        }; break;
    case 45:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 5,
            .advance = 8,
            .width = 8,
            .height = 1,
            .address = 0x000001c4,
            .length = 4,
        }; break;
    case 46:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 1,
            .advance = 3,
            .width = 1,
            .height = 1,
            .address = 0x000001c8,
            .length = 4,
        }; break;
    case 47:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 8,
            .width = 8,
            .height = 17,
            .address = 0x000001cc,
            .length = 68,
        }; break;
    case 48:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 9,
            .width = 7,
            .height = 13,
            .address = 0x00000210,
            .length = 48,
        }; break;
    case 49:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 7,
            .width = 7,
            .height = 13,
            .address = 0x00000240,
            .length = 48,
        }; break;
    case 50:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 9,
            .width = 8,
            .height = 13,
            .address = 0x00000270,
            .length = 52,
        }; break;
    case 51:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 9,
            .width = 7,
            .height = 13,
            .address = 0x000002a4,
            .length = 48,
        }; break;
    case 52:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 10,
            .width = 9,
            .height = 13,
            .address = 0x000002d4,
            .length = 60,
        }; break;
    case 53:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 9,
            .width = 8,
            .height = 13,
            .address = 0x00000310,
            .length = 52,
        }; break;
    case 54:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 9,
            .width = 7,
            .height = 13,
            .address = 0x00000344,
            .length = 48,
        }; break;
    case 55:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 9,
            .width = 9,
            .height = 13,
            .address = 0x00000374,
            .length = 60,
        }; break;
    case 56:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 9,
            .width = 7,
            .height = 13,
            .address = 0x000003b0,
            .length = 48,
        }; break;
    case 57:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 9,
            .width = 7,
            .height = 13,
            .address = 0x000003e0,
            .length = 48,
        }; break;
    case 58:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 3,
            .width = 1,
            .height = 8,
            .address = 0x00000410,
            .length = 4,
        }; break;
    case 59:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 8,
            .advance = 3,
            .width = 2,
            .height = 10,
            .address = 0x00000414,
            .length = 12,
        }; break;
    case 60:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 9,
            .advance = 9,
            .width = 7,
            .height = 9,
            .address = 0x00000420,
            .length = 32,
        }; break;
    case 61:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 7,
            .advance = 11,
            .width = 11,
            .height = 5,
            .address = 0x00000440,
            .length = 28,
        }; break;
    case 62:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 9,
            .advance = 9,
            .width = 8,
            .height = 9,
            .address = 0x0000045c,
            .length = 36,
        }; break;
    case 63:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 10,
            .width = 8,
            .height = 13,
            .address = 0x00000480,
            .length = 52,
        }; break;
    case 64:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 10,
            .width = 9,
            .height = 8,
            .address = 0x000004b4,
            .length = 36,
        }; break;
    case 65:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 12,
            .width = 12,
            .height = 13,
            .address = 0x000004d8,
            .length = 80,
        }; break;
    case 66:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 11,
            .width = 8,
            .height = 13,
            .address = 0x00000528,
            .length = 52,
        }; break;
    case 67:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 14,
            .width = 12,
            .height = 13,
            .address = 0x0000055c,
            .length = 80,
        }; break;
    case 68:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 13,
            .width = 11,
            .height = 13,
            .address = 0x000005ac,
            .length = 72,
        }; break;
    case 69:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 10,
            .width = 7,
            .height = 13,
            .address = 0x000005f4,
            .length = 48,
        }; break;
    case 70:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 9,
            .width = 7,
            .height = 13,
            .address = 0x00000624,
            .length = 48,
        }; break;
    case 71:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 14,
            .width = 12,
            .height = 13,
            .address = 0x00000654,
            .length = 80,
        }; break;
    case 72:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 13,
            .width = 9,
            .height = 13,
            .address = 0x000006a4,
            .length = 60,
        }; break;
    case 73:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 5,
            .width = 1,
            .height = 13,
            .address = 0x000006e0,
            .length = 8,
        }; break;
    case 74:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 7,
            .width = 4,
            .height = 13,
            .address = 0x000006e8,
            .length = 28,
        }; break;
    case 75:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 11,
            .width = 8,
            .height = 13,
            .address = 0x00000704,
            .length = 52,
        }; break;
    case 76:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 8,
            .width = 6,
            .height = 13,
            .address = 0x00000738,
            .length = 40,
        }; break;
    case 77:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 17,
            .width = 13,
            .height = 13,
            .address = 0x00000760,
            .length = 88,
        }; break;
    case 78:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 13,
            .width = 9,
            .height = 13,
            .address = 0x000007b8,
            .length = 60,
        }; break;
    case 79:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 15,
            .width = 13,
            .height = 13,
            .address = 0x000007f4,
            .length = 88,
        }; break;
    case 80:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 10,
            .width = 7,
            .height = 13,
            .address = 0x0000084c,
            .length = 48,
        }; break;
    case 81:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 15,
            .width = 14,
            .height = 13,
            .address = 0x0000087c,
            .length = 92,
        }; break;
    case 82:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 10,
            .width = 7,
            .height = 13,
            .address = 0x000008d8,
            .length = 48,
        }; break;
    case 83:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 9,
            .width = 7,
            .height = 13,
            .address = 0x00000908,
            .length = 48,
        }; break;
    case 84:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 9,
            .width = 9,
            .height = 13,
            .address = 0x00000938,
            .length = 60,
        }; break;
    case 85:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 14,
            .width = 10,
            .height = 13,
            .address = 0x00000974,
            .length = 68,
        }; break;
    case 86:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 11,
            .width = 12,
            .height = 13,
            .address = 0x000009b8,
            .length = 80,
        }; break;
    case 87:
        *ch_info = (struct char_info) {
            .xoffset = -1,
            .yoffset = 13,
            .advance = 18,
            .width = 20,
            .height = 13,
            .address = 0x00000a08,
            .length = 132,
        }; break;
    case 88:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 12,
            .width = 12,
            .height = 13,
            .address = 0x00000a8c,
            .length = 80,
        }; break;
    case 89:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 9,
            .width = 9,
            .height = 13,
            .address = 0x00000adc,
            .length = 60,
        }; break;
    case 90:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 9,
            .width = 9,
            .height = 13,
            .address = 0x00000b18,
            .length = 60,
        }; break;
    case 91:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 6,
            .width = 5,
            .height = 17,
            .address = 0x00000b54,
            .length = 44,
        }; break;
    case 92:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 8,
            .width = 8,
            .height = 17,
            .address = 0x00000b80,
            .length = 68,
        }; break;
    case 93:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 6,
            .width = 5,
            .height = 17,
            .address = 0x00000bc4,
            .length = 44,
        }; break;
    case 94:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 9,
            .width = 9,
            .height = 7,
            .address = 0x00000bf0,
            .length = 32,
        }; break;
    case 95:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 1,
            .advance = 8,
            .width = 8,
            .height = 1,
            .address = 0x00000c10,
            .length = 4,
        }; break;
    case 96:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 4,
            .width = 4,
            .height = 3,
            .address = 0x00000c14,
            .length = 8,
        }; break;
    case 97:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 11,
            .width = 8,
            .height = 8,
            .address = 0x00000c1c,
            .length = 32,
        }; break;
    case 98:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 11,
            .width = 8,
            .height = 13,
            .address = 0x00000c3c,
            .length = 52,
        }; break;
    case 99:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 9,
            .width = 8,
            .height = 8,
            .address = 0x00000c70,
            .length = 32,
        }; break;
    case 100:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 11,
            .width = 8,
            .height = 13,
            .address = 0x00000c90,
            .length = 52,
        }; break;
    case 101:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 10,
            .width = 8,
            .height = 8,
            .address = 0x00000cc4,
            .length = 32,
        }; break;
    case 102:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 7,
            .width = 6,
            .height = 13,
            .address = 0x00000ce4,
            .length = 40,
        }; break;
    case 103:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 11,
            .width = 8,
            .height = 12,
            .address = 0x00000d0c,
            .length = 48,
        }; break;
    case 104:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 10,
            .width = 7,
            .height = 13,
            .address = 0x00000d3c,
            .length = 48,
        }; break;
    case 105:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 11,
            .advance = 5,
            .width = 1,
            .height = 11,
            .address = 0x00000d6c,
            .length = 8,
        }; break;
    case 106:
        *ch_info = (struct char_info) {
            .xoffset = -2,
            .yoffset = 10,
            .advance = 4,
            .width = 4,
            .height = 14,
            .address = 0x00000d74,
            .length = 28,
        }; break;
    case 107:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 8,
            .width = 6,
            .height = 13,
            .address = 0x00000d90,
            .length = 40,
        }; break;
    case 108:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 5,
            .width = 1,
            .height = 13,
            .address = 0x00000db8,
            .length = 8,
        }; break;
    case 109:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 8,
            .advance = 14,
            .width = 11,
            .height = 8,
            .address = 0x00000dc0,
            .length = 44,
        }; break;
    case 110:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 8,
            .advance = 9,
            .width = 6,
            .height = 8,
            .address = 0x00000dec,
            .length = 24,
        }; break;
    case 111:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 10,
            .width = 8,
            .height = 8,
            .address = 0x00000e04,
            .length = 32,
        }; break;
    case 112:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 8,
            .advance = 11,
            .width = 8,
            .height = 12,
            .address = 0x00000e24,
            .length = 48,
        }; break;
    case 113:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 11,
            .width = 8,
            .height = 12,
            .address = 0x00000e54,
            .length = 48,
        }; break;
    case 114:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 8,
            .advance = 7,
            .width = 5,
            .height = 8,
            .address = 0x00000e84,
            .length = 20,
        }; break;
    case 115:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 7,
            .width = 5,
            .height = 8,
            .address = 0x00000e98,
            .length = 20,
        }; break;
    case 116:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 11,
            .advance = 7,
            .width = 7,
            .height = 11,
            .address = 0x00000eac,
            .length = 40,
        }; break;
    case 117:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 8,
            .width = 6,
            .height = 8,
            .address = 0x00000ed4,
            .length = 24,
        }; break;
    case 118:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 8,
            .advance = 8,
            .width = 8,
            .height = 8,
            .address = 0x00000eec,
            .length = 32,
        }; break;
    case 119:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 8,
            .advance = 14,
            .width = 14,
            .height = 8,
            .address = 0x00000f0c,
            .length = 56,
        }; break;
    case 120:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 8,
            .advance = 8,
            .width = 8,
            .height = 8,
            .address = 0x00000f44,
            .length = 32,
        }; break;
    case 121:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 8,
            .advance = 8,
            .width = 8,
            .height = 12,
            .address = 0x00000f64,
            .length = 48,
        }; break;
    case 122:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 8,
            .advance = 8,
            .width = 8,
            .height = 8,
            .address = 0x00000f94,
            .length = 32,
        }; break;
    case 123:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 7,
            .width = 5,
            .height = 17,
            .address = 0x00000fb4,
            .length = 44,
        }; break;
    case 124:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 5,
            .width = 1,
            .height = 17,
            .address = 0x00000fe0,
            .length = 12,
        }; break;
    case 125:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 6,
            .width = 5,
            .height = 17,
            .address = 0x00000fec,
            .length = 44,
        }; break;
    case 126:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 12,
            .advance = 9,
            .width = 6,
            .height = 2,
            .address = 0x00001018,
            .length = 8,
        }; break;
    case 161:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 5,
            .width = 3,
            .height = 13,
            .address = 0x00001020,
            .length = 20,
        }; break;
    case 162:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 10,
            .advance = 9,
            .width = 8,
            .height = 11,
            .address = 0x00001034,
            .length = 44,
        }; break;
    case 163:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 11,
            .width = 9,
            .height = 13,
            .address = 0x00001060,
            .length = 60,
        }; break;
    case 165:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 9,
            .width = 10,
            .height = 13,
            .address = 0x0000109c,
            .length = 68,
        }; break;
    case 166:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 5,
            .width = 1,
            .height = 17,
            .address = 0x000010e0,
            .length = 12,
        }; break;
    case 168:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 11,
            .advance = 6,
            .width = 4,
            .height = 1,
            .address = 0x000010ec,
            .length = 4,
        }; break;
    case 169:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 7,
            .address = 0x000010f0,
            .length = 28,
        }; break;
    case 171:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 9,
            .advance = 12,
            .width = 12,
            .height = 9,
            .address = 0x0000110c,
            .length = 56,
        }; break;
    case 172:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 8,
            .advance = 13,
            .width = 11,
            .height = 5,
            .address = 0x00001144,
            .length = 28,
        }; break;
    case 174:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 7,
            .address = 0x00001160,
            .length = 28,
        }; break;
    case 176:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 6,
            .width = 4,
            .height = 3,
            .address = 0x0000117c,
            .length = 8,
        }; break;
    case 177:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 10,
            .advance = 11,
            .width = 10,
            .height = 10,
            .address = 0x00001184,
            .length = 52,
        }; break;
    case 180:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 4,
            .width = 4,
            .height = 3,
            .address = 0x000011b8,
            .length = 8,
        }; break;
    case 181:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 8,
            .advance = 10,
            .width = 6,
            .height = 12,
            .address = 0x000011c0,
            .length = 36,
        }; break;
    case 182:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 9,
            .width = 7,
            .height = 13,
            .address = 0x000011e4,
            .length = 48,
        }; break;
    case 183:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 3,
            .width = 3,
            .height = 3,
            .address = 0x00001214,
            .length = 8,
        }; break;
    case 184:
        *ch_info = (struct char_info) {
            .xoffset = 7,
            .yoffset = 1,
            .advance = 19,
            .width = 3,
            .height = 4,
            .address = 0x0000121c,
            .length = 8,
        }; break;
    case 187:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 9,
            .advance = 12,
            .width = 12,
            .height = 9,
            .address = 0x00001224,
            .length = 56,
        }; break;
    case 191:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 10,
            .width = 8,
            .height = 13,
            .address = 0x0000125c,
            .length = 52,
        }; break;
    case 192:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 16,
            .advance = 12,
            .width = 13,
            .height = 16,
            .address = 0x00001290,
            .length = 104,
        }; break;
    case 193:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 16,
            .advance = 12,
            .width = 13,
            .height = 16,
            .address = 0x000012f8,
            .length = 104,
        }; break;
    case 194:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 16,
            .advance = 12,
            .width = 13,
            .height = 16,
            .address = 0x00001360,
            .length = 104,
        }; break;
    case 195:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 15,
            .advance = 12,
            .width = 13,
            .height = 15,
            .address = 0x000013c8,
            .length = 100,
        }; break;
    case 196:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 15,
            .advance = 12,
            .width = 12,
            .height = 15,
            .address = 0x0000142c,
            .length = 92,
        }; break;
    case 197:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 15,
            .advance = 13,
            .width = 12,
            .height = 15,
            .address = 0x00001488,
            .length = 92,
        }; break;
    case 198:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 19,
            .width = 18,
            .height = 13,
            .address = 0x000014e4,
            .length = 120,
        }; break;
    case 199:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 14,
            .width = 12,
            .height = 16,
            .address = 0x0000155c,
            .length = 96,
        }; break;
    case 200:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 16,
            .advance = 9,
            .width = 7,
            .height = 16,
            .address = 0x000015bc,
            .length = 56,
        }; break;
    case 201:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 16,
            .advance = 9,
            .width = 7,
            .height = 16,
            .address = 0x000015f4,
            .length = 56,
        }; break;
    case 202:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 16,
            .advance = 9,
            .width = 7,
            .height = 16,
            .address = 0x0000162c,
            .length = 56,
        }; break;
    case 203:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 15,
            .advance = 10,
            .width = 7,
            .height = 15,
            .address = 0x00001664,
            .length = 56,
        }; break;
    case 204:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 16,
            .advance = 4,
            .width = 3,
            .height = 16,
            .address = 0x0000169c,
            .length = 24,
        }; break;
    case 205:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 16,
            .advance = 4,
            .width = 4,
            .height = 16,
            .address = 0x000016b4,
            .length = 32,
        }; break;
    case 206:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 16,
            .advance = 4,
            .width = 5,
            .height = 16,
            .address = 0x000016d4,
            .length = 40,
        }; break;
    case 207:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 15,
            .advance = 4,
            .width = 4,
            .height = 15,
            .address = 0x000016fc,
            .length = 32,
        }; break;
    case 208:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 13,
            .advance = 13,
            .width = 12,
            .height = 13,
            .address = 0x0000171c,
            .length = 80,
        }; break;
    case 209:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 15,
            .advance = 12,
            .width = 9,
            .height = 15,
            .address = 0x0000176c,
            .length = 68,
        }; break;
    case 210:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 16,
            .advance = 15,
            .width = 13,
            .height = 16,
            .address = 0x000017b0,
            .length = 104,
        }; break;
    case 211:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 16,
            .advance = 15,
            .width = 13,
            .height = 16,
            .address = 0x00001818,
            .length = 104,
        }; break;
    case 212:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 16,
            .advance = 15,
            .width = 13,
            .height = 16,
            .address = 0x00001880,
            .length = 104,
        }; break;
    case 213:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 15,
            .advance = 15,
            .width = 13,
            .height = 15,
            .address = 0x000018e8,
            .length = 100,
        }; break;
    case 214:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 15,
            .advance = 16,
            .width = 13,
            .height = 15,
            .address = 0x0000194c,
            .length = 100,
        }; break;
    case 215:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 8,
            .advance = 9,
            .width = 9,
            .height = 8,
            .address = 0x000019b0,
            .length = 36,
        }; break;
    case 216:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 15,
            .advance = 15,
            .width = 13,
            .height = 17,
            .address = 0x000019d4,
            .length = 112,
        }; break;
    case 217:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 16,
            .advance = 13,
            .width = 10,
            .height = 16,
            .address = 0x00001a44,
            .length = 80,
        }; break;
    case 218:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 16,
            .advance = 13,
            .width = 10,
            .height = 16,
            .address = 0x00001a94,
            .length = 80,
        }; break;
    case 219:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 16,
            .advance = 13,
            .width = 10,
            .height = 16,
            .address = 0x00001ae4,
            .length = 80,
        }; break;
    case 220:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 15,
            .advance = 13,
            .width = 10,
            .height = 15,
            .address = 0x00001b34,
            .length = 76,
        }; break;
    case 221:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 16,
            .advance = 9,
            .width = 9,
            .height = 16,
            .address = 0x00001b80,
            .length = 72,
        }; break;
    case 222:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 10,
            .width = 7,
            .height = 13,
            .address = 0x00001bc8,
            .length = 48,
        }; break;
    case 223:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 10,
            .width = 7,
            .height = 13,
            .address = 0x00001bf8,
            .length = 48,
        }; break;
    case 224:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00001c28,
            .length = 48,
        }; break;
    case 225:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00001c58,
            .length = 48,
        }; break;
    case 226:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00001c88,
            .length = 48,
        }; break;
    case 227:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 11,
            .width = 8,
            .height = 12,
            .address = 0x00001cb8,
            .length = 48,
        }; break;
    case 228:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 11,
            .advance = 10,
            .width = 8,
            .height = 11,
            .address = 0x00001ce8,
            .length = 44,
        }; break;
    case 229:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00001d14,
            .length = 48,
        }; break;
    case 230:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 8,
            .advance = 16,
            .width = 16,
            .height = 8,
            .address = 0x00001d44,
            .length = 64,
        }; break;
    case 231:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 9,
            .width = 8,
            .height = 11,
            .address = 0x00001d84,
            .length = 44,
        }; break;
    case 232:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00001db0,
            .length = 48,
        }; break;
    case 233:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00001de0,
            .length = 48,
        }; break;
    case 234:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00001e10,
            .length = 48,
        }; break;
    case 235:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 11,
            .advance = 10,
            .width = 8,
            .height = 11,
            .address = 0x00001e40,
            .length = 44,
        }; break;
    case 236:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 12,
            .advance = 5,
            .width = 4,
            .height = 12,
            .address = 0x00001e6c,
            .length = 24,
        }; break;
    case 237:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 4,
            .width = 4,
            .height = 12,
            .address = 0x00001e84,
            .length = 24,
        }; break;
    case 238:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 12,
            .advance = 4,
            .width = 5,
            .height = 12,
            .address = 0x00001e9c,
            .length = 32,
        }; break;
    case 239:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 11,
            .advance = 5,
            .width = 4,
            .height = 11,
            .address = 0x00001ebc,
            .length = 24,
        }; break;
    case 240:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 11,
            .width = 10,
            .height = 13,
            .address = 0x00001ed4,
            .length = 68,
        }; break;
    case 241:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 12,
            .advance = 9,
            .width = 6,
            .height = 12,
            .address = 0x00001f18,
            .length = 36,
        }; break;
    case 242:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00001f3c,
            .length = 48,
        }; break;
    case 243:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00001f6c,
            .length = 48,
        }; break;
    case 244:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00001f9c,
            .length = 48,
        }; break;
    case 245:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00001fcc,
            .length = 48,
        }; break;
    case 246:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 11,
            .advance = 10,
            .width = 8,
            .height = 11,
            .address = 0x00001ffc,
            .length = 44,
        }; break;
    case 247:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 8,
            .advance = 11,
            .width = 11,
            .height = 8,
            .address = 0x00002028,
            .length = 44,
        }; break;
    case 248:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 10,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x00002054,
            .length = 48,
        }; break;
    case 249:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 12,
            .advance = 9,
            .width = 6,
            .height = 12,
            .address = 0x00002084,
            .length = 36,
        }; break;
    case 250:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 12,
            .advance = 9,
            .width = 6,
            .height = 12,
            .address = 0x000020a8,
            .length = 36,
        }; break;
    case 251:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 12,
            .advance = 9,
            .width = 6,
            .height = 12,
            .address = 0x000020cc,
            .length = 36,
        }; break;
    case 252:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 11,
            .advance = 9,
            .width = 6,
            .height = 11,
            .address = 0x000020f0,
            .length = 36,
        }; break;
    case 253:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 11,
            .advance = 8,
            .width = 8,
            .height = 15,
            .address = 0x00002114,
            .length = 60,
        }; break;
    case 254:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 13,
            .advance = 11,
            .width = 8,
            .height = 17,
            .address = 0x00002150,
            .length = 68,
        }; break;
    case 255:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 10,
            .advance = 8,
            .width = 8,
            .height = 14,
            .address = 0x00002194,
            .length = 56,
        }; break;
    default: return 1;
    }
    return 0;
}

int8_t ICACHE_FLASH_ATTR get_kerning(const uint8_t ch1, const uint8_t ch2) {
    switch(ch1) {
    case 65:
        switch(ch2) {
        case 84:
        case 86:
        case 89:
            return -1;
        default:
            return 0;
        }
    case 70:
        switch(ch2) {
        case 65:
        case 192:
        case 193:
        case 194:
        case 195:
        case 196:
        case 197:
        case 198:
            return -1;
        default:
            return 0;
        }
    case 79:
        switch(ch2) {
        case 89:
        case 90:
            return -1;
        default:
            return 0;
        }
    case 80:
        switch(ch2) {
        case 65:
        case 97:
        case 99:
        case 100:
        case 101:
        case 103:
        case 105:
        case 106:
        case 111:
        case 113:
        case 115:
        case 192:
        case 193:
        case 194:
        case 195:
        case 196:
        case 197:
            return -1;
        default:
            return 0;
        }
    case 84:
        switch(ch2) {
        case 97:
        case 99:
        case 101:
        case 103:
        case 106:
        case 111:
        case 113:
        case 114:
        case 115:
        case 117:
        case 119:
        case 198:
            return -1;
        default:
            return 0;
        }
    case 86:
        switch(ch2) {
        case 65:
        case 192:
        case 193:
        case 194:
        case 195:
        case 196:
        case 197:
        case 198:
            return -1;
        default:
            return 0;
        }
    case 87:
        switch(ch2) {
        case 81:
        case 97:
        case 101:
        case 103:
        case 111:
        case 113:
        case 198:
            return -1;
        default:
            return 0;
        }
    case 89:
        switch(ch2) {
        case 65:
        case 79:
        case 192:
        case 193:
        case 194:
        case 195:
        case 196:
        case 197:
        case 198:
        case 210:
        case 211:
        case 212:
        case 213:
        case 214:
        case 216:
            return -1;
        default:
            return 0;
        }
    case 90:
        switch(ch2) {
        case 210:
        case 211:
        case 212:
        case 213:
        case 214:
        case 216:
            return -1;
        default:
            return 0;
        }
    case 97:
        switch(ch2) {
        case 84:
        case 87:
            return -1;
        default:
            return 0;
        }
    case 99:
        switch(ch2) {
        case 84:
            return -1;
        default:
            return 0;
        }
    case 101:
        switch(ch2) {
        case 84:
        case 87:
            return -1;
        default:
            return 0;
        }
    case 103:
        switch(ch2) {
        case 84:
            return -1;
        default:
            return 0;
        }
    case 111:
        switch(ch2) {
        case 84:
        case 87:
            return -1;
        default:
            return 0;
        }
    case 113:
        switch(ch2) {
        case 84:
            return -1;
        default:
            return 0;
        }
    case 114:
        switch(ch2) {
        case 84:
            return -1;
        default:
            return 0;
        }
    case 115:
        switch(ch2) {
        case 84:
            return -1;
        default:
            return 0;
        }
    case 117:
        switch(ch2) {
        case 84:
            return -1;
        default:
            return 0;
        }
    case 119:
        switch(ch2) {
        case 84:
            return -1;
        default:
            return 0;
        }
    case 192:
        switch(ch2) {
        case 86:
        case 89:
            return -1;
        default:
            return 0;
        }
    case 193:
        switch(ch2) {
        case 86:
            return -1;
        default:
            return 0;
        }
    case 194:
        switch(ch2) {
        case 86:
        case 89:
            return -1;
        default:
            return 0;
        }
    case 195:
        switch(ch2) {
        case 86:
        case 89:
            return -1;
        default:
            return 0;
        }
    case 196:
        switch(ch2) {
        case 86:
        case 89:
            return -1;
        default:
            return 0;
        }
    case 197:
        switch(ch2) {
        case 86:
        case 89:
            return -1;
        default:
            return 0;
        }
    case 210:
        switch(ch2) {
        case 89:
        case 90:
            return -1;
        default:
            return 0;
        }
    case 211:
        switch(ch2) {
        case 89:
        case 90:
            return -1;
        default:
            return 0;
        }
    case 212:
        switch(ch2) {
        case 89:
        case 90:
            return -1;
        default:
            return 0;
        }
    case 213:
        switch(ch2) {
        case 89:
        case 90:
            return -1;
        default:
            return 0;
        }
    case 214:
        switch(ch2) {
        case 89:
        case 90:
            return -1;
        default:
            return 0;
        }
    case 216:
        switch(ch2) {
        case 89:
        case 90:
            return -1;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

