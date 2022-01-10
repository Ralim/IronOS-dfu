#ifndef I2C_BB_H_
#define I2C_BB_H_
#include <stdint.h>
void i2c_init(void);
void i2c_write_reg(const uint8_t address, const uint8_t reg,
                   const uint8_t value);
void i2c_write_bulk(const uint8_t address, const uint16_t length,
                    const uint8_t *data);
#endif