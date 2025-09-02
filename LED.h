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

#ifdef __AVR__

#include <avr/io.h>

typedef struct gpio_pin_t {
  volatile uint8_t* ddr;
  volatile uint8_t* port;
  uint8_t pin;
} gpio_pin_t;


#elif defined(ESP_PROJECT)

#endif


#ifdef __cplusplus
}
#endif

#endif