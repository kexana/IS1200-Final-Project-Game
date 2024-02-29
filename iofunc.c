#include <stdint.h>
#include <pic32mx.h>
#include "projecthead.h"  /* Declatations for the project */

uint8_t getsw(void);
int getbtns(void);

uint8_t getsw(){

    return 0x00000000 | (((PORTD & 0x00000800) >> 11) &0b1); //only gets switch 4
}

int getbtns(){

    return 0x00000000 | ((PORTD & 0x000000e0) >> 5); //only gets buttons 2, 3, 4
}
