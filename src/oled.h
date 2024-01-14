#ifndef OLED_H_
#define OLED_H_
#include "i2c_bb.h"
#include <stdint.h>

#define DEVICEADDR_OLED  0x78

// DFU use 96x16 even if on 128x32 screen
#define OLED_WIDTH       96
#define OLED_HEIGHT      16

#ifdef MODEL_S60
    #define OLED_GRAM_START  0x10
    #define OLED_GRAM_END    0x6F
    #define OLED_VCOM_LAYOUT 0x12
    #define OLED_SEGMENT_MAP 0xA1
    #define OLED_VCOM_SCAN   0xC0 // normal mode, COM0 to COM[N-1]
#elif MODEL_TS101
    #define OLED_GRAM_START  0x10
    #define OLED_GRAM_END    0x6F
    #define OLED_VCOM_LAYOUT 0x12
    #define OLED_SEGMENT_MAP 0xA0
    #define OLED_VCOM_SCAN   0xC8 // reverse mode, COM[N-1] to COM0
#elif MODEL_TS80 || MODEL_TS80P
    #define OLED_GRAM_START  0x00
    #define OLED_GRAM_END    0x5F
    #define OLED_VCOM_LAYOUT 0x02
    #define OLED_SEGMENT_MAP 0xA1
    #define OLED_VCOM_SCAN   0xC8
#else
    #define OLED_GRAM_START  0x20
    #define OLED_GRAM_END    0x7F
    #define OLED_VCOM_LAYOUT 0x02
    #define OLED_SEGMENT_MAP 0xA0
    #define OLED_VCOM_SCAN   0xC0
#endif

#define FRAMEBUFFER_START    17

// Run OLED init
void oled_init(void);
// Send out the buffer to the screen
void oled_Refresh(void);

// Draw a buffer into the oled scratch buffer
void oled_DrawArea(uint8_t x0, uint8_t y0, uint8_t wide, uint8_t height, const uint8_t *ptr);

void oled_clearScreen(void); // Clear the screen

/*Functions for writing to the screen*/
void OLED_DrawString(const char *string, const uint8_t row);

#endif
