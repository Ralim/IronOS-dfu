#include "oled.h"
#include "config.h"
#include "font.h"
#include "setup.h"
#include "watchdog.h"
#include <string.h>

uint8_t oled_init_array[] = {
    // Every line is a pair of init terms
    0x80, 0xAE, /*Display off*/
    0x80, 0xD5, /*Set display clock divide ratio / osc freq*/
    0x80, 0x52, /*Divide ratios*/
    0x80, 0xA8, /*Set Multiplex Ratio*/
    0x80, 0x0F, /*Vertical size - 1*/
#ifdef OLED_FLIP
    0x80, 0xC8, /*Set COM Scan direction backwards*/
#else
    0x80, 0xC0, /*Set COM Scan direction*/
#endif
    0x80, 0xD3, /*Set vertical Display offset*/
    0x80, 0x00, /*0 Offset*/
    0x80, 0x40, /*Set Display start line to 0*/
#ifdef OLED_FLIP
    0x80, 0xA1, /*Set Segment remap to backwards*/
#else
    0x80, 0xA0, /*Set Segment remap to normal*/
#endif
    0x80, 0x8D, /*Charge Pump*/
    0x80, 0x14, /*Charge Pump settings*/
    0x80, 0xDA, /*Set VCOM Pins hardware config*/
    0x80, 0x02, /*Combination 2*/
    0x80, 0x81, /*Brightness*/
    0x80, 0x80, /*FF == brightest, 0 == dimmest*/
    0x80, 0xD9, /*Set pre-charge period*/
    0x80, 0xF1, /*Pre charge period*/
    0x80, 0xDB, /*Adjust VCOMH regulator ouput*/
    0x80, 0x30, /*VCOM level*/
    0x80, 0xA4, /*Enable the display GDDR*/
    0x80, 0XA6, /*Normal display*/
    0x80, 0x20, /*Memory Mode*/
    0x80, 0x00, /*Wrap memory*/
    0x80, 0xAF, /*Display on*/

};

const uint8_t REFRESH_COMMANDS[] = {
    // Set display ON:
    0x80,
    0xAF, // cmd

    // Set column address:
    //  A[6:0] - Column start address = 0x20
    //  B[6:0] - Column end address = 0x7F
    0x80,            //
    0x21,            // cmd
    0x80,            // Data
    OLED_GRAM_START, // Start
    0x80,            // Data
    OLED_GRAM_END,   // End
#ifdef OLED_FLIP
    // Set COM output scan direction (reverse mode, COM[N-1] to COM0)
    0x80, 0xC8,
#else
    // Set COM output scan direction (normal mode, COM0 to COM[N-1])
    0x80, 0xC0,
#endif
    // Set page address:
    //  A[2:0] - Page start address = 0
    //  B[2:0] - Page end address = 1
    0x80,
    0x22, // cmd
    0x80,
    0x00, // A
    0x80,
    0x01, // B

};

void Data_Command(uint8_t len, const uint8_t *ptr);
void OLED_DrawChar(char c, uint8_t x, const uint8_t row);
void Set_ShowPos(uint8_t x, uint8_t y);
void oled_init(void) {
  rcc_gpio_enable(OLED_RESET_GPIO_Port);
  gpio_set_output(OLED_RESET_GPIO_Port, OLED_RESET_Pin);
  gpio_clear(OLED_RESET_GPIO_Port, OLED_RESET_Pin);
  // Delay a few ms

#ifdef ENABLE_WATCHDOG
  iwdg_reset();
#endif
  for (unsigned int yyyy = 0; yyyy < 10; yyyy++) {
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
  }

  gpio_set(OLED_RESET_GPIO_Port, OLED_RESET_Pin);
  const int len = sizeof(oled_init_array) / 2;
  for (int i = 0; i < len; i++) {
    i2c_write_reg(DEVICEADDR_OLED, oled_init_array[i * 2], oled_init_array[(i * 2) + 1]);
  }
  // init should be done now :)
}
uint8_t displayOffset = 32;
uint8_t displayBuffer[2 * 96];
uint8_t OLEDOnOffState = 0; // Used to lock out so we dont send it too often

/*
 Description: write a command to the Oled display
 Input: number of bytes to write, array to write
 Output:
 */
void Data_Command(uint8_t length, const uint8_t *data) {
  int     i;
  uint8_t tx_data[96 + 96 + 1];
  // here we are inserting the data write command at the beginning
  tx_data[0] = 0x40;
  length++;
  for (i = 1; i <= length; i++) // Loop through the array of data
  {
    if (data == 0)
      tx_data[i] = 0;
    else
      tx_data[i] = *data++;
  }
  i2c_write_bulk(DEVICEADDR_OLED, length, tx_data);
}
// This causes us to write out the buffered screen data to the display
void oled_Refresh() {
  i2c_write_bulk(DEVICEADDR_OLED, sizeof(REFRESH_COMMANDS), REFRESH_COMMANDS);
  Data_Command(96 * 2, displayBuffer);
}
/*******************************************************************************
 Function:Set_ShowPos
 Description:Set the current position in GRAM that we are drawing to
 Input:x,y co-ordinates
 *******************************************************************************/
void Set_ShowPos(uint8_t x, uint8_t y) {
  uint8_t pos_param[8] = {0x80, 0xB0, 0x80, 0x21, 0x80, 0x00, 0x80, 0x7F};
  // page 0, start add = x(below) through to 0x7F (aka 127)
  pos_param[5] = x + displayOffset; /*Display offset ==0 for Lefty, == 32 for righty*/
  pos_param[1] += y;
  i2c_write_bulk(DEVICEADDR_OLED, 8, pos_param);
}

/*******************************************************************************
 Function:oled_DrawArea
 Description:
 Inputs:(x,y) start point, (width,height) of enclosing rect, pointer to data
 Output: pointer to the last byte written out
 *******************************************************************************/
void oled_DrawArea(uint8_t x, uint8_t y, uint8_t wide, uint8_t height, const uint8_t *ptr) {
  // We want to blat the given data over the buffer
  // X is the left right position (index's through the display buffer)
  // Y is the height value (affects the bits)
  // Y is either 0 or 8, we dont do smaller bit blatting
  uint8_t lines = height / 8;
  // We draw the 1 or two stripes seperately
  for (uint8_t i = 0; i < (wide * lines); i++) {
    uint8_t xp      = x + (i % wide);
    uint8_t yoffset = i < wide ? 0 : 96;
    if (y == 8)
      yoffset = 96;
    displayBuffer[xp + yoffset] = ptr[i];
  }
}

/*******************************************************************************
 Function:Clear_Screen
 Description:Clear the entire screen to off (black)
 *******************************************************************************/
void oled_clearScreen(void) { memset(displayBuffer, 0, 96 * 2); }

/*
 * Draws a string onto the screen starting at the left
 */
void OLED_DrawString(const char *string, const uint8_t row) {
  uint8_t i = 0;
  while (string[0] != 0 && i < (96 / FONT_WIDTH)) {
    OLED_DrawChar(string[0], i, row);
    i++;
    string++;
  }
}
/*
 * Draw a char onscreen at letter index x
 */
void OLED_DrawChar(char c, uint8_t x, const uint8_t row) {
  // lowercase -> uppercase
  if (c >= 'a' && c <= 'z') {
    c -= 'a';
    c += 'A';
  }
  if (c > 'Z') {
    c = ' ';
  } else if (c < ' ') {
    c = ' ';
  }

  x *= FONT_WIDTH; // convert to a x coordinate
  uint16_t offset = 0;
  uint8_t *ptr    = (uint8_t *)FONT;
  offset          = c - ' ';

  offset *= (FONT_WIDTH * (FONT_HEIGHT / 8));
  ptr += offset;

  oled_DrawArea(x, row * 8, FONT_WIDTH, FONT_HEIGHT, (uint8_t *)ptr);
}
