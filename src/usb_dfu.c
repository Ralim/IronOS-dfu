#include "usb_dfu.h"
#include "config.h"
#include "flash.h"
#include "setup.h"
#include "usb.h"
#include <string.h>

enum usbd_request_return_codes usbdfu_control_request(struct usb_setup_data *req, uint16_t *len, void (**complete)(struct usb_setup_data *req));

// USB control data buffer
uint8_t usbd_control_buffer[DFU_TRANSFER_SIZE];

// DFU state
enum dfu_state usbdfu_state = STATE_DFU_IDLE;
static struct {
  uint8_t  buf[sizeof(usbd_control_buffer)];
  uint16_t len;
  uint32_t addr;
  uint16_t blocknum;
} prog;

// Serial number to expose via USB
char serial_no[25];

#define STR_HELPER(x) #x
#define STR(x)        STR_HELPER(x)
const char *const _usb_strings[5] = {"RalimTek <3 libopencm3",       // iManufacturer
                                     "DFU bootloader [" VERSION "]", // iProduct
                                     serial_no,                      // iSerialNumber
                                     // Interface desc string
                                     /* This string is used by ST Microelectronics' DfuSe utility. */
                                     /* Change check_do_erase() accordingly */
                                     "@Internal Flash /0x08000000/" STR(FLASH_BOOTLDR_SIZE_KB) "*001Ka," STR(FLASH_BOOTLDR_PAYLOAD_SIZE_KB) "*001Kg",
                                     // Config desc string
                                     "Bootloader config: "};

static const char hcharset[16] = "0123456789abcdef";
void              get_dev_unique_id(char *s) {
  volatile uint8_t *unique_id = (volatile uint8_t *)0x1FFFF7E8;
  /* Fetch serial number from chip's unique ID */
  for (int i = 0; i < 24; i += 2) {
    s[i]     = hcharset[(*unique_id >> 4) & 0xF];
    s[i + 1] = hcharset[*unique_id++ & 0xF];
  }
}

uint8_t usbdfu_getstatus(uint32_t *bwPollTimeout) {
  switch (usbdfu_state) {
  case STATE_DFU_DNLOAD_SYNC:
    usbdfu_state   = STATE_DFU_DNBUSY;
    *bwPollTimeout = 100;
    return DFU_STATUS_OK;
  case STATE_DFU_MANIFEST_SYNC:
    // Device will reset when read is complete.
    usbdfu_state = STATE_DFU_MANIFEST;
    return DFU_STATUS_OK;
  case STATE_DFU_ERROR:
    return STATE_DFU_ERROR;
  default:
    return DFU_STATUS_OK;
  }
}

static void usbdfu_getstatus_complete(struct usb_setup_data *req) {
  (void)req;

  // Protect the flash by only writing to the valid flash area
  const uint32_t start_addr = DFU_VALID_FLASH_START;
  const uint32_t end_addr   = DFU_VALID_FLASH_END;

  switch (usbdfu_state) {
  case STATE_DFU_DNBUSY:
    _flash_unlock(0);
    if (prog.blocknum == 0) {
      switch (prog.buf[0]) {
      case CMD_ERASE: {

        // Clear this page here.
        uint32_t baseaddr = *(uint32_t *)(prog.buf + 1);
        if (baseaddr >= start_addr && baseaddr + DFU_TRANSFER_SIZE <= end_addr) {
          if (!_flash_page_is_erased(baseaddr))
            _flash_erase_page(baseaddr);
        }
      } break;
      case CMD_SETADDR:
        // Assuming little endian here.
        prog.addr = *(uint32_t *)(prog.buf + 1);
        break;
      }
    } else {

      // From formula Address_Pointer + ((wBlockNum - 2)*wTransferSize)
      uint32_t baseaddr = prog.addr + ((prog.blocknum - 2) * DFU_TRANSFER_SIZE);

      if (baseaddr >= start_addr && baseaddr + prog.len <= end_addr) {
        // Program buffer in one go after erasing.
        if (!_flash_page_is_erased(baseaddr))
          _flash_erase_page(baseaddr);
        _flash_program_buffer(baseaddr, (uint16_t *)prog.buf, prog.len);
      }
    }
    _flash_lock();

    /* Jump straight to dfuDNLOAD-IDLE, skipping dfuDNLOAD-SYNC. */
    usbdfu_state = STATE_DFU_DNLOAD_IDLE;
    return;
  case STATE_DFU_MANIFEST:
    return; // Reset placed in main loop.
  default:
    return;
  }
}

enum usbd_request_return_codes usbdfu_control_request(struct usb_setup_data *req, uint16_t *len, void (**complete)(struct usb_setup_data *req)) {
  switch (req->bRequest) {
  case DFU_DNLOAD:
    if ((len == NULL) || (*len == 0)) {
      // wLength = 0 means leave DFU
      usbdfu_state = STATE_DFU_MANIFEST_SYNC;
      return USBD_REQ_HANDLED;
    } else {
      /* Copy download data for use on GET_STATUS. */
      prog.blocknum = req->wValue;
      // Beware overflows!
      prog.len = *len;
      if (prog.len > sizeof(prog.buf))
        prog.len = sizeof(prog.buf);
      memcpy(prog.buf, usbd_control_buffer, prog.len);
      usbdfu_state = STATE_DFU_DNLOAD_SYNC;
      return USBD_REQ_HANDLED;
    }
  case DFU_CLRSTATUS:
    // Just clears errors.
    if (usbdfu_state == STATE_DFU_ERROR)
      usbdfu_state = STATE_DFU_IDLE;
    return USBD_REQ_HANDLED;
  case DFU_ABORT:
    // Abort just returns to IDLE state.
    usbdfu_state = STATE_DFU_IDLE;
    return USBD_REQ_HANDLED;
  case DFU_DETACH:
    usbdfu_state = STATE_DFU_MANIFEST;
    return USBD_REQ_HANDLED;
  case DFU_UPLOAD:
    // Send data back to host by reading the image.
    usbdfu_state = STATE_DFU_UPLOAD_IDLE;
    if (!req->wValue) {
      // Send back supported commands.
      usbd_control_buffer[0] = 0x00;
      usbd_control_buffer[1] = CMD_SETADDR;
      usbd_control_buffer[2] = CMD_ERASE;
      *len                   = 3;
      return USBD_REQ_HANDLED;
    } else {
// Send back data if only if we enabled that.
#ifndef ENABLE_DFU_UPLOAD
      usbdfu_state = STATE_DFU_ERROR;
      *len         = 0;
#else
      // Protect the flash by only writing to the valid flash area
      const uint32_t start_addr = DFU_VALID_FLASH_START;
      const uint32_t end_addr   = DFU_VALID_FLASH_END;

      // From formula Address_Pointer + ((wBlockNum - 2)*wTransferSize)
      uint32_t baseaddr = prog.addr + ((req->wValue - 2) * DFU_TRANSFER_SIZE);
      if (baseaddr >= start_addr && baseaddr + DFU_TRANSFER_SIZE <= end_addr) {
        memcpy(usbd_control_buffer, (void *)baseaddr, DFU_TRANSFER_SIZE);
        *len = DFU_TRANSFER_SIZE;
      } else {
        usbdfu_state = STATE_DFU_ERROR;
        *len         = 0;
      }
#endif
    }
    return USBD_REQ_HANDLED;
  case DFU_GETSTATUS: {
    // Perfom the action and register complete callback.
    uint32_t bwPollTimeout = 0; /* 24-bit integer in DFU class spec */
    usbd_control_buffer[0] = usbdfu_getstatus(&bwPollTimeout);
    usbd_control_buffer[1] = bwPollTimeout & 0xFF;
    usbd_control_buffer[2] = (bwPollTimeout >> 8) & 0xFF;
    usbd_control_buffer[3] = (bwPollTimeout >> 16) & 0xFF;
    usbd_control_buffer[4] = usbdfu_state;
    usbd_control_buffer[5] = 0; /* iString not used here */
    *len                   = 6;
    *complete              = usbdfu_getstatus_complete;
    return USBD_REQ_HANDLED;
  }
  case DFU_GETSTATE:
    // Return state with no state transision.
    usbd_control_buffer[0] = usbdfu_state;
    *len                   = 1;
    return USBD_REQ_HANDLED;
  }

  return USBD_REQ_NEXT_CALLBACK;
}
