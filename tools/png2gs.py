#!/usr/bin/env python3

import sys
from xml.dom import minidom
import png
from array import array
from matplotlib import pyplot as plt
import numpy as np
import math
import struct
import copy



# CLASSES

class BmpStack:
    def __init__(self, bitmaps, targetDepth, targetWordLength, byteAlign):
        self.lbmp = bitmaps
        self.tdepth = targetDepth
        self.twlen = targetWordLength
        self.align = byteAlign
        self.lbmpFinal = []
        self.bmpaddress = []
        self.fnames = []
        self.sizes = []
        self.dims = []
        self.prefix = 'ass'
        
        self.bmpSuffixes = ['.png', '.PNG']
        
    def filename(self, string, suffix):
        for s in suffix:
            string = string.replace(s, '')
        string = string.split('/')[-1]
        return string
        
    #   quantise(
    #       ndarray(image),
    #       int(targetDepth))
    def quantise(self, imgarray, depth):
        return np.array([int(round(k / \
                ((math.pow(2, depth) / math.pow(2, self.tdepth)) + 1))) for k in imgarray])
                
                
    # consecutive pixels inside memory:
    #   stuff(
    #       ndarray(image))
    def stuff(self, imgarray8bit):
        stuffedarr = array('B')
        bytenum = math.ceil(len(imgarray8bit) / (8 / self.tdepth))
        j = 0
        oh = 0
        for i in range(0, bytenum):
            stuffedarr.append(0)
            k = 8
            while k > 0 and j < len(imgarray8bit):
                k -= self.tdepth
                #print(subimagearr[j], k, oh)
                if k >= 0:
                    stuffedarr[-1] |= ((imgarray8bit[j] << k + oh) & 0xFF)
                    oh = 0
                else:
                    stuffedarr[-1] |= imgarray8bit[j] >> -k
                    j -= 1
                    oh = self.tdepth + k
                j += 1
                
        return stuffedarr
        
        
    #   saveimg(
    #       ndarray(image),
    #       int(tarWordLength)
    #       bool(lendian),
    #       str(name))
    def saveimg(self, imgarray, twlen, littleendian, outname):        # TODO: endianness
        outb = open(outname, 'wb')
        #imgarray = np.array();
        #for i in range(0, (len(imgarray8bit) + (twlen / 8) - 1) / (twlen / 8)):
        #    imgarray.append(0)
        #    for k in range(0, twlen / 8):
        #        imgarray[-1] |= imgarray8bit[i * (twlen / 8) + k] << (twlen - (k + 1) * twlen / 8)
            
        outb.write(struct.pack('{}{}B'.format('<' if littleendian else '>', len(imgarray)), *imgarray))
        outb.close()
        print('Created bitmap:', outname + ',', len(imgarray), 'bytes on flash.')
        
        
    def read(self, showBmp):
        for fbmp in self.lbmp:
            preader = png.Reader(filename=fbmp)
            pngw, pngh, pngpixels, pngmeta = preader.read_flat()
            pngchannels = 1 if pngmeta['greyscale'] else 3
            if pngmeta['alpha']:
                pngchannels += 1
                
            gspixels = array('B')

            if pngchannels == 4:
            #    if pngpixels[0::4][1:] == pngpixels[0::4][:-1]:      # all R values equal -> no information in RGB channels
                gspixels.extend(pngpixels[3::4])  # use alpha channel
            #    print("using alpha")
            #    else:
            #        gspixels.extend(pngpixels[0::4])  # assume greyscale... TODO: convert RGB to greyscale first
            #        print("using RGB")
            elif pngchannels == 2:
                gspixels.extend(pngpixels[1::2])
            elif pngchannels == 1:
                gspixels.extend(pngpixels)
            else:   # just use red channel
                gspixels.extend(pngpixels[0::3])
            
            # quantise
            gspixels = self.quantise(np.array(gspixels), pngmeta['bitdepth'])

            self.lbmpFinal.append(gspixels)
        
            #print(gspixels, gspixels.shape, pngh, pngw)
            self.fnames.append(self.filename(fbmp, self.bmpSuffixes).upper())
            self.dims.append((pngw, pngh))
            
            if showBmp:
                plt.imshow(np.reshape(gspixels, (pngh, pngw)), interpolation='nearest', cmap='Greys_r')
                plt.axis('equal')
                ax = plt.gca()
                ax.set_axis_bgcolor('black')
                plt.show()
                

    def finalise(self, lendian, outname):
        for idx, sbmp in enumerate(self.lbmpFinal):
            self.lbmpFinal[idx] = self.stuff(sbmp)
            
        # stack aligned
        finalbytearr = array('B')
        for sbmp in self.lbmpFinal:
            if self.align:
                while len(sbmp) % self.align:
                    sbmp.append(0)
                    
            self.bmpaddress.append(len(finalbytearr))
            self.sizes.append(len(sbmp))
            finalbytearr.extend(sbmp)
            
        self.saveimg(finalbytearr, self.twlen, lendian, outname + '_{}bit.gray'.format(self.tdepth))
        print("Assets found:", self.fnames)
        
        
    def create_meta(self, outname, storeonflash):
        defs = \
            '#ifndef __{}_H__\n#define __{}_H__\n\n' \
            '#include "stdint.h"\n\n'.format(outname.upper(), outname.upper())
            
        declares = \
            'struct {}_info {{\n' \
            '    uint32_t address;\t// offset to asset storage on flash\n' \
            '    uint32_t length;\t// size in bytes\n' \
            '    uint16_t width;\n' \
            '    uint16_t height;\n' \
            '}};\n\n'.format(self.prefix)
            
        declares += \
            'typedef enum {{\n' \
            '    {}_NONE,\n'.format(self.prefix.upper())
        for name in self.fnames:
            declares += '    {}_{},\n'.format(self.prefix.upper(), name)
        declares += '}} {}_bmp_name;\n\n'.format(self.prefix)
        
        declares += 'uint8_t {}_get(struct {}_info *const return_ainfo, const {}_bmp_name asset);\n\n'.format( \
                    self.prefix, self.prefix, self.prefix)
        
        body = \
            '#include "{}.h"\n{}\n' \
            '{} {}_get(struct {}_info *const return_ainfo, const {}_bmp_name asset) {{\n' \
            '    switch (asset) {{\n'.format(outname, \
                                         '#include "user_interface.h"\n' if storeonflash else '', \
                                         'static uint8_t' if not storeonflash else 'uint8_t ICACHE_FLASH_ATTR', \
                                         self.prefix, self.prefix, self.prefix)
            
        for address, dimension, size, name in zip(self.bmpaddress, self.dims, self.sizes, self.fnames):
            body += \
                '    case {}_{}:\n' \
                '        *return_ainfo = (struct {}_info) {{\n' \
                '            .address = 0x{:08x},\n' \
                '            .length = {},\n' \
                '            .width = {},\n' \
                '            .height = {}\n' \
                '        }}; break;\n'.format(self.prefix.upper(), name, self.prefix, address, size, dimension[0], dimension[1])
                
        body += \
            '    default: return 1;\n' \
            '    }\n' \
            '    return 0;\n' \
            '}\n\n'
        
        
        out = open(outname + '.h', 'w')
        out.write(defs + declares + '#endif')
        out.close()
        out = open(outname + '.c', 'w')
        out.write(body)
        
    def proc(self, showBmp, showFnt, onflash, lendian):
        self.read(showBmp)
        self.finalise(lendian, 'assets')
        self.create_meta('assets', onflash)
        
        
        
class FontStack(BmpStack):
    def __init__(self, bitmaps, xmls, targetDepth, targetWordLength, byteAlign):
        super(FontStack, self).__init__(bitmaps, targetDepth, targetWordLength, byteAlign)
        self.lxml = xmls
        self.lxmlFinal = []
        self.fontmap = []
        self.fontInfo = {}
        self.charinfo = {}
        self.bytearr = array('B')
        self.prefix = 'fnt'
        
        self.cEscapes = ['\\', '\'', '"']
        
    def cEscape(self, char):
        if chr(char) in self.cEscapes:
            return '\\'
        else:
            return ''
        
    def parseInput(self):
        strippedXml = [(self.filename(x, ['.xml', '.XML']), x) for x in self.lxml]
        strippedPng = [(self.filename(p, ['.png', '.PNG']), p) for p in self.lbmp]
        strippedXml = sorted(strippedXml, key = lambda x: x[1])
        strippedPng = sorted(strippedPng, key = lambda p: p[1])
        
        for x in strippedXml:
            match = [(p, k) for p, k in strippedPng if p == x[0]][0]
            if match:
                self.fontmap.append((x[0], x[1], match[1]))
                
        if not self.fontmap:
            print('Error: Didn\'t find any matching fonts.')
            print('XMLs:', strippedXml)
            print('PNGs:', strippedPng)
            printHelp()
            sys.exit()
            
        self.lbmp = [p[2] for p in self.fontmap]
        self.lxml = [x[1] for x in self.fontmap]
            
    def extractChars(self, showChars):
        lfntpos = 0        
        fntMetricBase = {
            'maxcharlen': 0,
            'maxcharwidth': 0,
            'mincharwidth': 9999999,
            'maxcharheight': 0,
            'maxcharascent': 0
            }
        self.fontMetrics = copy.deepcopy(fntMetricBase)
        for cfont, gspixels, bmpdim in zip(self.fontmap, self.lbmpFinal, self.dims):
            self.fontInfo[cfont[0]] = {}
            doc = minidom.parse(cfont[1])            
            fdesc = doc.getElementsByTagName('description')[0]
            fmets = doc.getElementsByTagName('metrics')[0]
            ftex = doc.getElementsByTagName('texture')[0]
            self.fontInfo[cfont[0]]['desc'] = fdesc
            self.fontInfo[cfont[0]]['metrics'] = fmets
            self.fontInfo[cfont[0]]['tex'] = ftex
            self.fontInfo[cfont[0]]['name'] = fdesc.attributes['family'].value + \
                                              '_' + fdesc.attributes['style'].value + \
                                              '_' + fmets.attributes['ascender'].value
            chars = doc.getElementsByTagName('char')
            self.fontInfo[cfont[0]]['chars'] = []
            self.fontInfo[cfont[0]]['metricUnion'] = {}
            chaddresses = []
            fntMetric = copy.deepcopy(fntMetricBase)
            chInfo = []

            for char in chars:
                chx = int(char.attributes['rect'].value.split()[0])
                chy = int(char.attributes['rect'].value.split()[1])
                chw = int(char.attributes['rect'].value.split()[2])
                chh = int(char.attributes['rect'].value.split()[3])
                chadv = int(char.attributes['advance'].value)
                chxoff = int(char.attributes['offset'].value.split()[0])
                chyoff = int(char.attributes['offset'].value.split()[1])
                chInfo.append({
                    'ch': char,
                    'address': 0,
                    'kerning': []
                    })

                subimage = array('B')
                for line in range(0, chh):
                    subimage.extend(gspixels[chx + (chy + line) * bmpdim[0] : (chx + chw) + (chy + line) * bmpdim[0]])

                
                subimagearr = np.array(subimage)
                
                if showChars:
                    print(subimagearr, subimagearr.shape, chh, chw)
                    print('current ID:', char.attributes['id'].value)
                    plt.imshow(np.reshape(subimagearr, (chh, chw)), interpolation='nearest', cmap='Greys_r')
                    plt.axis('equal')
                    ax = plt.gca()
                    ax.set_axis_bgcolor('black')
                    plt.show()
            
                subimagebytearr = self.stuff(subimagearr)
                 
                # store in bytelist
                if talign:
                    while len(subimagebytearr) % talign:
                        subimagebytearr.append(0)
                chInfo[-1]['address'] = len(self.bytearr)
                self.bytearr.extend(subimagebytearr)
                chInfo[-1]['len'] = len(subimagebytearr)
                
                # local max
                if len(subimagebytearr) > fntMetric['maxcharlen']:
                    fntMetric['maxcharlen'] = len(subimagebytearr)
                if chw > fntMetric['maxcharwidth']:
                    fntMetric['maxcharwidth'] = chw
                if chadv < fntMetric['mincharwidth']:
                    fntMetric['mincharwidth'] = chadv;
                if chh > fntMetric['maxcharheight']:
                    fntMetric['maxcharheight'] = chh
                if chyoff > fntMetric['maxcharascent']:
                    fntMetric['maxcharascent'] = chyoff
                    
                # global max
                if len(subimagebytearr) > self.fontMetrics['maxcharlen']:
                    self.fontMetrics['maxcharlen'] = len(subimagebytearr)
                if chw > self.fontMetrics['maxcharwidth']:
                    self.fontMetrics['maxcharwidth'] = chw
                if chadv < self.fontMetrics['mincharwidth']:
                    self.fontMetrics['mincharwidth'] = chadv;
                if chh > self.fontMetrics['maxcharheight']:
                    self.fontMetrics['maxcharheight'] = chh
                if chyoff > self.fontMetrics['maxcharascent']:
                    self.fontMetrics['maxcharascent'] = chyoff
                    
                    
                # kerning
                kernings = char.getElementsByTagName('kerning')
                lkern = []
                if kernings != []:
                    for kerning in kernings:
                        lkern.append((ord(kerning.attributes['id'].value), kerning.attributes['advance'].value))
                    
                    lkern = sorted(lkern, key=lambda x: x[1])
                chInfo[-1]['kerning'] = lkern
            
            self.fontInfo[cfont[0]]['chars'] = list(chInfo)
            self.fontInfo[cfont[0]]['metricUnion'] = copy.deepcopy(fntMetric)
            self.fontInfo[cfont[0]]['fntlen'] = len(self.bytearr) - lfntpos
            lfntpos = len(self.bytearr)
            
        
    def create_meta(self, outname, storeonflash):
        longAddr = len(self.bytearr) > 65536
        defs = \
            '#ifndef __{}_H__\n#define __{}_H__\n\n' \
            '#include "stdint.h"\n\n'.format(outname.upper(), outname.upper())
            
        declares = \
        'struct {}_info {{\n' \
        '    uint32_t size;               \t// size in bytes\n' \
        '    uint8_t pix_depth;           \t// bits per pixel\n' \
        '    const char *name;\n' \
        '    uint8_t height;              \t// height including line spacing\n' \
        '    uint8_t ascend;              \t// usual ascent excluding accents\n' \
        '    uint16_t max_char_size_int8; \t// bytes to store biggest char (2D)\n' \
        '    uint16_t max_char_size_int32;\n' \
        '    uint16_t max_char_height;    \t// pix\n' \
        '    uint16_t max_char_width;     \t// pix\n' \
        '    uint16_t min_char_width;     \t// useful for worst case num of printable chars (min advance)\n' \
        '    uint16_t max_char_ascent;    \t// max char height with respect to base line (for cursor placement)\n' \
        '}};\n\n' \
        'struct {}_char_info {{\n' \
        '    int8_t xoffset;              \t// pixel offset to top left corner\n' \
        '    int8_t yoffset;\n' \
        '    uint8_t advance;\n' \
        '    uint{}_t address;\n' \
        '    uint16_t length;             \t// length in bytes\n' \
        '    uint8_t width;\n' \
        '    uint8_t height;\n' \
        '}};\n\n'.format(self.prefix, self.prefix, '32' if longAddr else '16')
        
        declares += \
            'typedef enum {\n'
            
        chbody = \
            'uint8_t{}\n{}_get_char(struct {}_char_info *const ch_info, const uint32_t ch, const {}_id fnt) {{\n' \
            '    switch(fnt) {{\n'.format('' if not storeonflash else ' ICACHE_FLASH_ATTR', self.prefix, self.prefix, self.prefix)
            
        kernbody = \
            'int8_t{}\n{}_get_kerning(const uint32_t ch1, const uint32_t ch2, const {}_id fnt) {{\n' \
            '    switch(fnt) {{\n'.format('' if not storeonflash else ' ICACHE_FLASH_ATTR', self.prefix, self.prefix)
            
        fntbody = \
            'uint8_t{} \n{}_get_info(struct {}_info *const finfo, const {}_id fnt) {{\n' \
            '    switch(fnt) {{\n'.format('' if not storeonflash else ' ICACHE_FLASH_ATTR', self.prefix, self.prefix, self.prefix)
                        
        for cfname, cfont in self.fontInfo.items():
            declares += '    {}_{},\n'.format(self.prefix.upper(), cfname.upper())
            
            chbody += \
                '    case {}_{}:\n' \
                '        switch(ch) {{\n'.format(self.prefix.upper(), cfname.upper())
                
            kernbody += \
                '    case {}_{}:\n' \
                '        switch(ch1) {{\n'.format(self.prefix.upper(), cfname.upper())
                
            fntbody += \
                 '    case {}_{}:\n'.format(self.prefix.upper(), cfname.upper())
                
            
            for char in cfont['chars']:
                chx = int(char['ch'].attributes['rect'].value.split()[0])
                chy = int(char['ch'].attributes['rect'].value.split()[1])
                chw = int(char['ch'].attributes['rect'].value.split()[2])
                chh = int(char['ch'].attributes['rect'].value.split()[3])
                chadv = int(char['ch'].attributes['advance'].value)
                chxoff = int(char['ch'].attributes['offset'].value.split()[0])
                chyoff = int(char['ch'].attributes['offset'].value.split()[1])
                #chid = int.from_bytes(char['ch'].attributes['id'].value.encode('utf-8'), byteorder='big')
                chid = ord(char['ch'].attributes['id'].value)
            
                chbody += \
                    '        case {}:\n' \
                    '            *ch_info = (struct {}_char_info) {{\n' \
                    '                .xoffset = {},\n' \
                    '                .yoffset = {},\n' \
                    '                .advance = {},\n' \
                    '                .width = {},\n' \
                    '                .height = {},\n' \
                    '                .address = 0x{},\n' \
                    '                .length = {}\n' \
                    '            }}; break;\n'.format( \
                         '0x{:X}'.format(chid) if chid > 127 else '\'{}{:c}\''.format(self.cEscape(chid), chid), \
                         self.prefix, \
                         chxoff, \
                         chyoff, \
                         chadv, \
                         chw, \
                         chh, \
                         '{:08x}'.format(char['address']) if longAddr else '{:04x}'.format(char['address']), \
                         char['len'])

                lkern = char['kerning']
                if lkern:
                    kernbody += \
                        '        case {}:\n' \
                        '            switch(ch2) {{\n'.format( \
                            '0x{:X}'.format(chid) if chid > 127 else '\'{}{:c}\''.format(self.cEscape(chid), chid))
                    for i in range(0, len(lkern)):
                        if i > 0 and int(lkern[i - 1][1]) != int(lkern[i][1]):
                            kernbody += '                return {};\n'.format(lkern[i - 1][1])
                        kernbody += '            case {}:\n'.format(
                            '0x{:X}'.format(lkern[i][0]) if lkern[i][0] > 127 else '\'{}{:c}\''.format( \
                                self.cEscape(lkern[i][0]), lkern[i][0]))
                    kernbody += '                return {};\n' \
                        '            default:\n' \
                        '                return 0;\n' \
                        '            }}\n'.format(lkern[-1][1])
            
            kernbody += \
                '        default:\n' \
                '            return 0;\n' \
                '        } // switch(ch1)\n'
                
            chbody += \
                '        default: return 1;\n' \
                '        } // switch(ch)\n' \
                '        break;\n' \

            
            fntbody += \
                '        *finfo = (struct {}_info) {{\n' \
                '            .size = {},\n' \
                '            .pix_depth = {},\n' \
                '            .name = \"{}\",\n' \
                '            .height = {},\n' \
                '            .ascend = {},\n' \
                '            .max_char_size_int8 = {},\n' \
                '            .max_char_size_int32 = {},\n' \
                '            .max_char_height = {},\n' \
                '            .max_char_width = {},\n' \
                '            .min_char_width = {},\n' \
                '            .max_char_ascent = {},\n' \
                '        }}; break;\n'.format(self.prefix, cfont['fntlen'], self.tdepth, cfont['name'], \
                    cfont['metrics'].attributes['height'].value, cfont['metrics'].attributes['ascender'].value, \
                    cfont['metricUnion']['maxcharlen'], math.ceil(cfont['metricUnion']['maxcharlen'] / 4), \
                    cfont['metricUnion']['maxcharheight'], cfont['metricUnion']['maxcharwidth'], \
                    cfont['metricUnion']['mincharwidth'], cfont['metricUnion']['maxcharascent'])
                                                   
        chbody += \
            '    default: return 1;\n' \
            '    } // switch(fnt)\n' \
            '    return 0;\n' \
            '}\n\n'
                
        kernbody += \
            '    default: \n' \
            '        return 0;\n' \
            '    } // switch(fnt)\n' \
            '} // (get_kerning)\n\n'
            
        fntbody += \
            '    default: return 1;\n' \
            '    } // switch(fnt)\n' \
            '    return 0;\n' \
            '}\n\n'
                
        declares += \
            '}} {}_id;\n\n'.format(self.prefix)
            
        declares += \
            '/**\n' \
            ' * @brief {}_get_char retrieve character information of ch\n' \
            ' */\n' \
            'uint8_t\n{}_get_char(struct {}_char_info *const ch_info, const uint32_t ch, const {}_id fnt);\n\n'.format( \
                self.prefix, self.prefix, self.prefix, self.prefix)
                
        declares += \
            '/**\n' \
            ' * @brief {}_get_kerning retrieve kerning information of ch2 with relation to ch1\n' \
            ' */\n' \
            'int8_t\n{}_get_kerning(const uint32_t ch1, const uint32_t ch2, const {}_id fnt);\n\n'.format( \
                self.prefix, self.prefix, self.prefix)
        
        declares += \
            '/**\n' \
            ' * @brief {}_get_info retrieve general font information\n' \
            ' */\n' \
            'uint8_t\n{}_get_info(struct {}_info *const finfo, const {}_id fnt);\n\n'.format( \
                self.prefix, self.prefix, self.prefix, self.prefix)
                
                
        body = '#include "{}.h"\n'.format(outname)        
        body += '#include "user_interface.h"\n\n' if storeonflash else '\n'
        
        defs += \
            '// global values\n' \
            '#define {}_MAX_CHAR_SIZE_INT8  \t{}\t// bytes to store biggest char (2D)\n' \
            '#define {}_MAX_CHAR_SIZE_INT32 \t{}\n' \
            '#define {}_MAX_CHAR_WIDTH      \t{}\t// pix\n' \
            '#define {}_MAX_CHAR_HEIGHT     \t{}\t// pix\n' \
            '#define {}_MIN_CHAR_WIDTH      \t{}\t// useful for worst case num of printable chars (min advance)\n' \
            '#define {}_MAX_CHAR_ASCENT     \t{}\t// max char height with respect to base line (for cursor placement)\n' \
            '\n'.format(self.prefix.upper(), self.fontMetrics['maxcharlen'], \
                self.prefix.upper(), math.ceil(self.fontMetrics['maxcharlen'] / 4), \
                self.prefix.upper(), self.fontMetrics['maxcharwidth'], \
                self.prefix.upper(), self.fontMetrics['maxcharheight'], \
                self.prefix.upper(), self.fontMetrics['mincharwidth'], \
                self.prefix.upper(), self.fontMetrics['maxcharascent'])


        out = open(outname + '.h', 'w')
        out.write(defs + declares + '#endif')
        out.close()
        out = open(outname + '.c', 'w')
        out.write(body + fntbody + chbody + kernbody)        

        print('Created {}.h, {}.c. Copy to resource/. Also do a clean make' \
            ' or gcc will not update the addresses!'.format(outname, outname))
      
    def finalise(self, lendian, outname):
        self.saveimg(self.bytearr, self.twlen, lendian, outname + '_{}bit.gray'.format(self.tdepth))
        print("Processed fonts:", self.fnames)
        
        
    def proc(self, showBmp, showFnt, onflash, lendian):
        self.parseInput()
        self.read(showBmp)
        self.extractChars(showFnt)
        self.finalise(lendian, 'fonts')
        
        self.create_meta('fonts', onflash)
            


# GLOBAL FUNCTIONS

def printHelp():
    print("Usage:", \
        "\tpython3 png2gs.py", \
        "\n mandatory:", \
        "\n\t -nf\t -> no font", \
        "\n\t <pic1.png>", \
        "\n\t <pic2.png>...", \
        "\n OR", \
        "\n mandatory:", \
        "\n\t -f\t -> font", \
        "\n\t <font1.xml>", \
        "\n\t <font1.png>", \
        "\n\t <font2.xml>", \
        "\n\t <font2.png>...", \
        "\n optional (some options will be ignored depending on choice above):", \
        "\n\t -d [int: bit depth]", \
        "\n\t -be\t -> big endian (if -d > 8)", \
        "\n\t -ic\t -> store in iRAM instead of flash", \
        "\n\t -a [int: aligned by bytes]", \
        "\n\t -p\t -> show preview")
        


# ENTRY

fxml = []
fpng = []
tdepth = 8
twlen = 32
talign = 0
lend = 1
sof = 1
font = False
preview = False     # preview of bitmaps
preview2 = False    # verbose font preview
globalMax = True    # provide max vals as #defines instead of const fontinfo

idx = 1
while idx < len(sys.argv):
    if sys.argv[idx] == '-f':
        font = True
        while (len(sys.argv) - 1 > idx and not sys.argv[idx + 1].startswith('-')):
            if sys.argv[idx + 1].endswith(('.xml', '.XML')):
                fxml.append(sys.argv[idx + 1])
            elif sys.argv[idx + 1].endswith(('.png', '.PNG')):
                fpng.append(sys.argv[idx + 1])
            idx += 1
    elif sys.argv[idx] == '-nf':
        font = False
        while (len(sys.argv) - 1 > idx and not sys.argv[idx + 1].startswith('-')):
            fpng.append(sys.argv[idx + 1])
            idx += 1
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
        lend = 0
    elif sys.argv[idx] == '-lm':
        globalMax = False
    elif sys.argv[idx] == '-ic':
        sof = 0
    elif sys.argv[idx] == '-p':
        preview = True;
    elif sys.argv[idx] == '-P':
        preview = True;
        preview2 = True;
    elif sys.argv[idx] == '-h':
        printHelp()
        sys.exit()
    idx += 1
    
if len(sys.argv) < 2:
    printHelp()
    sys.exit()
    
if not font:
    st = BmpStack(fpng, tdepth, twlen, talign)
else:
    st = FontStack(fpng, fxml, tdepth, twlen, talign)

st.proc(preview, preview2, sof, lend)


