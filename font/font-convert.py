#!/usr/bin/env python3

import sys
from xml.dom import minidom
import png
from array import array
from matplotlib import pyplot as plt
import numpy as np
import math
import struct


# FUNCTIONS
def printHelp():
    print("Usage:", \
        "\tpython3 png2gs.py", \
        "\n mandatory:", \
        "\n <generic.png>", \
        "\n OR", \
        "\n mandatory:", \
        "\n\t -f", \
        "\n\t <font.xml>", \
        "\n\t <font.png>", \
        "\n\t <generic.png>"
        "\n optional (some options will be ignored depending on above choice):", \
        "\n\t -d [int: bit depth]", \
        "\n\t -be\t -> big endian (if -d > 8)", \
        "\n\t -ic\t -> store in iRAM instead of flash", \
        "\n\t -a [int: aligned by bytes]", \
        "\n\t -p\t -> show preview")
        

#   stack(
#       ndarray(image),
#       int(targetDepth))
def stack(imgarray8bit, tdepth):
    stackedarr = array('B')
    bytenum = math.ceil(len(imgarray8bit) / (8 / tdepth))
    j = 0
    oh = 0
    for i in range(0, bytenum):
        stackedarr.append(0)
        k = 8
        while k > 0 and j < len(imgarray8bit):
            k -= tdepth
            #print(subimagearr[j], k, oh)
            if k >= 0:
                stackedarr[-1] |= ((imgarray8bit[j] << k + oh) & 0xFF)
                oh = 0
            else:
                stackedarr[-1] |= imgarray8bit[j] >> -k
                j -= 1
                oh = tdepth + k
            j += 1
            
    return stackedarr
    

#   quantise(
#       ndarray(image),
#       int(imageDepth),
#       int(targetDepth))
def quantise(imgarray, depth, tdepth):
    return np.array([int(round(k / \
            ((math.pow(2, depth) / math.pow(2, tdepth)) + 1))) for k in imgarray])
            
            
#   saveimg(
#       ndarray(image),
#       bool(lendian),
#       str(name))
def saveimg(imgarray, twlen, littleendian, outname):        # TODO: endianness
    outb = open(outname, 'wb')
    #imgarray = np.array();
    #for i in range(0, (len(imgarray8bit) + (twlen / 8) - 1) / (twlen / 8)):
    #    imgarray.append(0)
    #    for k in range(0, twlen / 8):
    #        imgarray[-1] |= imgarray8bit[i * (twlen / 8) + k] << (twlen - (k + 1) * twlen / 8)
        
    outb.write(struct.pack('{}{}B'.format('<' if littleendian else '>', len(imgarray)), *imgarray))
    outb.close()
    print('Created bitmap:', outname)



# MAIN
tdepth = 8
twlen = 32
talign = 0
lendian = 1
storeonflash = 1
font = False
preview = False

idx = 1
while idx < len(sys.argv):
    if sys.argv[idx] == '-f':
        font = True;
    elif sys.argv[idx] == '-d':
        if len(sys.argv) - 1 > idx:
            tdepth = int(sys.argv[idx + 1])
            idx += 1
    elif sys.argv[idx] == '-w':
        if len(sys.argv) - 1 > idx:
            twlen = int(sys.argv[idx + 1])
            idx += 1
    elif sys.argv[idx] == '-a':
        if len(sys.argv) - 1 > idx:
            talign = int(sys.argv[idx + 1])
            idx += 1
    elif sys.argv[idx] == '-be':
        lendian = 0
    elif sys.argv[idx] == '-ic':
        storeonflash = 0
    elif sys.argv[idx] == '-p':
        preview = True;
    elif sys.argv[idx] == '-h':
        printHelp()
        sys.exit()
    idx += 1
    
if len(sys.argv) < 2 or (not font and len(sys.argv) >= 2 and sys.argv[2][0] != '-'):
    printHelp()
    sys.exit()
    
if font:
    fxml = sys.argv[1]
    fpng = sys.argv[2]
else:
    fpng = sys.argv[1]
    
    
# BITMAP CONVERSION
preader = png.Reader(filename=fpng)
pngw, pngh, pngpixels, pngmeta = preader.read_flat()
pngpixwidth = 4 if pngmeta['alpha'] else 3
gspixels = array('B')

if pngpixwidth == 4:
#    if pngpixels[0::4][1:] == pngpixels[0::4][:-1]:      # all R values equal -> no information in RGB channels
    gspixels.extend(pngpixels[3::4])  # use alpha channel
#    print("using alpha")
#    else:
#        gspixels.extend(pngpixels[0::4])  # assume greyscale... TODO: convert RGB to greyscale first
#        print("using RGB")
else:
    gspixels.extend(pngpixels[0::3])
    
# quantise
gspixels = quantise(np.array(gspixels), pngmeta['bitdepth'], tdepth)


print(gspixels, gspixels.shape, pngh, pngw)
if preview:
    plt.imshow(np.reshape(gspixels, (pngh, pngw)), interpolation='nearest', cmap='Greys_r')
    plt.axis('equal')
    ax = plt.gca()
    ax.set_axis_bgcolor('black')
    plt.show()

if not font:
    gspixels = stack(gspixels, tdepth)
    outname = fpng.rstrip('.png').rstrip('.PNG').split('/')[-1] + '_{}bit.gray'.format(tdepth)
    saveimg(gspixels, twlen, lendian, outname)
    sys.exit()
    
    
# FONT
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
    chadv = int(char.attributes['advance'].value)
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

    # stack bytes
    subimagebytearr = stack(subimagearr, tdepth)
    
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
                                         chadv, \
                                         chw, \
                                         chh, \
                                         chaddress, \
                                         len(subimagebytearr))
                                         
    if len(subimagebytearr) > maxfontlen:
        maxfontlen = len(subimagebytearr)
    if chw > maxfontwidth:
        maxfontwidth = chw
    if chadv < minfontwidth:
        minfontwidth = chadv;
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
saveimg(bytearr, twlen, lendian, outbname)
    
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
    '#define _FONT_{}_\n' \
    '#define _FONT_MAX_CHAR_SIZE_INT8_ \t{}\t// bytes to store biggest char (2D)\n' \
    '#define _FONT_MAX_CHAR_SIZE_INT32_ \t{}\n' \
    '#define _FONT_MAX_CHAR_WIDTH_ \t{}\t// pix\n' \
    '#define _FONT_MAX_CHAR_HEIGHT_ \t{}\t// pix\n' \
    '#define _FONT_MIN_CHAR_WIDTH_ \t{}\t// useful for worst case num of printable chars (min advance)\n' \
    '#define _FONT_MAX_CHAR_ASCENT_ \t{}\t// max char height with respect to base line (for cursor placement)\n' \
    '\n'.format(fontname.upper(), maxfontlen, math.ceil(maxfontlen / 4), \
        maxfontwidth, maxfontheight, minfontwidth, maxfontascent);
 
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
