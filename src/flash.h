#ifndef FLASH_H_
#define FLASH_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "config.h"
#include <stddef.h>
#include <stdint.h>
// Flashing routines //

#define FLASH_CR_LOCK (1 << 7)
#define FLASH_CR_STRT (1 << 6)
#define FLASH_CR_OPTER (1 << 5)
#define FLASH_CR_OPTPG (1 << 4)
#define FLASH_CR_PER (1 << 1)
#define FLASH_CR_PG (1 << 0)
#define FLASH_SR_BSY (1 << 0)
#define FLASH_KEYR (*(volatile uint32_t *)0x40022004U)
#define FLASH_OPTKEYR (*(volatile uint32_t *)0x40022008U)
#define FLASH_SR (*(volatile uint32_t *)0x4002200CU)
#define FLASH_CR (*(volatile uint32_t *)0x40022010U)
#define FLASH_AR (*(volatile uint32_t *)0x40022014U)
void _flash_lock(void);
void _flash_unlock(int opt);

#define _flash_wait_for_last_operation()                                       \
  while (FLASH_SR & FLASH_SR_BSY)                                              \
    ;

void _flash_erase_page(uint32_t page_address);

int _flash_page_is_erased(uint32_t addr);

void _flash_program_buffer(uint32_t address, uint16_t *data, unsigned len);

#ifdef __cplusplus
}
#endif
#endif // FLASH_H_