#include "i2c_bb.h"
#include "setup.h"
// A minimal i2c bit-banging code implementation
// So that we can push info to the lcd
#define SCL_Pin       6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin       7
#define SDA_GPIO_Port GPIOB

#define SOFT_SCL_HIGH() gpio_set(SCL_GPIO_Port, SCL_Pin)
#define SOFT_SCL_LOW()  gpio_clear(SCL_GPIO_Port, SCL_Pin)
#define SOFT_SDA_HIGH() gpio_set(SDA_GPIO_Port, SDA_Pin)
#define SOFT_SDA_LOW()  gpio_clear(SDA_GPIO_Port, SDA_Pin)

#define SOFT_SDA_READ() (gpio_read(SDA_GPIO_Port, SDA_Pin) ? 1 : 0)
#define SOFT_SCL_READ() (gpio_read(SCL_GPIO_Port, SCL_Pin) ? 1 : 0)
#define SOFT_I2C_DELAY()              \
  {                                   \
    for (int xx = 0; xx < 40; xx++) { \
      asm("nop");                     \
    }                                 \
  }

void    i2c_start(void);
void    i2c_stop(void);
uint8_t i2c_read(uint8_t ack);
void    i2c_write_bit(uint8_t val);
uint8_t i2c_send(uint8_t value);
uint8_t i2c_read_bit(void);

void i2c_init(void) {
  // Init the GPIO as output, open drain
  gpio_set_output_od(SCL_GPIO_Port, SCL_Pin);
  gpio_set_output_od(SDA_GPIO_Port, SDA_Pin);
  SOFT_SCL_HIGH();
  SOFT_SDA_HIGH();
}

void i2c_start(void) {
  /* I2C Start condition, data line goes low when clock is high */
  SOFT_SCL_HIGH();
  SOFT_SDA_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SDA_LOW();
  SOFT_I2C_DELAY();
  SOFT_SCL_LOW();
  SOFT_I2C_DELAY();
  SOFT_SDA_HIGH();
}

void i2c_stop(void) {
  /* I2C Stop condition, clock goes high when data is low */
  SOFT_SDA_LOW();
  SOFT_I2C_DELAY();
  SOFT_SCL_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SDA_HIGH();
  SOFT_I2C_DELAY();
}

void i2c_write_bit(uint8_t val) {
  if (val) {
    SOFT_SDA_HIGH();
  } else {
    SOFT_SDA_LOW();
  }

  SOFT_I2C_DELAY();
  SOFT_SCL_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SCL_LOW();
}
uint8_t i2c_send(uint8_t value) {

  for (uint8_t i = 0; i < 8; i++) {
    i2c_write_bit(value & 0x80); // write the most-significant bit
    value <<= 1;
  }

  SOFT_SDA_HIGH();
  uint8_t ack = (i2c_read_bit() == 0);
  return ack;
}

uint8_t i2c_read(uint8_t ack) {
  uint8_t B = 0;

  uint8_t i;
  for (i = 0; i < 8; i++) {
    B <<= 1;
    B |= i2c_read_bit();
  }

  SOFT_SDA_HIGH();
  if (ack)
    i2c_write_bit(0);
  else
    i2c_write_bit(1);
  return B;
}

uint8_t i2c_read_bit(void) {
  uint8_t b;

  SOFT_SDA_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SCL_HIGH();
  SOFT_I2C_DELAY();

  if (SOFT_SDA_READ())
    b = 1;
  else
    b = 0;

  SOFT_SCL_LOW();
  return b;
}

void i2c_write_reg(const uint8_t address, const uint8_t reg, const uint8_t value) {
  // Write out to device, so send start,device addr, reg, value, stop
  i2c_start();
  i2c_send(address);
  i2c_send(reg);
  i2c_send(value);
  i2c_stop();
}

void i2c_write_bulk(const uint8_t address, const uint16_t length, const uint8_t *data) {
  i2c_start();
  i2c_send(address);
  for (int i = 0; i < length; i++) {
    i2c_send(data[i]);
  }
  i2c_stop();
}