#ifndef OLED_H_
#define OLED_H_
#include "config.h"
#include "i2c_bb.h"
#include <stdint.h>

#define DEVICEADDR_OLED (0x3c << 1)

#if MODEL_S60 || MODEL_S60P || MODEL_S99
#define OLED_AREA_X      16
#define OLED_AREA_Y      8
#define OLED_WIDTH       128
#define OLED_HEIGHT      32
#define OLED_GRAM_START  0x00
#define OLED_GRAM_END    0x7F
#define OLED_VCOM_LAYOUT 0x12
#define OLED_SEGMENT_MAP 0xA1
#else

#define OLED_AREA_X      0
#define OLED_AREA_Y      0
#define OLED_WIDTH       96
#define OLED_HEIGHT      16
#define OLED_VCOM_LAYOUT 0x02

#ifdef OLED_FLIP_BUFFER
#define OLED_GRAM_START  0 // Aligned to top left of buffer
#define OLED_GRAM_END    95
#define OLED_SEGMENT_MAP 0xA1
#else
#define OLED_GRAM_START  0x20 // Align to bottom right of buffer
#define OLED_GRAM_END    0x7F
#define OLED_SEGMENT_MAP 0xA0
#endif

#endif

#define FRAMEBUFFER_START 17

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
