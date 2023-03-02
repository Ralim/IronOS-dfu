#ifndef OLED_H_
#define OLED_H_
#include "i2c_bb.h"
#include <stdint.h>

#define DEVICEADDR_OLED (0x3c << 1)

#ifdef MODEL_S60
// TODO; for now just cropping in on the screen
#define OLED_WIDTH      96
#define OLED_HEIGHT     16
#define OLED_GRAM_START 0x20 // Should be 0x00 when we have full width
#define OLED_GRAM_END   0x7F
#else
#define OLED_WIDTH  96
#define OLED_HEIGHT 16
#ifdef OLED_FLIP
#define OLED_GRAM_START 0 // Should be 0x00 when we have full width
#define OLED_GRAM_END   95
#else
#define OLED_GRAM_START 0x20 // Should be 0x00 when we have full width
#define OLED_GRAM_END   0x7F
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
