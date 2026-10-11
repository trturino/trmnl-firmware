#include <Arduino.h>
#include <config.h>
#ifdef BOARD_SPARKFUN_SPECTRA6
#include <Wire.h>
static_assert(SCK == 12 && MOSI == 11 && SS == 10, "Unexpected SparkFun SPI pin mapping");
static_assert(SDA == 8 && SCL == 9 && Q_EN == 45, "Unexpected SparkFun I2C pin mapping");
#endif
#ifndef BOARD_TRMNL_X
extern TRMNL_DEVICE *pDevice;
#endif

void pins_init(void) {
#ifdef BOARD_SPARKFUN_SPECTRA6
  pinMode(Q_EN, OUTPUT);
  digitalWrite(Q_EN, HIGH);
  Wire.begin(SDA, SCL);
#endif
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT;
#ifdef BOARD_TRMNL_X
  io_conf.pin_bit_mask = (1ULL << PIN_INTERRUPT);
#else
  io_conf.pin_bit_mask = (1ULL << pDevice->interrupt_pin);
#endif
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&io_conf);
}
