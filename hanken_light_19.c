#include "hanken_light_19.h"

#include "user_interface.h"

void ICACHE_FLASH_ATTR get_font_info(struct font_info *const finfo) {
    *finfo = (struct font_info) {
        .font_size = 19908,
        .pix_depth = 4,
        .font_name = "Hanken_Light_19",
        .font_height = 28,
        .font_ascend = 19,
    };
}

uint8_t ICACHE_FLASH_ATTR get_char(struct char_info *const ch_info, const uint8_t ch) {
    switch(ch) {
    case 32:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 0,
            .advance = 9,
            .width = 0,
            .height = 0,
            .address = 0x00000000,
            .length = 0,
        }; break;
    case 33:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 7,
            .width = 3,
            .height = 19,
            .address = 0x00000000,
            .length = 32,
        }; break;
    case 34:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 6,
            .width = 6,
            .height = 5,
            .address = 0x00000020,
            .length = 16,
        }; break;
    case 35:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 20,
            .advance = 22,
            .width = 21,
            .height = 20,
            .address = 0x00000030,
            .length = 212,
        }; break;
    case 36:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 22,
            .advance = 13,
            .width = 11,
            .height = 25,
            .address = 0x00000104,
            .length = 140,
        }; break;
    case 37:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 15,
            .width = 15,
            .height = 19,
            .address = 0x00000190,
            .length = 144,
        }; break;
    case 38:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 18,
            .width = 17,
            .height = 19,
            .address = 0x00000220,
            .length = 164,
        }; break;
    case 39:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 3,
            .width = 3,
            .height = 5,
            .address = 0x000002c4,
            .length = 8,
        }; break;
    case 40:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 20,
            .advance = 8,
            .width = 6,
            .height = 25,
            .address = 0x000002cc,
            .length = 76,
        }; break;
    case 41:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 20,
            .advance = 8,
            .width = 6,
            .height = 25,
            .address = 0x00000318,
            .length = 76,
        }; break;
    case 42:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 8,
            .width = 8,
            .height = 6,
            .address = 0x00000364,
            .length = 24,
        }; break;
    case 43:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 14,
            .width = 13,
            .height = 12,
            .address = 0x0000037c,
            .length = 80,
        }; break;
    case 44:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 2,
            .advance = 4,
            .width = 4,
            .height = 5,
            .address = 0x000003cc,
            .length = 12,
        }; break;
    case 45:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 8,
            .advance = 12,
            .width = 10,
            .height = 3,
            .address = 0x000003d8,
            .length = 16,
        }; break;
    case 46:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 3,
            .advance = 4,
            .width = 3,
            .height = 3,
            .address = 0x000003e8,
            .length = 8,
        }; break;
    case 47:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 20,
            .advance = 11,
            .width = 12,
            .height = 25,
            .address = 0x000003f0,
            .length = 152,
        }; break;
    case 48:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x00000488,
            .length = 116,
        }; break;
    case 49:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 11,
            .width = 8,
            .height = 19,
            .address = 0x000004fc,
            .length = 76,
        }; break;
    case 50:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x00000548,
            .length = 116,
        }; break;
    case 51:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x000005bc,
            .length = 116,
        }; break;
    case 52:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 14,
            .width = 13,
            .height = 19,
            .address = 0x00000630,
            .length = 124,
        }; break;
    case 53:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x000006ac,
            .length = 116,
        }; break;
    case 54:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x00000720,
            .length = 116,
        }; break;
    case 55:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 13,
            .width = 11,
            .height = 19,
            .address = 0x00000794,
            .length = 108,
        }; break;
    case 56:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x00000800,
            .length = 116,
        }; break;
    case 57:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x00000874,
            .length = 116,
        }; break;
    case 58:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 4,
            .width = 3,
            .height = 13,
            .address = 0x000008e8,
            .length = 20,
        }; break;
    case 59:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 14,
            .advance = 4,
            .width = 4,
            .height = 17,
            .address = 0x000008fc,
            .length = 36,
        }; break;
    case 60:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 13,
            .width = 10,
            .height = 13,
            .address = 0x00000920,
            .length = 68,
        }; break;
    case 61:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 10,
            .advance = 16,
            .width = 14,
            .height = 8,
            .address = 0x00000964,
            .length = 56,
        }; break;
    case 62:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 13,
            .width = 11,
            .height = 13,
            .address = 0x0000099c,
            .length = 72,
        }; break;
    case 63:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 16,
            .width = 14,
            .height = 19,
            .address = 0x000009e4,
            .length = 136,
        }; break;
    case 64:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 16,
            .width = 14,
            .height = 12,
            .address = 0x00000a6c,
            .length = 84,
        }; break;
    case 65:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 18,
            .width = 18,
            .height = 19,
            .address = 0x00000ac0,
            .length = 172,
        }; break;
    case 66:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 16,
            .width = 14,
            .height = 19,
            .address = 0x00000b6c,
            .length = 136,
        }; break;
    case 67:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 20,
            .width = 18,
            .height = 19,
            .address = 0x00000bf4,
            .length = 172,
        }; break;
    case 68:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 20,
            .width = 17,
            .height = 19,
            .address = 0x00000ca0,
            .length = 164,
        }; break;
    case 69:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 14,
            .width = 11,
            .height = 19,
            .address = 0x00000d44,
            .length = 108,
        }; break;
    case 70:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 13,
            .width = 11,
            .height = 19,
            .address = 0x00000db0,
            .length = 108,
        }; break;
    case 71:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 21,
            .width = 18,
            .height = 19,
            .address = 0x00000e1c,
            .length = 172,
        }; break;
    case 72:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 19,
            .width = 15,
            .height = 19,
            .address = 0x00000ec8,
            .length = 144,
        }; break;
    case 73:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 7,
            .width = 3,
            .height = 19,
            .address = 0x00000f58,
            .length = 32,
        }; break;
    case 74:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 9,
            .width = 7,
            .height = 19,
            .address = 0x00000f78,
            .length = 68,
        }; break;
    case 75:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 16,
            .width = 13,
            .height = 19,
            .address = 0x00000fbc,
            .length = 124,
        }; break;
    case 76:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 11,
            .width = 9,
            .height = 19,
            .address = 0x00001038,
            .length = 88,
        }; break;
    case 77:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 24,
            .width = 20,
            .height = 19,
            .address = 0x00001090,
            .length = 192,
        }; break;
    case 78:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 18,
            .width = 14,
            .height = 19,
            .address = 0x00001150,
            .length = 136,
        }; break;
    case 79:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 23,
            .width = 21,
            .height = 19,
            .address = 0x000011d8,
            .length = 200,
        }; break;
    case 80:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x000012a0,
            .length = 116,
        }; break;
    case 81:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 23,
            .width = 21,
            .height = 19,
            .address = 0x00001314,
            .length = 200,
        }; break;
    case 82:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x000013dc,
            .length = 116,
        }; break;
    case 83:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x00001450,
            .length = 116,
        }; break;
    case 84:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x000014c4,
            .length = 116,
        }; break;
    case 85:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 20,
            .width = 16,
            .height = 19,
            .address = 0x00001538,
            .length = 152,
        }; break;
    case 86:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 17,
            .width = 17,
            .height = 19,
            .address = 0x000015d0,
            .length = 164,
        }; break;
    case 87:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 27,
            .width = 28,
            .height = 19,
            .address = 0x00001674,
            .length = 268,
        }; break;
    case 88:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 17,
            .width = 17,
            .height = 19,
            .address = 0x00001780,
            .length = 164,
        }; break;
    case 89:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 15,
            .width = 14,
            .height = 19,
            .address = 0x00001824,
            .length = 136,
        }; break;
    case 90:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 14,
            .width = 14,
            .height = 19,
            .address = 0x000018ac,
            .length = 136,
        }; break;
    case 91:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 9,
            .width = 8,
            .height = 24,
            .address = 0x00001934,
            .length = 96,
        }; break;
    case 92:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 20,
            .advance = 11,
            .width = 12,
            .height = 25,
            .address = 0x00001994,
            .length = 152,
        }; break;
    case 93:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 10,
            .width = 8,
            .height = 24,
            .address = 0x00001a2c,
            .length = 96,
        }; break;
    case 94:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 13,
            .width = 13,
            .height = 10,
            .address = 0x00001a8c,
            .length = 68,
        }; break;
    case 95:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 2,
            .advance = 12,
            .width = 10,
            .height = 3,
            .address = 0x00001ad0,
            .length = 16,
        }; break;
    case 96:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 5,
            .width = 5,
            .height = 4,
            .address = 0x00001ae0,
            .length = 12,
        }; break;
    case 97:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 17,
            .width = 14,
            .height = 12,
            .address = 0x00001aec,
            .length = 84,
        }; break;
    case 98:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 16,
            .width = 14,
            .height = 19,
            .address = 0x00001b40,
            .length = 136,
        }; break;
    case 99:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 13,
            .width = 12,
            .height = 12,
            .address = 0x00001bc8,
            .length = 72,
        }; break;
    case 100:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 17,
            .width = 14,
            .height = 19,
            .address = 0x00001c10,
            .length = 136,
        }; break;
    case 101:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 15,
            .width = 13,
            .height = 12,
            .address = 0x00001c98,
            .length = 80,
        }; break;
    case 102:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 10,
            .width = 9,
            .height = 19,
            .address = 0x00001ce8,
            .length = 88,
        }; break;
    case 103:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 17,
            .width = 14,
            .height = 17,
            .address = 0x00001d40,
            .length = 120,
        }; break;
    case 104:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 14,
            .width = 11,
            .height = 19,
            .address = 0x00001db8,
            .length = 108,
        }; break;
    case 105:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 17,
            .advance = 7,
            .width = 3,
            .height = 17,
            .address = 0x00001e24,
            .length = 28,
        }; break;
    case 106:
        *ch_info = (struct char_info) {
            .xoffset = -3,
            .yoffset = 17,
            .advance = 6,
            .width = 7,
            .height = 22,
            .address = 0x00001e40,
            .length = 80,
        }; break;
    case 107:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 12,
            .width = 9,
            .height = 19,
            .address = 0x00001e90,
            .length = 88,
        }; break;
    case 108:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 7,
            .width = 3,
            .height = 19,
            .address = 0x00001ee8,
            .length = 32,
        }; break;
    case 109:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 12,
            .advance = 20,
            .width = 17,
            .height = 12,
            .address = 0x00001f08,
            .length = 104,
        }; break;
    case 110:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 12,
            .advance = 13,
            .width = 10,
            .height = 12,
            .address = 0x00001f70,
            .length = 60,
        }; break;
    case 111:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 16,
            .width = 14,
            .height = 12,
            .address = 0x00001fac,
            .length = 84,
        }; break;
    case 112:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 12,
            .advance = 16,
            .width = 14,
            .height = 17,
            .address = 0x00002000,
            .length = 120,
        }; break;
    case 113:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 17,
            .width = 14,
            .height = 17,
            .address = 0x00002078,
            .length = 120,
        }; break;
    case 114:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 12,
            .advance = 10,
            .width = 8,
            .height = 12,
            .address = 0x000020f0,
            .length = 48,
        }; break;
    case 115:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 11,
            .width = 9,
            .height = 12,
            .address = 0x00002120,
            .length = 56,
        }; break;
    case 116:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 17,
            .advance = 10,
            .width = 8,
            .height = 17,
            .address = 0x00002158,
            .length = 68,
        }; break;
    case 117:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 13,
            .width = 11,
            .height = 12,
            .address = 0x0000219c,
            .length = 68,
        }; break;
    case 118:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 12,
            .advance = 11,
            .width = 11,
            .height = 12,
            .address = 0x000021e0,
            .length = 68,
        }; break;
    case 119:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 12,
            .advance = 20,
            .width = 20,
            .height = 12,
            .address = 0x00002224,
            .length = 120,
        }; break;
    case 120:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 12,
            .advance = 12,
            .width = 11,
            .height = 12,
            .address = 0x0000229c,
            .length = 68,
        }; break;
    case 121:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 12,
            .advance = 11,
            .width = 11,
            .height = 17,
            .address = 0x000022e0,
            .length = 96,
        }; break;
    case 122:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 12,
            .advance = 12,
            .width = 12,
            .height = 12,
            .address = 0x00002340,
            .length = 72,
        }; break;
    case 123:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 10,
            .width = 9,
            .height = 24,
            .address = 0x00002388,
            .length = 108,
        }; break;
    case 124:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 7,
            .width = 3,
            .height = 24,
            .address = 0x000023f4,
            .length = 36,
        }; break;
    case 125:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 9,
            .width = 7,
            .height = 24,
            .address = 0x00002418,
            .length = 84,
        }; break;
    case 126:
        *ch_info = (struct char_info) {
            .xoffset = 3,
            .yoffset = 19,
            .advance = 14,
            .width = 8,
            .height = 4,
            .address = 0x0000246c,
            .length = 16,
        }; break;
    case 161:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 7,
            .width = 3,
            .height = 19,
            .address = 0x0000247c,
            .length = 32,
        }; break;
    case 162:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 14,
            .advance = 13,
            .width = 11,
            .height = 16,
            .address = 0x0000249c,
            .length = 88,
        }; break;
    case 163:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 16,
            .width = 14,
            .height = 19,
            .address = 0x000024f4,
            .length = 136,
        }; break;
    case 165:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 16,
            .width = 15,
            .height = 19,
            .address = 0x0000257c,
            .length = 144,
        }; break;
    case 166:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 7,
            .width = 3,
            .height = 24,
            .address = 0x0000260c,
            .length = 36,
        }; break;
    case 168:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 17,
            .advance = 8,
            .width = 7,
            .height = 3,
            .address = 0x00002630,
            .length = 12,
        }; break;
    case 169:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 20,
            .advance = 13,
            .width = 13,
            .height = 13,
            .address = 0x0000263c,
            .length = 88,
        }; break;
    case 171:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 18,
            .width = 16,
            .height = 13,
            .address = 0x00002694,
            .length = 104,
        }; break;
    case 172:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 14,
            .advance = 20,
            .width = 16,
            .height = 9,
            .address = 0x000026fc,
            .length = 72,
        }; break;
    case 174:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 20,
            .advance = 13,
            .width = 13,
            .height = 13,
            .address = 0x00002744,
            .length = 88,
        }; break;
    case 176:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 20,
            .advance = 7,
            .width = 7,
            .height = 7,
            .address = 0x0000279c,
            .length = 28,
        }; break;
    case 177:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 15,
            .advance = 14,
            .width = 13,
            .height = 15,
            .address = 0x000027b8,
            .length = 100,
        }; break;
    case 180:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 5,
            .width = 5,
            .height = 4,
            .address = 0x0000281c,
            .length = 12,
        }; break;
    case 181:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 12,
            .advance = 15,
            .width = 11,
            .height = 17,
            .address = 0x00002828,
            .length = 96,
        }; break;
    case 182:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 14,
            .width = 11,
            .height = 19,
            .address = 0x00002888,
            .length = 108,
        }; break;
    case 183:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 11,
            .advance = 5,
            .width = 4,
            .height = 4,
            .address = 0x000028f4,
            .length = 8,
        }; break;
    case 184:
        *ch_info = (struct char_info) {
            .xoffset = 11,
            .yoffset = 1,
            .advance = 27,
            .width = 4,
            .height = 6,
            .address = 0x000028fc,
            .length = 12,
        }; break;
    case 187:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 13,
            .advance = 18,
            .width = 16,
            .height = 13,
            .address = 0x00002908,
            .length = 104,
        }; break;
    case 191:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 16,
            .width = 14,
            .height = 19,
            .address = 0x00002970,
            .length = 136,
        }; break;
    case 192:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 24,
            .advance = 18,
            .width = 17,
            .height = 24,
            .address = 0x000029f8,
            .length = 204,
        }; break;
    case 193:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 24,
            .advance = 18,
            .width = 17,
            .height = 24,
            .address = 0x00002ac4,
            .length = 204,
        }; break;
    case 194:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 24,
            .advance = 18,
            .width = 17,
            .height = 24,
            .address = 0x00002b90,
            .length = 204,
        }; break;
    case 195:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 23,
            .advance = 18,
            .width = 17,
            .height = 23,
            .address = 0x00002c5c,
            .length = 196,
        }; break;
    case 196:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 23,
            .advance = 18,
            .width = 18,
            .height = 23,
            .address = 0x00002d20,
            .length = 208,
        }; break;
    case 197:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 24,
            .advance = 18,
            .width = 19,
            .height = 24,
            .address = 0x00002df0,
            .length = 228,
        }; break;
    case 198:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 19,
            .advance = 27,
            .width = 26,
            .height = 19,
            .address = 0x00002ed4,
            .length = 248,
        }; break;
    case 199:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 21,
            .width = 18,
            .height = 24,
            .address = 0x00002fcc,
            .length = 216,
        }; break;
    case 200:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 24,
            .advance = 14,
            .width = 11,
            .height = 24,
            .address = 0x000030a4,
            .length = 132,
        }; break;
    case 201:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 24,
            .advance = 14,
            .width = 11,
            .height = 24,
            .address = 0x00003128,
            .length = 132,
        }; break;
    case 202:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 24,
            .advance = 14,
            .width = 11,
            .height = 24,
            .address = 0x000031ac,
            .length = 132,
        }; break;
    case 203:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 23,
            .advance = 13,
            .width = 11,
            .height = 23,
            .address = 0x00003230,
            .length = 128,
        }; break;
    case 204:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 24,
            .advance = 6,
            .width = 5,
            .height = 24,
            .address = 0x000032b0,
            .length = 60,
        }; break;
    case 205:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 24,
            .advance = 6,
            .width = 5,
            .height = 24,
            .address = 0x000032ec,
            .length = 60,
        }; break;
    case 206:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 24,
            .advance = 6,
            .width = 7,
            .height = 24,
            .address = 0x00003328,
            .length = 84,
        }; break;
    case 207:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 23,
            .advance = 6,
            .width = 7,
            .height = 23,
            .address = 0x0000337c,
            .length = 84,
        }; break;
    case 208:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 20,
            .width = 18,
            .height = 19,
            .address = 0x000033d0,
            .length = 172,
        }; break;
    case 209:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 23,
            .advance = 17,
            .width = 14,
            .height = 23,
            .address = 0x0000347c,
            .length = 164,
        }; break;
    case 210:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 24,
            .advance = 23,
            .width = 21,
            .height = 24,
            .address = 0x00003520,
            .length = 252,
        }; break;
    case 211:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 24,
            .advance = 23,
            .width = 21,
            .height = 24,
            .address = 0x0000361c,
            .length = 252,
        }; break;
    case 212:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 24,
            .advance = 23,
            .width = 21,
            .height = 24,
            .address = 0x00003718,
            .length = 252,
        }; break;
    case 213:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 23,
            .advance = 23,
            .width = 21,
            .height = 23,
            .address = 0x00003814,
            .length = 244,
        }; break;
    case 214:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 23,
            .advance = 24,
            .width = 21,
            .height = 23,
            .address = 0x00003908,
            .length = 244,
        }; break;
    case 215:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 12,
            .advance = 14,
            .width = 13,
            .height = 12,
            .address = 0x000039fc,
            .length = 80,
        }; break;
    case 216:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 22,
            .advance = 23,
            .width = 21,
            .height = 25,
            .address = 0x00003a4c,
            .length = 264,
        }; break;
    case 217:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 24,
            .advance = 19,
            .width = 16,
            .height = 24,
            .address = 0x00003b54,
            .length = 192,
        }; break;
    case 218:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 24,
            .advance = 19,
            .width = 16,
            .height = 24,
            .address = 0x00003c14,
            .length = 192,
        }; break;
    case 219:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 24,
            .advance = 19,
            .width = 16,
            .height = 24,
            .address = 0x00003cd4,
            .length = 192,
        }; break;
    case 220:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 23,
            .advance = 19,
            .width = 16,
            .height = 23,
            .address = 0x00003d94,
            .length = 184,
        }; break;
    case 221:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 24,
            .advance = 14,
            .width = 14,
            .height = 24,
            .address = 0x00003e4c,
            .length = 168,
        }; break;
    case 222:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x00003ef4,
            .length = 116,
        }; break;
    case 223:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 14,
            .width = 12,
            .height = 19,
            .address = 0x00003f68,
            .length = 116,
        }; break;
    case 224:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 16,
            .width = 14,
            .height = 18,
            .address = 0x00003fdc,
            .length = 128,
        }; break;
    case 225:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 16,
            .width = 14,
            .height = 18,
            .address = 0x0000405c,
            .length = 128,
        }; break;
    case 226:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 16,
            .width = 14,
            .height = 18,
            .address = 0x000040dc,
            .length = 128,
        }; break;
    case 227:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 17,
            .width = 14,
            .height = 18,
            .address = 0x0000415c,
            .length = 128,
        }; break;
    case 228:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 17,
            .advance = 16,
            .width = 14,
            .height = 17,
            .address = 0x000041dc,
            .length = 120,
        }; break;
    case 229:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 16,
            .width = 14,
            .height = 19,
            .address = 0x00004254,
            .length = 136,
        }; break;
    case 230:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 24,
            .width = 22,
            .height = 12,
            .address = 0x000042dc,
            .length = 132,
        }; break;
    case 231:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 13,
            .width = 12,
            .height = 17,
            .address = 0x00004360,
            .length = 104,
        }; break;
    case 232:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 15,
            .width = 13,
            .height = 18,
            .address = 0x000043c8,
            .length = 120,
        }; break;
    case 233:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 15,
            .width = 13,
            .height = 18,
            .address = 0x00004440,
            .length = 120,
        }; break;
    case 234:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 15,
            .width = 13,
            .height = 18,
            .address = 0x000044b8,
            .length = 120,
        }; break;
    case 235:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 17,
            .advance = 15,
            .width = 13,
            .height = 17,
            .address = 0x00004530,
            .length = 112,
        }; break;
    case 236:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 18,
            .advance = 7,
            .width = 5,
            .height = 18,
            .address = 0x000045a0,
            .length = 48,
        }; break;
    case 237:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 18,
            .advance = 7,
            .width = 5,
            .height = 18,
            .address = 0x000045d0,
            .length = 48,
        }; break;
    case 238:
        *ch_info = (struct char_info) {
            .xoffset = 0,
            .yoffset = 18,
            .advance = 6,
            .width = 7,
            .height = 18,
            .address = 0x00004600,
            .length = 64,
        }; break;
    case 239:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 17,
            .advance = 7,
            .width = 7,
            .height = 17,
            .address = 0x00004640,
            .length = 60,
        }; break;
    case 240:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 19,
            .advance = 17,
            .width = 15,
            .height = 19,
            .address = 0x0000467c,
            .length = 144,
        }; break;
    case 241:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 18,
            .advance = 13,
            .width = 10,
            .height = 18,
            .address = 0x0000470c,
            .length = 92,
        }; break;
    case 242:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 15,
            .width = 14,
            .height = 18,
            .address = 0x00004768,
            .length = 128,
        }; break;
    case 243:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 15,
            .width = 14,
            .height = 18,
            .address = 0x000047e8,
            .length = 128,
        }; break;
    case 244:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 15,
            .width = 14,
            .height = 18,
            .address = 0x00004868,
            .length = 128,
        }; break;
    case 245:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 15,
            .width = 14,
            .height = 18,
            .address = 0x000048e8,
            .length = 128,
        }; break;
    case 246:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 17,
            .advance = 15,
            .width = 14,
            .height = 17,
            .address = 0x00004968,
            .length = 120,
        }; break;
    case 247:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 12,
            .advance = 17,
            .width = 15,
            .height = 12,
            .address = 0x000049e0,
            .length = 92,
        }; break;
    case 248:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 14,
            .advance = 15,
            .width = 14,
            .height = 16,
            .address = 0x00004a3c,
            .length = 112,
        }; break;
    case 249:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 18,
            .advance = 14,
            .width = 11,
            .height = 18,
            .address = 0x00004aac,
            .length = 100,
        }; break;
    case 250:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 18,
            .advance = 14,
            .width = 11,
            .height = 18,
            .address = 0x00004b10,
            .length = 100,
        }; break;
    case 251:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 18,
            .advance = 14,
            .width = 11,
            .height = 18,
            .address = 0x00004b74,
            .length = 100,
        }; break;
    case 252:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 17,
            .advance = 14,
            .width = 11,
            .height = 17,
            .address = 0x00004bd8,
            .length = 96,
        }; break;
    case 253:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 18,
            .advance = 11,
            .width = 10,
            .height = 23,
            .address = 0x00004c38,
            .length = 116,
        }; break;
    case 254:
        *ch_info = (struct char_info) {
            .xoffset = 2,
            .yoffset = 19,
            .advance = 16,
            .width = 14,
            .height = 24,
            .address = 0x00004cac,
            .length = 168,
        }; break;
    case 255:
        *ch_info = (struct char_info) {
            .xoffset = 1,
            .yoffset = 17,
            .advance = 11,
            .width = 10,
            .height = 22,
            .address = 0x00004d54,
            .length = 112,
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
        case 87:
            return -1;
        case 89:
            return -2;
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
            return -1;
        case 198:
            return -2;
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
            return -2;
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
            return -2;
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
            return -1;
        case 198:
            return -2;
        default:
            return 0;
        }
    case 87:
        switch(ch2) {
        case 65:
        case 81:
        case 97:
        case 101:
        case 103:
        case 105:
        case 111:
        case 113:
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
    case 89:
        switch(ch2) {
        case 79:
        case 210:
        case 211:
        case 212:
        case 213:
        case 214:
        case 216:
            return -1;
        case 65:
        case 192:
        case 193:
        case 194:
        case 195:
        case 196:
        case 197:
        case 198:
            return -2;
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
        case 87:
            return -1;
        case 84:
            return -2;
        default:
            return 0;
        }
    case 99:
        switch(ch2) {
        case 84:
            return -2;
        default:
            return 0;
        }
    case 101:
        switch(ch2) {
        case 87:
            return -1;
        case 84:
            return -2;
        default:
            return 0;
        }
    case 103:
        switch(ch2) {
        case 84:
            return -2;
        default:
            return 0;
        }
    case 105:
        switch(ch2) {
        case 87:
            return -1;
        default:
            return 0;
        }
    case 111:
        switch(ch2) {
        case 87:
            return -1;
        case 84:
            return -2;
        default:
            return 0;
        }
    case 113:
        switch(ch2) {
        case 84:
            return -2;
        default:
            return 0;
        }
    case 114:
        switch(ch2) {
        case 84:
            return -2;
        default:
            return 0;
        }
    case 115:
        switch(ch2) {
        case 84:
            return -2;
        default:
            return 0;
        }
    case 117:
        switch(ch2) {
        case 84:
            return -2;
        default:
            return 0;
        }
    case 119:
        switch(ch2) {
        case 84:
            return -2;
        default:
            return 0;
        }
    case 192:
        switch(ch2) {
        case 86:
        case 87:
            return -1;
        case 89:
            return -2;
        default:
            return 0;
        }
    case 193:
        switch(ch2) {
        case 86:
        case 87:
            return -1;
        default:
            return 0;
        }
    case 194:
        switch(ch2) {
        case 86:
        case 87:
            return -1;
        case 89:
            return -2;
        default:
            return 0;
        }
    case 195:
        switch(ch2) {
        case 86:
        case 87:
            return -1;
        case 89:
            return -2;
        default:
            return 0;
        }
    case 196:
        switch(ch2) {
        case 86:
        case 87:
            return -1;
        case 89:
            return -2;
        default:
            return 0;
        }
    case 197:
        switch(ch2) {
        case 86:
        case 87:
            return -1;
        case 89:
            return -2;
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

