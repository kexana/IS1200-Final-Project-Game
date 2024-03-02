/*projecthighscore.c
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "projecthead.h"  /* Declatations for the project */


void highscoreinit( void ){
    int i; int j;
	for(i =0; i<9; i++){
		scorekeeppoint[i] = 0;
		for(j = 0; j<4; j++){
			scorekeepername[i][j] = ' ';
		}
        scorekeepername[i][4] = 0;
	}
}

void appendhighscore(int score, char name[4]){
    int i; int j; int k;

    char canWewrite = 0;

    char newname[5];

    for(i=0; i<9; i++){
        if(scorekeeppoint[i]==0) canWewrite = 1;
        if(scorekeeppoint[i]<score) canWewrite = 1;
    }

    //if(canWewrite == 0) return;

    

    int index = 0;
    for(i = 0; i<9; i++){
        if(score > scorekeeppoint[i]){
            index = i;
            break;
        }
    }

    newname[0] = index+49;
    for(i=0; i<4; i++){
        newname[i+1] = name[i];
    }
    
    



    for(i = 7; i>=index; i--){
        for(j=0; j<5; j++){
            scorekeeppoint[i+1] = scorekeeppoint[i];
            scorekeepername[i+1][j]= scorekeepername[i][j];

        }
        
    }
    for(j=0; j<5; j++){
        scorekeeppoint[index] = score;
        scorekeepername[index][j] = newname[j];
    }


}
/*helper function only*/
void display_string_pixelLine(int line, char *s, char inv) {
	int i; int l;
	if(line < 0 || line >= 128) //change from base, for the vertical layout of text
		return;
  	line = 128 - line - 1;
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
        		if(!verted) canvas[i * 8 + j][line + k] = (font[*s * 8 + k]) >> j;
        		else canvas[i * 8 + j][line + k] = ( ~ font[*s * 8 + k]) >> j;
        	}
      	}
		s++;
		}
	}

}

void displayhighscore( void ){
    int i; int j; int k;

    for(i = 0; i<9; i++){
        if(scorekeeppoint[i]){
            scorekeepername[i][0] = i+49;
            display_string_pixelLine(10+i*14, scorekeepername[i], 0);
            display_smallNums((10+i*14), scorekeeppoint[i]);
        }

    }
    
}

void takehighscore( int newscore ){
    int wait = 0;
    char submitname[] = "AAA";
    int kutya = 0;
    int kutyaIndex = 0;
    //if(newscore<=scorekeeppoint[8]) return;
    buttonmap = 0;
    while(1) {
        if(newFrameFlag){
            newFrameFlag = 0;
            if(wait>0) wait--;
        }

        if(wait==0){
            if(buttonmap== 0b100){ //means down
                kutya=-1;
                buttonmap = 0;
            }
            if(buttonmap== 0b010){ //means up
                kutya=1;
                buttonmap = 0;
            }
            if(buttonmap == 0b001){ //select
                kutya = 0;
                kutyaIndex++;
                buttonmap = 0;
                if(kutyaIndex == 4){
                appendhighscore(newscore, submitname);
                gamestate = 0;
                return;
            }
            }
            
            if((submitname[kutyaIndex]<=125 && submitname[kutyaIndex]>=33) || (submitname[kutyaIndex]==126 && kutya == -1) || (submitname[kutyaIndex]==32 && kutya == 1)) submitname[kutyaIndex] += kutya;
            kutya = 0;
            display_string(6, "YOU", 0b0);
            display_string(7, " ARE", 0);
            display_string(8, "IN ", 0);
            display_string(9, " THE", 0);
            display_string(10, "TOP", 0b1110);
            char highlighter=0;
            switch (kutyaIndex)
            {
            case 0:
                highlighter=0b1000;
                break;
            
            case 1:
                highlighter=0b0100;
                break;
            case 2:
                highlighter=0b0010;
                break;
            case 3:
                highlighter=0b0001;
                break;
            }
            display_string(12, submitname, highlighter);
        }
        
        
    }
}

