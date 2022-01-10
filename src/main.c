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
#include "flash.h"
#include "i2c_bb.h"
#include "oled.h"
#include "usb.h"
#include "usb_dfu.h"
#include "watchdog.h"

// Payload/app comes inmediately after Bootloader
#define APP_ADDRESS (FLASH_BASE_ADDR + (FLASH_BOOTLDR_SIZE_KB)*1024)

#define FLASH_ACR_LATENCY 7
#define FLASH_ACR_LATENCY_2WS 0x02
#define FLASH_ACR (*(volatile uint32_t *)0x40022000U)

#define RCC_CFGR_HPRE_SYSCLK_NODIV 0x0
#define RCC_CFGR_PPRE1_HCLK_DIV2 0x4
#define RCC_CFGR_PPRE2_HCLK_NODIV 0x0
#define RCC_CFGR_ADCPRE_PCLK2_DIV8 0x3
#define RCC_CFGR_PLLMUL_PLL_CLK_MUL9 0x7
#define RCC_CFGR_PLLSRC_HSE_CLK 0x1
#define RCC_CFGR_PLLXTPRE_HSE_CLK 0x0
#define RCC_CFGR_SW_SYSCLKSEL_PLLCLK 0x2
#define RCC_CFGR_SW_SHIFT 0
#define RCC_CFGR_SW (3 << RCC_CFGR_SW_SHIFT)

#define RCC_CR_HSEON (1 << 16)
#define RCC_CR_HSERDY (1 << 17)
#define RCC_CR_PLLON (1 << 24)
#define RCC_CR_PLLRDY (1 << 25)
#define RCC_CR (*(volatile uint32_t *)0x40021000U)
#define RCC_CFGR (*(volatile uint32_t *)0x40021004U)

#define RCC_CSR (*(volatile uint32_t *)0x40021024U)
#define RCC_CSR_LPWRRSTF (1 << 31)
#define RCC_CSR_WWDGRSTF (1 << 30)
#define RCC_CSR_IWDGRSTF (1 << 29)
#define RCC_CSR_SFTRSTF (1 << 28)
#define RCC_CSR_PORRSTF (1 << 27)
#define RCC_CSR_PINRSTF (1 << 26)
#define RCC_CSR_RMVF (1 << 24)

#ifdef ENABLE_PINRST_DFU_BOOT
static inline int reset_due_to_pin() {
  return (RCC_CSR & RCC_CSR_PINRSTF) &&
         !(RCC_CSR & (RCC_CSR_LPWRRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_IWDGRSTF |
                      RCC_CSR_SFTRSTF | RCC_CSR_PORRSTF));
}
#endif

void clock_setup_in_hse_8mhz_out_72mhz(void) {
  // No need to use HSI or HSE while setting up the PLL, just use the RC osc.

  /* Enable external high-speed oscillator 8MHz. */
  RCC_CR |= RCC_CR_HSEON;
  while (!(RCC_CR & RCC_CR_HSERDY))
    ;

  /*
   * Set prescalers for AHB, ADC, ABP1, ABP2.
   * Do this before touching the PLL (TODO: why?).
   */
  uint32_t reg32 = RCC_CFGR & 0xFFC0000F;
  reg32 |= (RCC_CFGR_HPRE_SYSCLK_NODIV << 4) | (RCC_CFGR_PPRE1_HCLK_DIV2 << 8) |
           (RCC_CFGR_PPRE2_HCLK_NODIV << 11) |
           (RCC_CFGR_ADCPRE_PCLK2_DIV8 << 14) |
           (RCC_CFGR_PLLMUL_PLL_CLK_MUL9 << 18) |
           (RCC_CFGR_PLLSRC_HSE_CLK << 16) | (RCC_CFGR_PLLXTPRE_HSE_CLK << 17);
  RCC_CFGR = reg32;

  // 0WS from 0-24MHz
  // 1WS from 24-48MHz
  // 2WS from 48-72MHz
  FLASH_ACR = (FLASH_ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_2WS;

  /* Enable PLL oscillator and wait for it to stabilize. */
  RCC_CR |= RCC_CR_PLLON;
  while (!(RCC_CR & RCC_CR_PLLRDY))
    ;

  // Select PLL as SYSCLK source.
  RCC_CFGR = (RCC_CFGR & ~RCC_CFGR_SW) |
             (RCC_CFGR_SW_SYSCLKSEL_PLLCLK << RCC_CFGR_SW_SHIFT);
}

int main(void) {
  /* Boot the application if it seems valid and we haven't been
   * asked to reboot into DFU mode. This should make the CPU to
   * boot into DFU if the user app has been erased. */

  int go_dfu =
#ifdef ENABLE_PINRST_DFU_BOOT
      reset_due_to_pin() ||
#endif
#ifdef ENABLE_WATCHDOG
      reset_due_to_watchdog() ||
#endif
      force_dfu_gpio();
  RCC_CSR |= RCC_CSR_RMVF;

  if (!go_dfu &&
      (*(volatile uint32_t *)APP_ADDRESS & 0x2FFE0000) == 0x20000000) {

    // Do some simple XOR checking

#ifdef ENABLE_WATCHDOG
    // Enable the watchdog
    enable_iwdg(4096 * ENABLE_WATCHDOG / 26);
#endif
    // Set vector table base address.
    volatile uint32_t *_csb_vtor = (uint32_t *)0xE000ED08U;
    *_csb_vtor = APP_ADDRESS & 0xFFFF;
    // Initialise master stack pointer.
    __asm__ volatile("msr msp, %0" ::"g"(*(volatile uint32_t *)APP_ADDRESS));
    // Jump to application.
    (*(void (**)())(APP_ADDRESS + 4))();
  }

  clock_setup_in_hse_8mhz_out_72mhz();

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
  oled_draw_logo();
  for (unsigned int i = 0; i < 100000; i++) {
    __asm__("nop");
    __asm__("nop");
  }

  get_dev_unique_id(serial_no);
  usb_init();

  while (1) {
    // Poll based approach
    do_usb_poll();
    if (usbdfu_state == STATE_DFU_MANIFEST) {
      // USB device must detach, we just reset...
      _full_system_reset();
    }
  }
}
