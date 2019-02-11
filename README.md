esp-glib
======

WIP 2D graphics / FB library for ESP8266 with NONOS SDK.

Included are right now:
* Bitmap-to-4bit greyscale converter (see tools/)
* Font-renderer with 
  * textbox support
  * line-break on words
  * multiple fonts / sizes
  * backspace support
  * UTF-8 support (escape with 'u+xxxx' or 'U+xxxxxxxx')...
  
* Some simple animations
* Some primitives
* driver for SSD1322. SPI is done with [MetalPhreak's](https://github.com/MetalPhreak/ESP8266_SPI_Driver) SPI  driver.

Usage:
```c
#include "esp-glib.h"
// user_init:
glib_init();

// example: print to a screen
glib_clear_fb(GLIB_OS_ALL);         // clear everything in framebuffer
glib_set_textbox(NULL);             // set current textbox to whole screen
glib_clear_tb_txt_state();          // clear current text positioning information
glib_set_font(A_FONT_YOU_CONVERTED);// select font (as well as font size)
glib_set_mode(GLIB_DM_FREE);        // set draw mode to free mode
glib_print("ESP8266 here!",         // zero-terminated uint8_t array; utf8 escape: u+xxxx
           0,                       // x-coordinate (origin: top left)
           0,                       // y-coordinate (char origin: baseline *)
           GLIB_TP_TOPMOST | GLIB_TP_CENTER_X, /* glib_txt_position: 
                                       auto handle y coordinate & center horizontically*/
           GLIB_DA_NONE,            /* glib_draw_args: mainly for drawing bitmaps
                                       (e.g. flip, invert, switch endianness...) */
           NULL,                    
           NULL);                   // returns coordinates of end of string
           
glib_fb2gram();                     // push framebuffer to display graphics RAM (slow)

```
\* see https://en.wikipedia.org/wiki/Typeface#Font_metrics

Fonts and assets (bitmaps) can be created with a bitmap font converter such as [andryblack's](https://github.com/andryblack/fontbuilder) fontbuilder* and converted to greyscale via 'png2gs.py' in tools/.

\* I added the option to specify the freetype hinting algorithm ("Smoothing" box). Try "LCD horizontal" or "LCD vertical"; both look distinctively better than the standard option on a small LCD screen like the one we're using.

[Sample project using this library/driver](https://github.com/r-or/esp-glib-example)

png2gs
======
* Install pypng: ```pip3 install pypng```
* To convert assets/bitmaps: ```./png2gs -nf resources/myassets/* -a 4 -d 4```
* To convert fonts: ```./png2gs -f resources/myfonts/* -a 4 -d 4```,

where the ```-d``` switch selects target bit depth (16 shades of grey in this example for SSD1322) and ```-a``` can be used to align data by that many bytes (for ESP8266 data in flash must be 4 byte aligned).

For additional options call png2gs without arguments.

Take a look at the headers created for means of accessing the data once written to flash.
