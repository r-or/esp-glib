esp-glib
======

WIP 2D graphics / FB library for ESP8266 with NONOS SDK.

Included are right now:
* Bitmap-to-4bit greyscale converter (see tools/)
* Font-renderer with 
  * textbox support
  * line-break on words
  * backspace support
  * ...(https://en.wikipedia.org/wiki/Typeface#Font_metrics)
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
glib_set_mode(GLIB_DM_FREE);        // set draw mode to free mode
glib_print("ESP8266 here!",         // latin1-encoded, zero-terminated uint8_t array
           0,                       // x-coordinate (origin: top left)
           _FONT_MAX_CHAR_ASCENT_,  // y-coordinate (char origin: baseline *)
           (glib_draw_args)(GLIB_DA_CENTER_Y | GLIB_DA_CENTER_X),   
                                    // draw arguments: center horizontically & vertically
           NULL,                    // returns coordinates of end of string
           NULL);

```
\* see https://en.wikipedia.org/wiki/Typeface#Font_metrics

Fonts and assets (bitmaps) can be created with a bitmap font converter such as [andryblack's](https://github.com/andryblack/fontbuilder) fontbuilder and converted to greyscale via my tool 'png2gs.py' in tools/.

TODO: sample project using this library/driver
