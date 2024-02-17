/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */

int mytime = 0x5957;

int prime = 1234567;

int timeoutcount = 0;

char textstring[] = "text, more text, and even more text!";

/* Interrupt Service Routine */
void user_isr( void )
{
	
	if(IFS(0)&0x100){
		timeoutcount++;
		IFSCLR(0) = 0x100;

		if (timeoutcount==10)
		{
			timeoutcount = 0;
			time2string( textstring, mytime );
			//display_string( 3, textstring );
			//display_update();
			tick( &mytime );
		}
	}else if(IFS(0) & 0x0800){
		IFSCLR(0) = 0x0800;
		
		mytime = mytime + 3;
	}

	
}

/* Lab-specific initialization goes here */
void labinit( void )
{
 	prime = nextprime( prime );
	//display_string( 0, itoaconv( prime ) );
	display_update();

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
void labwork( void ) {
	prime = nextprime( prime );
	//display_string( 0, itoaconv( prime ) );
	display_update();
	//display_image(96, icon);
}
