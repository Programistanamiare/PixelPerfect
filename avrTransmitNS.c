#ifdef __AVR__

#include "LED.h"

void transmitNS(volatile uint8_t *data, volatile uint16_t data_l, const gpio_pin_t *p) {
  register uint8_t color asm("r16");
  register uint8_t bit_count asm("r17");
  const uint8_t hi = *p->port | (1 << p->pin), lo = *p->port & ~(1 << p->pin);

#if (F_CPU >= 14000000) && (F_CPU <= 20000000)
  asm volatile(
    // Init
    "ld %[bit_count], 0x08 \n\t"
    "ld %[color], %a[data]+ \n\t"
    "lsl %[color] \n\t"
    "brcc 0f \n\t"
    "rjmp 4f \n\t"

    // Last state 1 [ 2L | 13H | 4/5L ]
    "5: \n\t"
    "st %[port], %[hi] \n\t"          // 2 prev -> set to hi
    "ldi %[bit_count], 0x08 \n\t"     // 1
    "nop \n\t"                        // 2
    "nop \n\t"                        // 3
    "nop \n\t"                        // 4
    "nop \n\t"                        // 5
    "nop \n\t"                        // 6
    "nop \n\t"                        // 7
    "sbiw %[data_l], 0x01 \n\t"       // 9
    "breq 6f \n\t"                    // 10 | 11 if jump
    "nop \n\t"                        // 11
    "st %[port], %[lo] \n\t"          // 13 -> set to lo
    "nop \n\t"                        // 1
    "lsl %[color] \n\t"               // 2
    "brcc 0b \n\t"                    // 3 | 4 if jump
    "rjmp 4b \n\t"                    // 5

    // State 0 [ 3L | 6H | 11/12L ]
    "0: \n\t"                         
    "nop \n\t"                        // 1 prev
    "st %a[port], %[hi] \n\t"         // 3 prev -> set to hi
    "nop \n\t"                        // 1
    "nop \n\t"                        // 2
    "nop \n\t"                        // 3
    "nop \n\t"                        // 4
    "st %a[port], %[lo] \n\t"         // 6 -> set to lo
    "nop \n\t"                        // 1
    "nop \n\t"                        // 2
    "nop \n\t"                        // 3
    "nop \n\t"                        // 4
    "dec %[bit_count] \n\t"           // 5
    "cpi %[bit_count], 0x01 \n\t"     // 6
    "breq 1f \n\t"                    // 7 | 8 if jump to 
    "nop \n\t"                        // 8
    "lsl %[color] \n\t"               // 9
    "brcc 0b \n\t"                    // 10 | 11 if jump [ State 0 ]
    "rjmp 4f \n\t"                    // 12 jump to [ State 1 ]

    // Last byte bridge from state 0 (end with 8)
    "1: \n\t"
    "lsl %[color] \n\t"               // 9
    "brcc 2f \n\t"                    // 10 | 11 if jump
    "rjmp 5f \n\t"                    // 12

    // Last state 0 [ 3L | 6H | 11/12L ]
    "2: \n\t"
    "nop \n\t"                        // 1 prev
    "st %[port], %[hi] \n\t"          // 3 prev -> set to hi
    "ldi %[bit_count], 0x08 \n\t"     // 1
    "nop \n\t"                        // 2
    "nop \n\t"                        // 3
    "nop \n\t"                        // 4
    "st %[port], %[lo] \n\t"          // 6 -> set to lo
    "nop \n\t"                        // 1
    "nop \n\t"                        // 2
    "nop \n\t"                        // 3
    "sbiw %[data_l], 0x01 \n\t"       // 5
    "breq 6f \n\t"                    // 6 | 7 if jump
    "ld %[color], %a[data]+ \n\t"     // 8
    "lsl %[color] \n\t"               // 9
    "brcc 0b \n\t"                    // 10 | 11 if jump
    "rjmp 4b \n\t"                    // 12

    // Last byte bridge from state 1 (end with 11)
    "3: \n\t"
    "st %[port], %[lo] \n\t"          // 13 -> set to low
    "nop \n\t"                        // 1
    "lsl %[color] \n\t"               // 2
    "brcc 2b \n\t"                    // 3 | 4 if jump
    "rjmp 5f \n\t"                    // 5
    
    // State 1 [ 2L | 13H | 4/5L ]
    "4: \n\t"
    "st %[port], %[hi] \n\t"          // 2 prev -> set to hi
    "nop \n\t"                        // 1
    "nop \n\t"                        // 2
    "nop \n\t"                        // 3
    "nop \n\t"                        // 4
    "nop \n\t"                        // 5
    "nop \n\t"                        // 6
    "nop \n\t"                        // 7
    "dec %[bit_count] \n\t"           // 8
    "cpi %[bit_count], 0x01 \n\t"     // 9
    "breq 2b \n\t"                    // 10 | 11 if jump
    "nop \n\t"                        // 11
    "st %[port], %[lo] \n\t"          // 13 -> set to lo
    "nop \n\t"                        // 1
    "lsl %[color] \n\t"               // 2
    "brcc 0b \n\t"                    // 3 | 4 if jump
    "rjmp 4b \n\t"                    // 5

    // End transmit (set low, and wait 3/4 nop)
    "6: \n\t"
    "st %[port], %[lo] \n\t"
    "nop \n\t"
    "nop \n\t"
    "nop \n\t"
    "nop \n\t"
  );
#elif (F_CPU >= 5500000) && (F_CPU <= 8500000)
  
#else
#error "Unsupported F_CPU!"
#endif
}


#endif // __AVR__