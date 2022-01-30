#include "setup.h"

#include "config.h"
void _full_system_reset() {
  // Reset and wait for it!
  volatile uint32_t *_scb_aircr = (uint32_t *)0xE000ED0CU;
  *_scb_aircr                   = 0x05FA0000 | 0x4;
  while (1)
    ;
  __builtin_unreachable();
}

#ifdef ENABLE_GPIO_DFU_BOOT
int force_dfu_gpio(void) {
  rcc_gpio_enable(GPIO_DFU_BOOT_PORT);
  gpio_set_input_pp(GPIO_DFU_BOOT_PORT, GPIO_DFU_BOOT_PIN);
  gpio_set(GPIO_DFU_BOOT_PORT, GPIO_DFU_BOOT_PIN);
  for (unsigned int i = 0; i < 512; i++)
    __asm__("nop");
  uint16_t val = gpio_read(GPIO_DFU_BOOT_PORT, GPIO_DFU_BOOT_PIN);
  gpio_set_input(GPIO_DFU_BOOT_PORT, GPIO_DFU_BOOT_PIN);
  return val == 0;
}
#else
int force_dfu_gpio(void) { return 1; }
#endif

void gpio_set_mode(uint32_t gpiodev, uint16_t gpion, uint8_t mode) {
  if (gpion < 8)
    GPIO_CRL(gpiodev) = (GPIO_CRL(gpiodev) & ~(0xf << ((gpion) << 2))) | (mode << ((gpion) << 2));
  else
    GPIO_CRH(gpiodev) = (GPIO_CRL(gpiodev) & ~(0xf << ((gpion - 8) << 2))) | (mode << ((gpion - 8) << 2));
}
