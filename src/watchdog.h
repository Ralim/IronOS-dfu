#ifndef WATCHDOG_H_
#define WATCHDOG_H_
#include "config.h"
#include "rcc.h"
#include <stdint.h>
#define IWDG_PVU 1
#define IWDG_RVU 2

#ifdef ENABLE_WATCHDOG

#define IWDG_KR        *((volatile uint32_t *)0x40003000U)
#define IWDG_PR        *((volatile uint32_t *)0x40003004U)
#define IWDG_RLR       *((volatile uint32_t *)0x40003008U)
#define IWDG_SR        *((volatile uint32_t *)0x4000300CU)
#define IWDG_KR_RESET  0xaaaa
#define IWDG_KR_UNLOCK 0x5555
#define IWDG_KR_START  0xcccc
#define IWDG_RSTF      (1 << 29)
#define LSI_RDY        (1 << 1)
#define LSI_ON         (1 << 0)

// Enables the watchdog using a period of 1/(40Khz / 256 / 4095) = 26.2s
static void enable_iwdg(uint16_t rldval) {
  // First start LSI oscillator
  RCC_CSR |= LSI_ON;
  while (RCC_CSR & LSI_RDY) {
    asm("nop");
  }

  while (IWDG_SR & IWDG_PVU)
    ;
  IWDG_KR = IWDG_KR_UNLOCK; // Unlock PR/RLR
  IWDG_PR = 7;              // 256 prescaler

  while (IWDG_SR & IWDG_RVU)
    ;
  IWDG_KR  = IWDG_KR_UNLOCK; // Unlock PR/RLR
  IWDG_RLR = rldval;         // 4095 reload value

  // Starts the watchdog
  IWDG_KR = IWDG_KR_START;
}

static void iwdg_reset(void) { IWDG_KR = IWDG_KR_RESET; }
#endif
#endif // WATCHDOG_H_