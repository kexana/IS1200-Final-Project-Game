/* projectfunc.c 
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "projecthead.h"  /* Declarations for the project */


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
	
  //3rd input is an inverter character, 4 least significant bits decide if the char to print is
  //inverted or not, e.g. 0b00001100 means the first two chars to print will be inverted

	for(i = 0; i < 4; i++){// number of characters per line
		if(*s) {
		//goes through the string setting each character the right place for display_update, unless empty
      	
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

}

//could be aranged -> note: maybe in functions where we write to canvas, we should call it draw instead of display, it makes more sense that way

//taken care of -> note: maybe change the small 8, it looks very much like the zero.. otherwise v cool

void display_smallNums(int line, int num){
	int i; int k; int j; 
	int numtoprint;
	int divisor = 100000;

	char zerosFlag = 0; //set this flag to 1 before the loop to display leading zeroes

	line = 127 - line;
	for(i = 0; i < 6; i++){ // number of characters per line
		numtoprint = num / divisor;
		num = num%divisor;
		divisor = divisor /10;

		if(!zerosFlag){
			if(numtoprint == 0) continue;
			zerosFlag = 1;
		}

    	for(k = 0; k < 5; k++){
    		for(j=0; j<5; j++){
        		canvas[i * 5 + j ][line -5 + k] = (smallNumbers[(numtoprint) * 5 + k]) >> j;
			} 
    	}

	}
} 

/*Displays a Sprite of size xSize by ySize, with the xOffset and yOffset
In order to work properly the sprite bitmap array should be declared as x = 32 by y = whatever height
and the actual sprite x dimension should be divisible by 8*/
void display_sprite(int xSize, int ySize, const uint8_t pxlarray[], int xOffset, int yOffset, short inf){
	int i, j, k;

	int canvasx; int canvasy;
	
	//info 16bit int, least 8 bits are for the canvas information (that is the 7 bits above the lowest)
	//the other 8 are for flipping x or y axis; bit N.o. 8 is flip around y axis ((inf >> 7) & 0b1)
	if((inf >> 8) & 0b1){	//if sprite is flipped around y axis
		for(i=0; i<ySize; i++){ //i for each row
			canvasy = i + yOffset;
			if(canvasy<0 || canvasy>127) continue;

			for(j=0; j<xSize/8; j++){ //j for each column in the row //division by 8 because one character is 8 bits
				
				char temp = pxlarray[(ySize-1-i)*4 + (xSize/8)-1 - j];

				for(k=j*8; k<xSize; k++){

					canvasx =  xOffset + k;	//this is the important change
					if(canvasx<0 || canvasx>31) continue;


					canvas[canvasx][canvasy] = canvas[canvasx][canvasy]&0b10 | temp & 0b1 | (inf & 0xfc); //PAY ATTENTION TO INDEX OUT OF BOUNDS, IT WILL BREAK THE GAME
					temp = temp >> 1;
				}
			}
		}
	}else for(i=0; i<ySize; i++){ //i for each row //sprite is not flipped around y axis
			canvasy = i + yOffset;
			if(canvasy<0 || canvasy>127) continue; //it proved to be very good praxis to always check this

			for(j=0; j<xSize/8; j++){ //j for each column in the row //division by 8 because one character is 8 bits
				
				char temp = pxlarray[(ySize-1-i)*4 + (xSize/8)-1 - j];

				for(k=j*8; k<xSize; k++){

					canvasx = xSize-1 -k + xOffset;
					if(canvasx<0 || canvasx>31) continue;


					canvas[canvasx][canvasy] = canvas[canvasx][canvasy]&0b10 | temp & 0b1 | (inf & 0xfc); //PAY ATTENTION TO INDEX OUT OF BOUNDS, IT WILL BREAK THE GAME
					temp = temp >> 1;
				}
			}
		}
}

/*we draw and set the reserved background bit to the relevant background; 
remember backgrounds are always 32x90, and always placed at 0, 0*/
void display_background(const uint8_t pxlarray[], short inf){
	int i, j, k;
	
	//function call			display_sprite(32, 90, bg1, 0, 0, ~sw4<<8);
	//is replaced by		display_background(bg1, ~sw4<<8);

	if((inf >> 8) & 0b1){
		for(i=0; i<90; i++){ //i for each row
			for(j=0; j<4; j++){ //j for each column in the row
				
				char temp = pxlarray[(89-i)*4 + 3 - j];

				for(k=j*8; k<32; k++){
					canvas[k][i] = (temp & 0b1)<<2 | (temp & 0b1)<<1 | (temp & 0b1); //| (inf & 0xf8); 
					temp = temp >> 1;
				}
			}
		}
	}else for(i=0; i<90; i++){ //i for each row
			for(j=0; j<4; j++){ //j for each column in the row //division by 8 because one character is 8 bits
				
				char temp = pxlarray[(89-i)*4 + 3 - j];

				for(k=j*8; k<32; k++){
					canvas[31-k][i] = (temp & 0b1)<<2 | (temp & 0b1)<<1 | (temp & 0b1); //| (inf & 0xf8); 
					//PAY ATTENTION TO INDEX OUT OF BOUNDS, IT WILL BREAK THE GAME
					//PAY ATTENTION TO SPRITE DATA BITS; IT CAN BREAK THE GAME  //  | (0b0000 << 3);
					
					temp = temp >> 1;
				}
			}
		}
}

/*we move sprite location on canvas by vector x and vector y 
the remaining artifacts are cleared, and background is shown instead;
this does not draw the sprite, just updates its position!
use this whenever you change the position of a sprite!
note: do not change a sprite's position during an interrupt, put it on a buffer, and deal with it in main()*/
void change_sprite_by_vector(int xSize, int ySize, int *x, int *y, int vectorX, int vectorY){
	int i; int j;

	//remember, canvas[][] is 2d array of characters, only the last bit is passed to the display as data (on the correct page...)
	//see: display_upgrade()

	//we can use the M.S. 7 B. as information about sprites and such;
	//our convention is to use the 2nd LSB as background layer;

	//as such when we shift a sprite by vector, we shall update the canvas that the rectangle of pixels where the sprite used to be
	// shall display the old background once again

	int canvasx; int canvasy;
	if(vectorX>0){
		for(i=0; i<vectorX; i++){
			canvasx = i+*x;
			if(canvasx<0 || canvasx>31) continue;

			for(j=0; j<ySize; j++){
				canvasy = j+*y;
				if(canvasy<0 || canvasy>127) continue;

				canvas[canvasx][canvasy] = canvas[canvasx][canvasy] & 0xfe | (canvas[canvasx][canvasy]>>1)&0b1;
			}
		}
	}else{
		for(i=0; i>vectorX; i--){
			canvasx = i+*x+xSize;
			if(canvasx<0 || canvasx>31) continue;

			for(j=0; j<ySize; j++){
				canvasy = j+*y;
				if(canvasy<0 || canvasy>127) continue;

				canvas[canvasx][canvasy] = canvas[canvasx][canvasy] & 0xfe | (canvas[canvasx][canvasy]>>1)&0b1;

			}
		}
	}
		
	if(vectorY>0){
		for(i=0; i<xSize; i++){
			canvasx = i+*x;
			if(canvasx<0 || canvasx>31) continue;

			for(j=0; j<vectorY; j++){
				canvasy = j+*y;
				if(canvasy<0 || canvasy>127) continue;

				canvas[canvasx][canvasy] =  canvas[canvasx][canvasy] & 0xfe | (canvas[canvasx][canvasy]>>1)&0b1;
			}
		}
	}else{
		for(i=0; i<xSize; i++){
			canvasx = i+*x;
			if(canvasx<0 || canvasx>31) continue;

			for(j=0; j>=vectorY; j--){ //note the >= (idk why, idk maths ¯\_(ツ)_/¯)
				canvasy = j+*y+ySize;
				if(canvasy<0 || canvasy>127) continue;

				canvas[canvasx][canvasy] = canvas[canvasx][canvasy] & 0xfe | (canvas[canvasx][canvasy]>>1)&0b1;

			}
		}
	}

	*x +=vectorX; 
	*y +=vectorY;

}

/*PLEASE TAKE NOTE OF CANVAS BIT LAYOUT (a canvas[x][y] is 1 char, that is 8 bits)
LSB 0:		Display bit - this bit is the data which is sent by display_upgrade unto the oled display
LSB 1:		Background bit - this bit is reserved for the background, it maps the current background's pixel status in that x,y pos
LSB 2:		Sprite/Layer Data bit
LSB 3-6:	4bit Encoding of what type of sprite/layer is loaded in LSB 2 (Sprite/Layer Data bit)
			0x0: Background
			0x1: Coin Sprite
			0x2: Train Sprite
			0x3: ShortBarrier Sprite
			0x4: TallBarrier Sprite
			(goes up to 0xf, or if we dont have enough things then we can use one more bit of information)
LSB 7:		Player bit - this is essentially player hitbox, its the cutout player sprite,
			it is also used when player pos is updated by vector to clean artifacts

(remember LSB 7 is the same as MSB 0, the largest bit in a char)
*/

/*Takes in a playercharacter bitmap array;
cuts out the unneeded bits, flips it if needed, and displays it
it also sets the 8th LSB to the player bits
remember the player sprite size is always 16x24 pixels*/
void display_player(const uint8_t pxlarray[], int xPos, int yPos, short inf){
	int i, j, k;

	int const xSize = 16; int const ySize = 24;
	int canvasx; int canvasy;

	char flagLeft;
	char flagRight;
	
	//info 16bit int, least 8 bits are for the canvas information (that is the 7 bits above the lowest)
	//the other 8 are for flipping x or y axis; bit N.o. 8 is flip around y axis ((inf >> 7) & 0b1)
	if((inf >> 8) & 0b1){	//if sprite is flipped around y axis
		for(i=0; i<ySize; i++){ //i for each row
			canvasy = i + yPos;
			if(canvasy<0 || canvasy>127) continue;

			flagLeft =0;
			flagRight =0;

				short localizer = (pxlarray[(ySize-1-i)*4 + 1])+(pxlarray[(ySize-1-i)*4]<<8);

				char locL =0;
				char locR =15;

				char locLflag =0;
				char locRflag =0;
				for(j=0; j<16; j++){
					if(!locLflag){
						if((localizer>>j)&0b1) {locLflag=1; locL=j;}
					}
					if(!locRflag){
						if((localizer<<j)&0x8000) {locRflag=1; locR=15-j;}
					}
				}

			/*for(j=0; j<xSize/8; j++){ //j for each column in the row //division by 8 because one character is 8 bits
				*/
				//char temp = pxlarray[(ySize-1-i)*4 + (xSize/8)-1 - j];
				//char nexttemp = pxlarray[(ySize-1-i)*4 + (xSize/8)-1 - j]

				for(k=/*j*8*/0; k<xSize; k++){

					canvasx =  xPos + k;	//this is the important change
					if(canvasx<0 || canvasx>31) continue;

					/*if(!flagLeft){
						if((temp&0b1) == 0){
							canvas[canvasx][canvasy] = canvas[canvasx][canvasy]&0x7e | (canvas[canvasx][canvasy]&0b100) >>2;
							temp = temp >> 1;
							continue;
						}
						flagLeft = 1;
					}*/

					/*if(flagLeft) canvas[canvasx][canvasy] = canvas[canvasx][canvasy]&0b10 | temp & 0b1 | (inf & 0xfc);//PAY ATTENTION TO INDEX OUT OF BOUNDS, IT WILL BREAK THE GAME
					if(flagLeft) temp = temp >> 1;*/

					if((k>=locL) && (k<=locR)) canvas[canvasx][canvasy] = canvas[canvasx][canvasy]&0b110 | localizer & 0b1 | (inf & 0xf8); //PAY ATTENTION TO INDEX OUT OF BOUNDS, IT WILL BREAK THE GAME
					else canvas[canvasx][canvasy] = canvas[canvasx][canvasy]&0x7e | (canvas[canvasx][canvasy]&0b100) >>2;
					localizer = localizer >> 1;

					/*if((k>=locL) && (k<=locR)) {canvas[canvasx][canvasy] = canvas[canvasx][canvasy]&0b10 | temp & 0b1 | (inf & 0xfc);}
					else canvas[canvasx][canvasy] = canvas[canvasx][canvasy]&0x7e | (/*canvas[canvasx][canvasy]&*//*0b100) >>2;
					temp = temp >> 1;*/
				}
			//}
		}
	}else for(i=0; i<ySize; i++){ //i for each row //sprite is not flipped around y axis
			canvasy = i + yPos;
			if(canvasy<0 || canvasy>127) continue; //it proved to be very good praxis to always check this
			
			flagLeft =0;
			flagRight =0;

			//for(j=0; j<xSize/8; j++){ //j for each column in the row //division by 8 because one character is 8 bits
				
				//char temp = pxlarray[(ySize-1-i)*4 + (xSize/8)-1 - j];

				short localizer = (pxlarray[(ySize-1-i)*4 + 1])+(pxlarray[(ySize-1-i)*4]<<8);

				char locL =0;
				char locR =15;

				char locLflag =0;
				char locRflag =0;
				for(j=0; j<16; j++){
					if(!locLflag){
						if((localizer>>j)&0b1) {locLflag=1; locL=j;}
					}
					if(!locRflag){
						if((localizer<<j)&0x8000) {locRflag=1; locR=15-j;}
					}
				}
				for(k=/*j*8*/0; k<xSize; k++){
					canvasx = xSize-1 -k + xPos;
					if(canvasx<0 || canvasx>31) continue;
					/*if(!flagLeft){
						if((temp&0b1) == 0){ //jézus krisztus szupertsztár, ha nincs ott a zárójel nem működik xddddd
							canvas[canvasx][canvasy] = canvas[canvasx][canvasy]&0x7e | (/*canvas[canvasx][canvasy]&*//*0b100) >>2;
							temp = temp >> 1;
							continue;
						}
						flagLeft = 1;
					}*/
					if((k>=locL) && (k<=locR)) canvas[canvasx][canvasy] = canvas[canvasx][canvasy]&0b110 | localizer & 0b1 | (inf & 0xf8); //PAY ATTENTION TO INDEX OUT OF BOUNDS, IT WILL BREAK THE GAME
					else canvas[canvasx][canvasy] = canvas[canvasx][canvasy]&0x7e | (canvas[canvasx][canvasy]&0b100) >>2;
					localizer = localizer >> 1;
				}
			//}
		}
}

/*this is essentially the display_update() but without textbuffer, and is just nicer /w a canvas*/
void display_upgrade( void ){
	int i, j, k;
	
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
        data = data | (canvas[k + 8*i][j] &0b1)<<k;
      }
      spi_send_recv(data);
    }
  }
}

void display_clear( void ){
  int i; int j; int k;
  for(i=0; i<4; i++)
    for(j=0; j<128; j++)
      for(k=0; k<8; k++){
        canvas[i*8+k][j] = 0;
      }
}


