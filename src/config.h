#ifndef CONFIG_H_
#define CONFIG_H_
#define FLASH_BASE_ADDR 0x08000000
#define FLASH_SIZE_KB 128
#define FLASH_BOOTLDR_SIZE_KB 16
#define FLASH_BOOTLDR_PAYLOAD_SIZE_KB 112

// #define ENABLE_WATCHDOG 20 // If watchdog should be started before the app
// starts
#define ENABLE_GPIO_DFU_BOOT
#define GPIO_DFU_BOOT_PORT GPIOB
#define GPIO_DFU_BOOT_PIN 2

#ifdef RUNTIME_MODE
#define VECTOR_OFFSET (FLASH_BOOTLDR_SIZE_KB * 1024)
#define SHOW_HASH
#define HASH_REGION_START FLASH_BASE_ADDR
#define HASH_REGION_END (FLASH_BASE_ADDR + (1024 * FLASH_BOOTLDR_SIZE_KB))
#else
#define VECTOR_OFFSET 0

#endif

/* Commands sent with wBlockNum == 0 as per ST implementation. */
#define CMD_SETADDR 0x21
#define CMD_ERASE 0x41
#endif