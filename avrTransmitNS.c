#ifdef __AVR__

#include "LED.h"

void transmitNS(volatile uint8_t *data, volatile uint16_t data_l, const gpio_pin_t *p) {
  register uint8_t color asm("r16");
  register uint8_t bit_count asm("r17");
  const uint8_t hi = *p->port | (1 << p->pin), lo = *p->port & ~(1 << p->pin);

#if (F_CPU >= 14000000) && (F_CPU <= 16000000)
  asm volatile(
    // Init
    "ldi %[bit_count], 0x08 \n\t"
    "ld %[color], %a[data]+ \n\t"
    "lsl %[color] \n\t"
    "brcc 1f \n\t"
    "rjmp 4f \n\t"

    // Last bit 1 [ 13H | 8L ]
    "0: \n\t"
    "st %a[port], %[hi] \n\t"           // 2 prev -> set to hi
    "ldi %[bit_count], 0x08 \n\t"       // 1
    "nop \n\t"                          // 2
    "nop \n\t"                          // 3
    "nop \n\t"                          // 4
    "nop \n\t"                          // 5
    "nop \n\t"                          // 6
    "nop \n\t"                          // 7
    "sbiw %[data_l], 0x01 \n\t"         // 9
    "breq 2f \n\t"                      // 10 | 11 if jump
    "nop \n\t"                          // 11
    "st %a[port], %[lo] \n\t"           // 13 -> set to lo
    "ld %[color], %a[data]+ \n\t"       // 2
    "lsl %[color] \n\t"                 // 3
    "brcc 1f \n\t"                      // 4 | 5 if jump
    "rjmp 4f \n\t"                      // 6

    // State 0 [ 7H | 13L ]
    "1: \n\t"
    "nop \n\t"                          // 1 prev
    "st %a[port], %[hi] \n\t"           // 3 prev -> set to hi
    "nop \n\t"                          // 1
    "nop \n\t"                          // 2
    "nop \n\t"                          // 3
    "nop \n\t"                          // 4
    "nop \n\t"                          // 5
    "st %a[port], %[lo] \n\t"           // 7 -> set to lo
    "nop \n\t"                          // 1
    "nop \n\t"                          // 2
    "nop \n\t"                          // 3
    "dec %[bit_count] \n\t"             // 4
    "cpi %[bit_count] \n\t"             // 5
    "breq 3f \n\t"                      // 6 | 7 if jump
    "nop \n\t"                          // 7
    "lsl %[color] \n\t"                 // 8
    "brcc 1b \n\t"                      // 9 | 10 if jump
    "rjmp 4f \n\t"                      // 11

    // Bridge to end
    "2: \n\t"
    "st %a[port], %[lo] \n\t"
    "rjmp 6f \n\t"

    // Last bit bridge
    "3: \n\t"
    "lsl %[color] \n\t"                  
    "brcc 5f \n\t"                       
    "rjmp 0b \n\t"                      

    // State 1 [ 13H | 8L ]
    "4: \n\t"
    "st %a[port], %[hi] \n\t"           // 2 prev -> set to hi
    "nop \n\t"                          // 1
    "nop \n\t"                          // 2
    "nop \n\t"                          // 3
    "nop \n\t"                          // 4
    "nop \n\t"                          // 5
    "nop \n\t"                          // 6
    "nop \n\t"                          // 7
    "nop \n\t"                          // 8
    "nop \n\t"                          // 9
    "dec %[bit_count] \n\t"             // 10
    "cpi %[bit_count], 0x01 \n\t"       // 11
    "st %a[port], %[lo] \n\t"           // 13 -> set to lo
    "breq 3b \n\t"                      // 1 | 2 if jump
    "nop \n\t"                          // 2
    "lsl %[color] \n\t"                 // 3
    "brcc 1b \n\t"                      // 4 | 5 if jump
    "rjmp 4b \n\t"                      // 6

    // Last bit 0 [ 7H | 13 L ]
    "5: \n\t"
    "nop \n\t"                          // 1 prev
    "st %a[port], %[hi] \n\t"           // 3 prev -> set to hi
    "ldi %[bit_count], 0x08 \n\t"       // 1
    "nop \n\t"                          // 2
    "nop \n\t"                          // 3
    "nop \n\t"                          // 4
    "nop \n\t"                          // 5
    "st %a[port], %[lo] \n\t"           // 7 -> set to lo
    "nop \n\t"                          // 1
    "nop \n\t"                          // 2
    "sbiw %[data_l], 0x01 \n\t"         // 4
    "breq 6f \n\t"                      // 5 | 6 if jump
    "ld %[color], %a[data]+ \n\t"       // 7
    "lsl %[color] \n\t"                 // 8
    "brcc 1b \n\t"                      // 9 | 10 if jump
    "rjmp 4b \n\t"                      // 11

    // END
    "6: \n\t"
    "nop \n\t"
    "nop \n\t"
    "nop \n\t"
    "nop \n\t"
    "nop \n\t"
    "nop \n\t"
    "nop \n\t"
    
    : [port] "+e" (p->port), [color] "+r" (color), [bit_count] "+r" (bit_count), [data_l] "+w" (data_l)
    : [data] "e" (data), [hi] "r" (hi), [lo] "r" (lo)
  );
#else
#error "Unsupported F_CPU!"
#endif
}


#endif // __AVR__