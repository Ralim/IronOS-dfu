#include "watchdog.h"
#ifdef ENABLE_WATCHDOG
// Enables the watchdog using a period of 1/(40Khz / 256 / 4095) = 26.2s
void enable_iwdg(void) {
  if (IWDG_PR != 0) {
    return;
  }
  // First start LSI oscillator
  RCC_CSR |= LSI_ON;
  while (RCC_CSR & LSI_RDY) {}
  while (IWDG_SR & IWDG_PVU) {}

  IWDG_KR = IWDG_KR_UNLOCK; // Unlock PR/RLR
  IWDG_PR = 6;              // 256 prescaler

  while (IWDG_SR & IWDG_RVU) {}
  IWDG_KR  = IWDG_KR_UNLOCK; // Unlock PR/RLR
  IWDG_RLR = 0xFFF;          // 4095 reload value

  // Starts the watchdog
  IWDG_KR = IWDG_KR_START;
}
void iwdg_reset(void) { IWDG_KR = IWDG_KR_RESET; }
#endif
