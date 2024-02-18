/* projectgame.c
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "projecthead.h"  /* Declatations for the project */

int timeoutcount = 0;

char buttonmap = 0;

/* Interrupt Service Routine */
void user_isr( void )
{
	
	if(IFS(0)&0x100){
		timeoutcount++;
		IFSCLR(0) = 0x100;
	 	buttonmap = getbtns();
	 	display_upgrade();

		if (timeoutcount==10)		//fps setter
		{
			timeoutcount = 0;
			
			//here we used to tick a time by one second

			display_upgrade();
			
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

	enable_interrupt();
	
}

/* This function is called repetitively from the main program */
void projectwork( void ) {
	if(gamestate == 0) menu();
	if(gamestate == 1) ;
	if(gamestate == 2) ;
	if(gamestate == 8) highscores();
}

void menu( void ){
	char selector = 0;
	const int menuOptions = 3;	//number of options
	char txtSelect[menuOptions];	//
	
	char c[3];
	display_clear();

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
			if( selector == menuOptions-1 ) //if we are selecting hall of fame
			{
				gamestate = 8;
			}
		}
		int i=0;
		for(i = 0; i<3; i++){
			c[i] =  ( ( buttonmap >> 2-i ) &0b1 ) + 48;
		} 

		display_string(0, c, 0 );
		display_string(3, "PLAY", 0);
		display_string(4, " 1p ", txtSelect[0]);
		display_string(5, " 2p ", txtSelect[1]);
		display_string(8, "Hall", txtSelect[2]);
		display_string(9, " of ", txtSelect[2]); 
		display_string(10, "fame", txtSelect[2]);
		//display_update();
	}
}

void highscores( void ){
	display_clear();
	buttonmap = 0;

	while(1){
		if(gamestate != 8) return;
		display_string(5, " hS", 0);
		if (buttonmap == 0b001)  //means select
		{
			gamestate = 0;
		}
	}
	
}