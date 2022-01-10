#ifndef SETUP_H_
#define SETUP_H_
#include <stddef.h>
#include <stdint.h>
void clock_setup_in_hse_8mhz_out_72mhz(void);
void _full_system_reset(void);
int force_dfu_gpio(void);

void gpio_set_mode(uint32_t gpiodev, uint16_t gpion, uint8_t mode);
// GPIO/RCC stuff

#define RCC_APB2ENR (*(volatile uint32_t *)0x40021018U)

#define rcc_gpio_enable(gpion) RCC_APB2ENR |= (1 << (gpion + 2));

#define GPIOA 0
#define GPIOB 1
#define GPIOC 2
#define GPIOD 3
#define GPIOE 4
#define GPIOF 5

#define GPIO_CRL(x) *((volatile uint32_t *)(x * 0x400 + 0 + 0x40010800U))
#define GPIO_CRH(x) *((volatile uint32_t *)(x * 0x400 + 4 + 0x40010800U))
#define GPIO_IDR(x) *((volatile uint32_t *)(x * 0x400 + 8 + 0x40010800U))
#define GPIO_BSRR(x) *((volatile uint32_t *)(x * 0x400 + 16 + 0x40010800U))

#define gpio_set_output(a, b) gpio_set_mode(a, b, 0x2)
#define gpio_set_input(a, b) gpio_set_mode(a, b, 0x0)
#define gpio_set_input_pp(a, b) gpio_set_mode(a, b, 0x8)
#define gpio_set_output_od(a, b) gpio_set_mode(a, b, 0x01 | (0x01 << 2))
#define gpio_clear(gpiodev, gpion) GPIO_BSRR(gpiodev) = (1 << (16 + gpion))
#define gpio_set(gpiodev, gpion) GPIO_BSRR(gpiodev) = (1 << (gpion))

#define gpio_read(gpiodev, gpion) (GPIO_IDR(gpiodev) & (1 << (gpion)))

#endif // SETUP_H_