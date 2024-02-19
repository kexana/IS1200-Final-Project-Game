/* projectfunc.c 
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "projecthead.h"  /* Declatations for the project */


#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/*This function written 2015 by F Lundevall for the KTH IS1200 class
  Some parts might be original code written by Axel Isaksson
  quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/*This function written 2015 by F Lundevall for the KTH IS1200 class
  Some parts might be original code written by Axel Isaksson*/
uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}


/*This function written 2015 by F Lundevall for the KTH IS1200 class
  Some parts might be original code written by Axel Isaksson*/
void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void display_string(int line, char *s, char inv) {
	int i; int l;
	if(line < 0 || line >= 16) //change from base, for the vertical layout of text
		return;
  line = 16 - line - 1;
  if(!s)
		return;
	
  //3rd input is an inverter charachter, 4 least significant bits decide if the char to print is
  //inverted or not, e.g. 0b00001100 means the first two chars to print will be inverted

	for(i = 0; i < 4; i++)// number of charachters per line
		if(*s) {
			//goes through the string setting each character the right place for display_update
      int k; 
      int j; 
      char verted = (inv >> (3 - i) ) & 0b001;


      for(k = 0; k < 8; k++){
        for(j=0; j<8; j++){
          if(!verted) canvas[i * 8 + j][line * 8 + k] = (font[*s * 8 + k]) >> j;
        else canvas[i * 8 + j][line * 8 + k] = ( ~ font[*s * 8 + k]) >> j;
        }
      }

			s++;
		}

}

// test funciton for the small numbers, could be deleted later
void display_smallNums(){
	int i;
	for(i = 0; i < 6; i++){ // number of charachters per line
      int k; 
      int j; 
      for(k = 0; k < 5; k++){
        for(j=0; j<5; j++){
          canvas[i * 5 + j][13 * 5 + k] = (smallNumbers[(i) * 5 + k]) >> j;
		}
      }

	}
} 


//new functions for the project

void display_upgrade(void){
  int i, j, k;
	int c;
  char inv; //inverter
  char data;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0 & 0xF);
		spi_send_recv(0x10 | (0x0 & 0xF));
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
    for(j = 0; j < 128; j++){
      
      data = 0;
      for(k=0; k<8; k++){
        data = data | (canvas[k + 8*i][j])<<k;
      }
      spi_send_recv(data);
    }
  }
}

void display_clear( void ){
  int i; int j; int k;
  for(i=0; i<16; i++) display_string(i, "    ", 0);
  for(i=0; i<4; i++)
    for(j=0; j<128; j++)
      for(k=0; k<8; k++){
        canvas[i*8+k][j] = 0;
      }
}


