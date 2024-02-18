#include <stdint.h>
#include <pic32mx.h>
#include "projecthead.h"  /* Declatations for the project */

int getsw(void);
int getbtns(void);

int getsw(){

    return 0x00000000 | ((PORTD & 0x00000f00) >> 8);
}

int getbtns(){

    return 0x00000000 | ((PORTD & 0x000000e0) >> 5);
}
