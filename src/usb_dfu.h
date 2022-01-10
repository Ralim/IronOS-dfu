#ifndef USB_DFU_H_
#define USB_DFU_H_
#include "setup.h"
#include "usb.h"
#include <stddef.h>
#include <stdint.h>
uint8_t usbdfu_getstatus(uint32_t *bwPollTimeout);
extern char serial_no[25];
extern enum dfu_state usbdfu_state;
void get_dev_unique_id(char *s);

#endif // USB_DFU_H_