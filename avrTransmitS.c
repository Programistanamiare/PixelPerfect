#ifdef __AVR__

#include "LED.h"

#ifndef PIX_SCALE_METHOD
#define PIX_SCALE_METHOD 0
#endif

void transmitS(volatile uint8_t* data, volatile uint16_t data_l, const gpio_pin_t *p, uint8_t bright) {
  register uint8_t color asm("r16");
  register uint8_t bit_count asm("r17");
  register uint8_t color_2sc asm("r3");
  const uint8_t hi = *p->port | (1 << p->pin), lo = *p->port & ~(1 << p->pin);
#if 0 == PIX_SCALE_METHOD
  const uint8_t scale = 0;
  asm volatile(
    // Init
    "nop \n\t"
    : [port] "+e" (p->port), [color] "+r" (color), [color_2sc] "+r" (color_2sc), [bit_count] "+r" (bit_count), [data_l] "+w" (data_l)
    : [data] "e" (data), [hi] "r" (hi), [lo] "r" (lo), [scale] "r" (scale), [bright] "r" (bright)
    : "r0", "r1", "cc"
  );
#else
#error "Unrecognised Scale Method"
#endif
}



#endif // __AVR__