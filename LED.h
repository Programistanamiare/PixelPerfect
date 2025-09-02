#ifndef _PIX_LED_H
#define _PIX_LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct __attribute__((packed)) pix_LED {
  uint8_t g;
  uint8_t r;
  uint8_t b;
} pix_LED;

typedef pix_LED pix_Color;

void pix_LED_set32bit(pix_LED *led, uint32_t color);

#ifdef __AVR__

typedef struct gpio_pin_t {
  volatile uint8_t* ddr;
  volatile uint8_t* port;
  uint8_t pin;
} gpio_pin_t;


#elif defined(ESP_PROJECT)

#include <driver/gpio.h>

typedef struct gpio_pin_t {
  gpio_num_t gpio_num;
} gpio_pin_t;

#endif

void pix_pinConfig(const gpio_pin_t *pin);

void pix_clear(pix_LED *leds, uint16_t len);

void pix_fillUp8bit(pix_LED *leds, uint16_t len, uint8_t color_byte);

void pix_fillUp3x8bit(pix_LED *leds, uint16_t len, uint8_t r, uint8_t g, uint8_t b);

void pix_fillUp32bit(pix_LED *leds, uint16_t len, uint32_t color);

void pix_fillUpColor(pix_LED *leds, uint16_t len, pix_Color color);

void pix_fillFromTo8bit(pix_LED *leds, uint16_t len, uint8_t color_byte, uint16_t from, uint16_t to);

void pix_fillFromTo3x8bit(pix_LED *leds, uint16_t len, uint8_t r, uint8_t g, uint8_t b, uint16_t from, uint16_t to);

void pix_fillFromTo32bit(pix_LED *leds, uint16_t len, uint32_t color, uint16_t from, uint16_t to);

void pix_fillFromToColor(pix_LED *leds, uint16_t len, pix_Color color, uint16_t from, uint16_t to);

void pix_externTimerShowScale(pix_LED *leds, uint16_t len, const gpio_pin_t *pin, uint8_t bright, uint32_t *timer);

void pix_externTimerShow(pix_LED *leds, uint16_t len, const gpio_pin_t *pin, uint32_t *timer);

void pix_showScale(pix_LED *leds, uint16_t len, const gpio_pin_t *pin, uint8_t bright);

void pix_show(pix_LED *leds, uint16_t len, const gpio_pin_t *pin);

#ifdef __cplusplus
}
#endif

#endif