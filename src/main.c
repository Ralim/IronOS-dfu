/*
 * Copyright (C) 2018 David Guillen Fandos <david@davidgf.net>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "display.h"
#include "flash.h"
#include "i2c_bb.h"
#include "oled.h"
#include "rcc.h"
#include "usb.h"
#include "usb_dfu.h"
#include "watchdog.h"

// Payload/app comes inmediately after Bootloader
#define APP_ADDRESS (FLASH_BASE_ADDR + (FLASH_BOOTLDR_SIZE_KB)*1024)

#ifdef ENABLE_PINRST_DFU_BOOT
static inline int reset_due_to_pin() { return (RCC_CSR & RCC_CSR_PINRSTF) && !(RCC_CSR & (RCC_CSR_LPWRRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_SFTRSTF | RCC_CSR_PORRSTF)); }
#endif

int main(void) {
  /* Boot the application if it seems valid and we haven't been
   * asked to reboot into DFU mode. This should make the CPU to
   * boot into DFU if the user app has been erased. */

  // Setup vector table to use out offset whatever it is
  volatile uint32_t *_csb_vtor = (uint32_t *)0xE000ED08U;
  *_csb_vtor                   = FLASH_BASE | VECTOR_TABLE_OFFSET;

#ifdef ENABLE_WATCHDOG
  // Enable the watchdog
  enable_iwdg(4096 * ENABLE_WATCHDOG / 26);
#endif

  int go_dfu = force_dfu_gpio();
  RCC_CSR |= RCC_CSR_RMVF;
  // If not requested into DFU via gpio + flash is programmed
  if (!go_dfu && (*(volatile uint32_t *)APP_ADDRESS) != 0xFFFFFFFF) {
#ifdef ENABLE_WATCHDOG
    iwdg_reset();
#endif

    // Set vector table base address.

    *_csb_vtor = APP_ADDRESS & 0xFFFF;
    // Initialise master stack pointer.
    __asm__ volatile("msr msp, %0" ::"g"(*(volatile uint32_t *)APP_ADDRESS));
    // Jump to application.
    (*(void (**)())(APP_ADDRESS + 4))();
  }

  rcc_clock_setup_in_hsi_out_48mhz();

  /* Disable USB peripheral as it overrides GPIO settings */
  *USB_CNTR_REG = USB_CNTR_PWDN;

  /*
   * Vile hack to reenumerate, physically _drag_ d+ low.
   * (need at least 2.5us to trigger usb disconnect)
   */
  rcc_gpio_enable(GPIOA);
  gpio_set_output(GPIOA, 12);
  gpio_clear(GPIOA, 12);
  // We are using the I2C init and write to cover for the delay we need for the
  // USB de-enumeration
  i2c_init();
  oled_init();
  display_splash();
  for (unsigned int x10ms = 0; x10ms < 150; x10ms++) {
#ifdef ENABLE_WATCHDOG
    iwdg_reset();
#endif
    for (unsigned int i = 0; i < 100000; i++) {
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
    }
  }
  get_dev_unique_id(serial_no);
  usb_init();
  display_show_version();
  while (1) {
    // Poll based approach
#ifdef ENABLE_WATCHDOG
    iwdg_reset();
#endif
    do_usb_poll();
    if (usbdfu_state == STATE_DFU_MANIFEST) {
      // USB device must detach, we just reset...
      _full_system_reset();
    }
  }
}
