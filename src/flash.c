#include "flash.h"

void _flash_program_buffer(uint32_t address, uint16_t *data, unsigned len) {
  _flash_wait_for_last_operation();

  // Enable programming
  FLASH_CR |= FLASH_CR_PG;

  volatile uint16_t *addr_ptr = (uint16_t *)address;
  for (unsigned i = 0; i < len / 2; i++) {
    addr_ptr[i] = data[i];
    _flash_wait_for_last_operation();
  }

  // Disable programming
  FLASH_CR &= ~FLASH_CR_PG;
}

void _flash_unlock(int opt) {
  // Clear the unlock state.
  FLASH_CR |= FLASH_CR_LOCK;

  // Authorize the FPEC access.
  FLASH_KEYR = 0x45670123U;
  FLASH_KEYR = 0xcdef89abU;

  if (opt) {
    // F1 uses same keys for flash and option
    FLASH_OPTKEYR = 0x45670123U;
    FLASH_OPTKEYR = 0xcdef89abU;
  }
}
void _flash_lock() { FLASH_CR |= FLASH_CR_LOCK; }

void _flash_erase_page(uint32_t page_address) {
  _flash_wait_for_last_operation();

  FLASH_CR |= FLASH_CR_PER;
  FLASH_AR = page_address;
  FLASH_CR |= FLASH_CR_STRT;

  _flash_wait_for_last_operation();

  FLASH_CR &= ~FLASH_CR_PER;
}

int _flash_page_is_erased(uint32_t addr) {
  volatile uint32_t *_ptr32 = (uint32_t *)addr;
  for (unsigned i = 0; i < 1024 / sizeof(uint32_t); i++)
    if (_ptr32[i] != 0xffffffffU)
      return 0;
  return 1;
}