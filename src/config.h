#ifndef CONFIG_H_
#define CONFIG_H_
#define FLASH_BASE_ADDR 0x08000000
#define FLASH_SIZE_KB (128)
#define FLASH_BOOTLDR_SIZE_KB (4)
#define FLASH_BOOTLDR_PAYLOAD_SIZE_KB (FLASH_SIZE_KB - FLASH_BOOTLDR_SIZE_KB)

// #define ENABLE_WATCHDOG 20 // If watchdog should be started before the app
// starts
#define ENABLE_GPIO_DFU_BOOT
#define GPIO_DFU_BOOT_PORT GPIOB
#define GPIO_DFU_BOOT_PIN 2

/* Commands sent with wBlockNum == 0 as per ST implementation. */
#define CMD_SETADDR 0x21
#define CMD_ERASE 0x41
#endif