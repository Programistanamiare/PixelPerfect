#include "LED.h"

#ifndef NULL
#define NULL (void*)0
#endif

void pix_LED_set32bit(pix_LED *led, uint32_t color) {
  if (NULL == led) return;
  led->r = (color >> 16) & 0xff;
  led->g = (color >> 8) & 0xff;
  led->b = color & 0xff;
}

#ifdef __AVR__

#include <avr/io.h>

static int gpio_pin_valid(const gpio_pin_t *p) {
  if (NULL == p) return -1;
  if (NULL == p->ddr) return -2;
  if (NULL == p->port) return 3;
  if (p->port > 7) return 4;
  return 0;
}

void pix_pinConfig(const gpio_pin_t *p) {
  if (0 != gpio_pin_valid(p)) return;
  *p->ddr |= (1 << p->pin);
  *p->port &= ~(1 << p->pin);
}

#elif defined(ESP_PROJECT)

void pix_pinConfig(const gpio_pin_t *p) {
  gpio_config_t io_conf = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = (1ULL << p->gpio_num),
    .pull_down_en = 0,
    .pull_up_en = 0
  };

  gpio_config(&io_conf);

  gpio_set_level(p->gpio_num, 0);
}

#endif // ESP_PROJECT

#include <string.h>

void pix_fillUp8bit(pix_LED *leds, uint16_t len, uint8_t color_byte) {
  if (NULL == leds) return;
  memset(leds, color_byte, len * sizeof(pix_LED));
}

void pix_fillUp3x8bit(pix_LED *leds, uint16_t len, uint8_t r, uint8_t g, uint8_t b) {
  if (NULL == leds) return;
  for (uint16_t i = 0; i < len; ++i) {
    leds[i].r = r, leds[i].g = g, leds[i].b = b;
  }
}

void pix_fillUp32bit(pix_LED *leds, uint16_t len, uint32_t color) {
  return pix_fillUp3x8bit(leds, len, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
}

void pix_fillUpColor(pix_LED *leds, uint16_t len, pix_Color color) {
  return pix_fillUp3x8bit(leds, len, color.r, color.g, color.b);
}

void pix_fillFromTo8bit(pix_LED *leds, uint16_t len, uint8_t color_byte, uint16_t from, uint16_t to) {
  if (NULL == leds) return;
  if (from > to) return;
  if (to >= len) return;
  memset(leds + from * sizeof(pix_LED), color_byte, (to - from + 1) * sizeof(pix_LED));
}

void pix_fillFromTo3x8bit(pix_LED *leds, uint16_t len, uint8_t r, uint8_t g, uint8_t b, uint16_t from, uint16_t to) {
  if (NULL == leds) return;
  if (from > to) return;
  if (to >= len) return;
  for (; from <= to; ++from) {
    leds[from].r = r;
    leds[from].g = g;
    leds[from].b = b;
  }
}

void pix_fillFromTo32bit(pix_LED *leds, uint16_t len, uint32_t color, uint16_t from, uint16_t to) {
  return pix_fillFromTo3x8bit(leds, len, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff, from, to);
}

void pix_fillFromToColor(pix_LED *leds, uint16_t len, pix_Color color, uint16_t from, uint16_t to) {
  return pix_fillFromTo3x8bit(leds, len, color.r, color.g, color.b, from, to);
}

