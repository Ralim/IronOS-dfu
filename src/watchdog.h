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
void enable_iwdg(void);
void iwdg_reset(void);
#endif
#endif // WATCHDOG_H_
