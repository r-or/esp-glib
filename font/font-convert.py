#!/usr/bin/env python3

import sys
from xml.dom import minidom
import png
from array import array
from matplotlib import pyplot as plt
import numpy as np
import math
import struct

def printHelp():
    print("Usage:", \
        "\tpython3 font-convert.py", \
        "\n mandatory:", \
        "\n\t <font.xml>", \
        "\n\t <font.png>", \
        "\n optional:", \
        "\n\t -d [int: bit depth]", \
        "\n\t -be\t -> big endian (if -d > 8)", \
        "\n\t -ic\t -> store in iRAM instead of flash", \
        "\n\t -a [int: aligned by bytes]")


if len(sys.argv) > 2:
    fxml = sys.argv[1]
    fpng = sys.argv[2]
else:
    printHelp()
    sys.exit()
    
tdepth = 8
talign = 0
lendian = 1
storeonflash = 1

idx = 3
while idx < len(sys.argv):
    if sys.argv[idx] == '-d':
        if len(sys.argv) - 1 > idx:
            tdepth = int(sys.argv[idx + 1])
            idx += 1
    elif sys.argv[idx] == '-a':
        if len(sys.argv) - 1 > idx:
            talign = int(sys.argv[idx + 1])
            idx += 1
    elif sys.argv[idx] == '-be':
        lendian = 0
    elif sys.argv[idx] == '-ic':
        storeonflash = 0
    elif sys.argv[idx] == '-h':
        printHelp()
        sys.exit()
    idx += 1
    
    
doc = minidom.parse(fxml)

fdesc = doc.getElementsByTagName('description')[0]
fmets = doc.getElementsByTagName('metrics')[0]
ftex = doc.getElementsByTagName('texture')[0]

fontname = fdesc.attributes['family'].value + '_' + fdesc.attributes['style'].value + '_' + fmets.attributes['ascender'].value


defs = '#ifndef __FONT_H__\n#define __FONT_H__\n\n'

defs += '#include "stdint.h"\n\n'

declares = \
    'struct font_info {\n' \
    '    uint32_t font_size;\t\t// size in bytes\n' \
    '    uint8_t pix_depth;\t\t// bits per pixel\n' \
    '    const char *font_name;\n' \
    '    uint8_t font_height;\t\t// height including line spacing\n' \
    '    uint8_t font_ascend;\t\t// usual ascent excluding accents\n' \
    '};\n\n' \
    'struct char_info {\n' \
    '    int8_t xoffset;\t\t// pixel offset to top left corner\n' \
    '    int8_t yoffset;\n' \
    '    uint8_t advance;\n' \
    '    uint32_t address;\n' \
    '    uint16_t length;\t\t// length in bytes\n' \
    '    uint8_t width;\n' \
    '    uint8_t height;\n' \
    '};\n\n'
    
declares += \
    '// latin-1 encoding!\n' \
    'uint8_t get_char(struct char_info *const ch_info, const uint8_t ch);\n\n' \
    'int8_t get_kerning(const uint8_t ch1, const uint8_t ch2);\n\n' \
    'void get_font_info(struct font_info *const finfo);\n\n'

body = \
    '{} get_char(struct char_info *const ch_info, const uint8_t ch) {{\n' \
    '    switch(ch) {{\n'.format('static uint8_t' if not storeonflash else 'uint8_t ICACHE_FLASH_ATTR')
    
    
preader = png.Reader(filename=fpng)
pngw, pngh, pngpixels, pngmeta = preader.read_flat()
pngpixwidth = 4 if pngmeta['alpha'] else 3
gspixels = array('B')

if pngpixwidth == 4:
    if pngpixels[0::4][1:] == pngpixels[0::4][:-1]:      # all R values equal -> no information in RGB channels
        gspixels.extend(pngpixels[3::4])  # use alpha channel
    else:
        gspixels.extend(pngpixels[0::4])  # assume greyscale... TODO: convert RGB to greyscale first
else:
    gspixels.extend(pngpixels[0::3])

chars = doc.getElementsByTagName('char')

bytearr = array('B')
maxfontlen = 0
maxfontwidth = 0
minfontwidth = 100
maxfontheight = 0
maxfontascent = 0

for char in chars:
    chx = int(char.attributes['rect'].value.split()[0])
    chy = int(char.attributes['rect'].value.split()[1])
    chw = int(char.attributes['rect'].value.split()[2])
    chh = int(char.attributes['rect'].value.split()[3])
    chxoff = int(char.attributes['offset'].value.split()[0])
    chyoff = int(char.attributes['offset'].value.split()[1])

    subimage = array('B')
    for line in range(0, chh):
        subimage.extend(gspixels[chx + (chy + line) * pngw : (chx + chw) + (chy + line) * pngw])

    subimagearr = np.array(subimage)
#    print(subimagearr, subimagearr.shape, chh, chw)
#    print('current ID:', char.attributes['id'].value)
#    plt.imshow(np.reshape(subimagearr, (chh, chw)), interpolation='nearest', cmap='Greys_r')
#    plt.axis('equal')
#    ax = plt.gca()
#    ax.set_axis_bgcolor('black')
#    plt.show()

    # quantise
    subimagearr = np.array([int(round(k / \
        ((math.pow(2, pngmeta['bitdepth']) / math.pow(2, tdepth)) + 1))) for k in subimagearr])

    # stack bytes
    subimagebytearr = array('B')
    bytenum = math.ceil(len(subimagearr) / (8 / tdepth))
    j = 0
    oh = 0
    for i in range(0, bytenum):
        subimagebytearr.append(0)
        k = 8
        while k > 0 and j < len(subimagearr):
            k -= tdepth
            #print(subimagearr[j], k, oh)
            if k >= 0:
                subimagebytearr[-1] |= ((subimagearr[j] << k + oh) & 0xFF)
                oh = 0
            else:
                subimagebytearr[-1] |= subimagearr[j] >> -k
                j -= 1
                oh = tdepth + k
            j += 1

    # store in bytelist
    if talign:
        while len(subimagebytearr) % talign:
            subimagebytearr.append(0)
    chaddress = len(bytearr)
    bytearr.extend(subimagebytearr)
    
    
    # BODY
    #print(char.attributes['id'].value, '->', char.attributes['id'].value.encode('latin-1')[0])
    body += '    case {}:\n' \
        '        *ch_info = (struct char_info) {{\n' \
        '            .xoffset = {},\n' \
        '            .yoffset = {},\n' \
        '            .advance = {},\n' \
        '            .width = {},\n' \
        '            .height = {},\n' \
        '            .address = 0x{:08x},\n' \
        '            .length = {},\n' \
        '        }}; break;\n'.format(char.attributes['id'].value.encode('latin-1')[0], \
                                         chxoff, \
                                         chyoff, \
                                         char.attributes['advance'].value, \
                                         chw, \
                                         chh, \
                                         chaddress, \
                                         len(subimagebytearr))
                                         
    if len(subimagebytearr) > maxfontlen:
        maxfontlen = len(subimagebytearr)
    if chw > maxfontwidth:
        maxfontwidth = chw
    if chw and char.getElementsByTagName('kerning') != [] and chw < minfontwidth:
        minfontwidth = chw;
    if chh > maxfontheight:
        maxfontheight = chh
    if chyoff > maxfontascent:
        maxfontascent = chyoff
    
body += \
    '    default: return 1;\n' \
    '    }}\n' \
    '    return 0;\n' \
    '}}\n\n' \
    '{} get_kerning(const uint8_t ch1, const uint8_t ch2) {{\n' \
    '    switch(ch1) {{\n'.format('static int8_t' if not storeonflash else 'int8_t ICACHE_FLASH_ATTR')
    
# store image
outbname = fpng.rstrip('.png').rstrip('.PNG').split('/')[-1] + '_{}bit.gray'.format(tdepth)
outb = open(outbname, 'wb')

outb.write(struct.pack('{}{}B'.format('<' if lendian else '>', len(bytearr)), *bytearr))
outb.close()
    
    
for char in chars:
    kernings = char.getElementsByTagName('kerning')
    if kernings != []:
        lkern = []
        for kerning in kernings:
            lkern.append((kerning.attributes['id'].value.encode('latin-1')[0], kerning.attributes['advance'].value))
        
        lkern = sorted(lkern, key=lambda x: x[1])
        #print(lkern)
        
        body += \
            '    case {}:\n' \
            '        switch(ch2) {{\n'.format(char.attributes['id'].value.encode('latin-1')[0])
        for i in range(0, len(lkern)):
            if i > 0 and int(lkern[i - 1][1]) != int(lkern[i][1]):
                body += '            return {};\n'.format(lkern[i - 1][1])
            body += '        case {}:\n'.format(lkern[i][0])
        body += '            return {};\n' \
            '        default:\n' \
            '            return 0;\n' \
            '        }}\n'.format(lkern[-1][1])
        
body += \
    '    default:\n' \
    '        return 0;\n' \
    '    }\n' \
    '}\n\n'

body = \
    '{} get_font_info(struct font_info *const finfo) {{\n' \
    '    *finfo = (struct font_info) {{\n' \
    '        .font_size = {},\n' \
    '        .pix_depth = {},\n' \
    '        .font_name = \"{}\",\n' \
    '        .font_height = {},\n' \
    '        .font_ascend = {},\n' \
    '    }};\n' \
    '}}\n\n'.format('static void' if not storeonflash else 'void ICACHE_FLASH_ATTR', len(bytearr), tdepth, fontname, \
        fmets.attributes['height'].value, fmets.attributes['ascender'].value) \
    + body
 
if storeonflash:
    body = '#include "user_interface.h"\n\n' + body
 
defs += \
    '#define _FONT_{}_{}_\n' \
    '#define _FONT_MAX_CHAR_SIZE_INT8_ \t{}\n' \
    '#define _FONT_MAX_CHAR_SIZE_INT32_ \t{}\n' \
    '#define _FONT_MAX_CHAR_WIDTH_ \t{}\n' \
    '#define _FONT_MIN_CHAR_WIDTH_ \t{}\n' \
    '#define _FONT_MAX_CHAR_HEIGHT_ \t{}\n' \
    '#define _FONT_MAX_CHAR_ASCENT_ \t{}\n' \
    '\n'.format(fontname.upper(), fontname.upper(), maxfontlen, math.ceil(maxfontlen / 4), \
        maxfontwidth, minfontwidth, maxfontheight, maxfontascent);
 
declares += \
    '#endif';
    
outname = 'font'
out = open(outname + '.h', 'w')
out.write(defs + declares);
out.close()
out = open(outname + '.c', 'w')
out.write('#include "{}h"\n\n'.format(outname) + body)

print('Processed {} glyphs. Size: {} bytes.'.format(len(chars), len(bytearr)))
print('Created {}.h, {}.c. Copy to ../. Also do a clean make' \
    ' or gcc will not update the addresses!'.format(outname, outname))
print('Created bitmap:', outbname)
