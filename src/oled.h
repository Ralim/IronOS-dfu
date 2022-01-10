#ifndef OLED_H_
#define OLED_H_
#include "i2c_bb.h"
#include <stdint.h>

#define DEVICEADDR_OLED (0x3c << 1)
#define OLED_WIDTH 96
#define OLED_HEIGHT 16
#define FRAMEBUFFER_START 17

void oled_init(void);
void oled_draw_logo(void);
#endif