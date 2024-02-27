/* projectgame.c
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "projecthead.h"  /* Declarations for the project */

int timeoutcount = 0;
int timer = 0;
char animframe[4];
int playerPositionX = 0;
int playerPositionY = 9;

int playerPositionXbuffer = 0; //start the same as playerPosition
int playerPositionYbuffer = 9;

uint8_t buttonmap = 0;
uint8_t buttonFlag = 0;
uint8_t inputbuffer = 0; // 0000 0000, MSB = sw 4, LS 3 bits = btn 2 trhough 4;
char sw4 = 0;

void playerMovement(int action){
	switch (action)
	{
	case 0:
		//moving to the other lane
		if(sw4){
			playerPositionXbuffer-=4;
		}else{
			playerPositionXbuffer+=4;
		}
		break;
	case 2:
		//jumping
		switch (animframe[action])
		{
		case 1:
		case 2:
			playerPositionYbuffer+=10;
			//change_sprite_by_vector(16, 24, &playerPositionX, &playerPositionY, 0, 10);
			break;
		case 3:
			playerPositionYbuffer+=5;
			//change_sprite_by_vector(16, 24, &playerPositionX, &playerPositionY, 0, 5);
			break;
		case 5:
			playerPositionYbuffer-=5;
			//change_sprite_by_vector(16, 24, &playerPositionX, &playerPositionY, 0, -5);
			break;
		case 6:
			playerPositionYbuffer-=10;
			//change_sprite_by_vector(16, 24, &playerPositionX, &playerPositionY, 0, -10);
			break;
		}
		break;
	}
}

/* Interrupt Service Routine */
void user_isr( void )
{
	
	if(IFS(0)&0x100){
		timeoutcount++;
		
		IFSCLR(0) = 0x100;
		

	 	
		//if (timeoutcount%2 == 0){ //for debugging, halving, or even dividing by three the framerate
			display_upgrade();
			int i;
			for (i = 0; i < 4; i++){
				if(animframe[i] != 0){
					animframe[i]++;
					playerMovement(i);
				}
			}

			inputbuffer = sw4 << 7 | buttonmap;
			buttonmap = getbtns();
			sw4 = getsw();
		
		//}

			if (timeoutcount == 10) // fps setter
			{
				timeoutcount = 0;
				timer++;

				// here we used to tick a time by one second
			}
	}else if(IFS(0) & 0x0800){
		IFSCLR(0) = 0x0800;

	}
}

/* Game-specific initialization goes here */
void gameinit( void )
{

	TMR2 = 0;
  	PR2 = 31250;
  	IFSCLR(0) = 0x100; // clear interrupt flag for timer 2

	T2CON = T2CON & 0xfff7f0f | 0x70 | 0x8000;	  // set prescale ; enable timer 2
	IEC(0) = (IEC(0) & 0xfffffeff) | (0x1 << 8);  // T2IE set
	IPC(2) = ( IPC(2) & 0xffffffe0) | (0b111 << 2); // T2IP set to 5


//surp
	//  TRISD = TRISD | 0xfe0;
  	TRISDSET = 0x100;

	INTCONSET = 0b1 << 2; //enable interrupt priority

	IECSET(0) = 0x800; //enable INT2 interrupt
	IPCSET(2) = 0b111 << 26; //set Interrupt priority to 6

	/*CNCONSET = 0x8000;
	CNENSET = 0x200;*/

	TRISDSET = 0b1 << 10;

	enable_interrupt();
	
}

/* This function is called repetitively from the main program */
void projectwork( void ) {
	if(gamestate == 0) menu();
	if(gamestate == 1) game();
	if(gamestate == 2) multiplayermenu();
	if(gamestate == 8) highscores();
}

void menu( void ){
	buttonmap = 0;
	char selector = 0;
	const int menuOptions = 3;	//number of options
	char txtSelect[menuOptions];	//
	
	display_clear();
	timeoutcount=0;

	while(1){
		if(gamestate != 0) return;
		int j=0;
		for(j = 0; j<menuOptions; j++){
			if(j == selector) txtSelect[j] = 0xf;
			else txtSelect[j] = 0x0;
		}
		
		if (buttonmap == 0b100) //means down
		{
			if( selector < menuOptions -1) selector++;
			buttonmap = 0;
		}
		if (buttonmap == 0b010) //means up
		{
			if( selector > 0 ) selector--;
		 	buttonmap = 0;
		}
		if (buttonmap == 0b001)  //means select
		{
			if( selector == 0 ) //if we are selecting hall of fame
			{
				gamestate = 1;
			}

			if( selector == 2 ) //if we are selecting hall of fame
			{
				gamestate = 8;
			}

			buttonmap = 0;
		}

		 char z[2] = {sw4+48, 0};

		 display_string(3, "PLAY", 0);
		 display_string(0, z, 0);
		 display_string(4, " 1p ", txtSelect[0]);
		 display_string(5, " 2p ", txtSelect[1]);
		 display_string(8, "Hall", txtSelect[2]);
		 display_string(9, " of ", txtSelect[2]); 
		 display_string(10, "fame", txtSelect[2]);
		 display_smallNums(123, timer);
	}
}


//animating background transition when the switch is flipped
void transitionBackground(){
	switch(animframe[0]){
		case 1:
			//display_sprite(32, 90, bg1, 0, 0, ~sw4<<8);
			display_background(bg1, ~sw4<<8);
			break;
		case 2:
			//display_sprite(32, 90, bg2, 0, 0, ~sw4<<8);
			display_background(bg2, ~sw4<<8);
			break;
		case 3:
			//display_sprite(32, 90, bg3, 0, 0, sw4<<8);
			display_background(bg3, sw4<<8);
			break;
		case 4:
			//display_sprite(32, 90, bg2, 0, 0, sw4<<8);
			display_background(bg2, sw4<<8);
			break;
		case 5:
			//display_sprite(32, 90, bg1, 0, 0, sw4<<8);
			display_background(bg1, sw4<<8);
			animframe[0] = 0;
			break;
	}
}

//animating character run cycle
void characterRun(int posX, int posY){
	switch(animframe[1]){
		case 1:
			display_sprite(16, 24, chrRun[0], posX, posY, 0x000);
			break;
		case 2:
			display_sprite(16, 24, chrRun[1], posX, posY, 0x000);
			break;
		case 3:
			display_sprite(16, 24, chrRun[1], posX, posY, 0x100);
			break;
		case 4:
			display_sprite(16, 24, chrRun[0], posX, posY, 0x100);
			break;
		case 5:
			display_sprite(16, 24, chrRun[1], posX, posY, 0x100);
			break;
		case 6:
			display_sprite(16, 24, chrRun[1], posX, posY, 0x000);
			break;
		case 7:
			animframe[1] = 1;
			break;
	}
}

//animating character jump
void characterJump(int posX, int posY){
	switch(animframe[2]){
		case 1:
			display_sprite(16, 24, chrJump[0], posX, posY, 0x000);
			break;
		case 2:
		case 3:
		case 4:
			display_sprite(16, 24, chrJump[1], posX, posY, 0x000);
			break;
		case 5:
			display_sprite(16, 24, chrJump[2], posX, posY, 0x000);
			break;
		case 6:
			display_sprite(16, 24, chrJump[3], posX, posY, 0x000);
			break;
		case 7:
			animframe[2] = 0;
			animframe[1] = 1;
			buttonFlag = 0;
			break;
	}
}

//animating character roll
void characterRoll(int posX, int posY){
	switch(animframe[3]){
		case 1:
			display_sprite(16, 24, chrRoll[0], posX, posY, 0x000);
			break;
		case 2:
			display_sprite(16, 24, chrRoll[1], posX, posY, 0x000);
			break;
		case 3:
			display_sprite(16, 24, chrRoll[2], posX, posY, 0x000);
			break;
		case 4:
			display_sprite(16, 24, chrRoll[3], posX, posY, 0x000);
			break;
		case 5:
			display_sprite(16, 24, chrRoll[4], posX, posY, 0x000);
			break;
		case 6:
			display_sprite(16, 24, chrRoll[5], posX, posY, 0x000);
			break;
		case 7:
			display_sprite(16, 24, chrRoll[0], posX, posY, 0x000);
			break;
		case 8:
			animframe[3] = 0;
			animframe[1] = 1;
			buttonFlag = 0;
			break;
	}
}

void game( void ){
	display_clear();
	buttonmap = 0;

	//Currently done in adifferent way but we could also use this action var if need be 
	//int action = 0; //0 - run; 1 - jump; 2 - roll; 3 - switch side


	//display_gui();
	display_sprite(32, 36, gui, 0, 92, 0);
	//display_sprite(32, 90, bg1, 0, 0, sw4<<8);
	display_background(bg1, sw4<<8);
	display_upgrade();

	if(sw4){
		playerPositionX = 0;
		playerPositionXbuffer = 0;
	}else{
		playerPositionX = 16;
		playerPositionXbuffer = 16;
	}

	//character run begin 
	animframe[1] = 1;

	while(1){
		if(gamestate != 1) return;

		if( ( (inputbuffer&0x80) >>7) !=sw4){
			animframe[0] = 1;
		}
		//character actions
		if(inputbuffer & 0x07 && !buttonFlag){
			buttonFlag = 1;
			switch((inputbuffer&0x07)){ //input breaks when both roll and jump is pressed at same frame!
				case 0b010:
					animframe[2] = 1;
					animframe[1] = 0;
					break;
				case 0b100:
					animframe[3] = 1;
					animframe[1] = 0;
					break;
			}
		}

		if(playerPositionX!=playerPositionXbuffer){
			//This technically should not matter, since we are upgrading the background anyways
			//change_sprite_by_vector(16, 24, &playerPositionX, &playerPositionY, playerPositionXbuffer-playerPositionX, 0);
			playerPositionX = playerPositionXbuffer;
		}

		if(playerPositionY!=playerPositionYbuffer){
			change_sprite_by_vector(16, 24, &playerPositionX, &playerPositionY, 0, playerPositionYbuffer-playerPositionY);
			playerPositionY = playerPositionYbuffer;
		}

		if(animframe[0]){
			transitionBackground();
		}
		if(animframe[2]){
			characterJump(playerPositionX, playerPositionY);
		}
		if(animframe[3]){
			characterRoll(playerPositionX, playerPositionY);
		}
		if(animframe[1]){
			characterRun(playerPositionX, playerPositionY);
		}

		display_smallNums(7, timer);
		
		if (buttonmap == 0b001)  //means select
		{
			gamestate = 0;
			buttonmap = 0;
		}
	}
	
}

void multiplayermenu( void ){
	gamestate = 0;
	return;
}

void highscores( void ){
	display_clear();
	buttonmap = 0;
	
	//testing here

	display_sprite(8, 5, coin, 32-8, 100, 0);
	display_sprite(24, 28, train, 0, 0, 0);
	display_sprite(16, 12, shortBarrier, 0, 30, 0);
	display_sprite(16, 20, tallBarrier, 10, 50, 0);
	
	display_upgrade();
	


	while(1){
		if(gamestate != 8) return;
		//display_string(5, " hS", 0);
		
		display_smallNums(1, timer);
		
		if (buttonmap == 0b001)  //means select
		{
			gamestate = 0;
			buttonmap = 0;
		}
	}
	
}