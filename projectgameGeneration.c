/* projectgameGeneration.c
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "projecthead.h"  /* Declatations for the project */

uint8_t queueHead[] ={-1, -1};
uint8_t queueTail[] ={-1, -1};

int obsticlePositionX[2][4];
int obsticlePositionY[4];

uint8_t queueMaxSize = 4;
int obstCount=0;

char cicamica = 0;

// decides how many empty spaces there are on the field; goes from 0 to 15 (0 is almost all empty; 15 is almost no empty)
int emptySpaceCoef=10;

// lane 0 = left; 1=right, value = type of obstcle 0=empty, 1=tall barrier, 2=short barrier, 3=train
void enqueue(int lane, uint8_t value)
    {
        if (queueTail[lane] != queueMaxSize - 1)
        {
            if (queueHead[lane] == -1)
            {
                queueHead[lane] = 0;
            }
            queueTail[lane]++;
            currentFieldQueue[lane][queueTail[lane]] = value;
            //currentFieldQueue[lane][queueTail[lane]] = ((currentFieldQueue[lane][queueTail[lane]] & 0x0003) | (0x8 << 9)) | 0x46 << 2;
            obsticlePositionX[0][queueTail[lane]] = -8;
            obsticlePositionX[1][queueTail[lane]] = 8;
            obsticlePositionY[queueTail[lane]] = 70;
            /*obsticlePositionXBuffer[queueTail[lane]] = 8;
            obsticlePositionYBuffer[queueTail[lane]] = 70;*/
        }
    }
// lane 0 = left; 1=right
void dequeue(uint8_t lane){
    if(queueHead[lane] == queueTail[lane]){
      //Empty
   }else{
        currentFieldQueue[0][queueHead[lane]] = -1;
        currentFieldQueue[1][queueHead[lane]] = -1;
        queueHead[lane]++;
        if(queueHead[lane] == queueTail[lane]){
	        queueHead[lane] = queueTail[lane] = -1;
        }
   }
}

int rand();
int rseed = 1;

inline void srand(int x)
{
	rseed = x;
}

#ifndef MS_RAND
#define RAND_MAX ((1U << 31) - 1)

// Linear congruantial Generator borrowed form Rosseta Code article on related topic
inline int rand()
{
	return rseed = (rseed * 1103515245 + 12345) & RAND_MAX;
}

#else /* MS rand */

#define RAND_MAX_32 ((1U << 31) - 1)
#define RAND_MAX ((1U << 15) - 1)

inline int rand()
{
	return (rseed = (rseed * 214013 + 2531011) & RAND_MAX_32) >> 16;
}

#endif/* MS_RAND */

void initializeFieldQueue(){
    int i;
    for (i = 0; i < queueMaxSize; i++)
    {
        currentFieldQueue[0][i] = -1;
        currentFieldQueue[1][i] = -1;
        obsticlePositionY[i] = 70;
    }
    queueHead[0] = -1;
    queueHead[1] = -1;
    queueTail[0] = -1;
    queueTail[1] = -1;
    obstCount = 0;
}

void moveObsticles(uint8_t switchState){
    int i;

    char cica =0;

    cicamica++;
    if(cicamica%6==0) {if(switchState) {cica=-1;}else{ cica=1;}}
    for (i = 0; i < queueMaxSize; i++){
        switch(currentFieldQueue[0][i]){
            case 1:
                change_sprite_by_vector(16, 20, &obsticlePositionX[0][i], &obsticlePositionY[i], cica, -1);
                break;
            case 2:
                change_sprite_by_vector(16, 12, &obsticlePositionX[0][i], &obsticlePositionY[i], cica, -1);
                break;
            case 3:
                change_sprite_by_vector(24, 28, &obsticlePositionX[0][i], &obsticlePositionY[i], cica, -1);
                break;
        }
        switch(currentFieldQueue[1][i]){
            case 1:
                change_sprite_by_vector(16, 20, &obsticlePositionX[1][i], &obsticlePositionY[i], cica, -1);
                break;
            case 2:
                change_sprite_by_vector(16, 12, &obsticlePositionX[1][i], &obsticlePositionY[i], cica, -1);
                break;
            case 3:
                change_sprite_by_vector(24, 28, &obsticlePositionX[1][i], &obsticlePositionY[i], cica, -1);
                break;
        }
	}
}

void drawObsticles(uint8_t switchState){
    int i;
    for (i = 0; i < queueMaxSize; i++){
        
        int xOffset = switchState * (((obsticlePositionY[i])+210)/18);
        switch(currentFieldQueue[0][i]){
			case 1:
				display_sprite(16, 20, tallBarrier, (obsticlePositionX[0][i] + xOffset), obsticlePositionY[i], 0x20);
				break;
			case 2:
				display_sprite(16, 12, shortBarrier, (obsticlePositionX[0][i] + xOffset), obsticlePositionY[i], 0x18);
				break;
			case 3:
				display_sprite(24, 28, train, (obsticlePositionX[0][i] -4 + xOffset), obsticlePositionY[i], switchState<<8 | 0x10);
				break;
		}
		switch(currentFieldQueue[1][i]){
			case 1:
				display_sprite(16, 20, tallBarrier, (obsticlePositionX[1][i] + xOffset), obsticlePositionY[i], 0x20);
				break;
			case 2:
				display_sprite(16, 12, shortBarrier, (obsticlePositionX[1][i] + xOffset), obsticlePositionY[i], 0x18);
				break;
			case 3:
				display_sprite(24, 28, train, (obsticlePositionX[1][i] -4 + xOffset), obsticlePositionY[i], switchState<<8 | 0x10);
				break;
		}
    }
}

//function to assign obsticles to the game field
void fieldGeneration() {
    //for (i = 0; i < queueMaxSize;i++){ // while(currentFieldQueue[0][4]==0xffff){
        if(currentFieldQueue[0][obstCount]==0xffff){
            int randValue =rand();
            int randLeft = randValue&0xff;
            int randRight = (randValue&0xfff00)>>8;
            
            char trainFlag=0;
            //Left Side:
            if((randLeft&0xf0)>>4 > emptySpaceCoef){
                //printf("Empty Space\n");
                enqueue(0, 0);
            }else{
                if((randLeft&0x40)>>6==0b0){
                    //printf("Tall barrier\n");
                    enqueue(0,1);
                }else{
                    if((randLeft&0x20)>>5==0b0){
                        //printf("Short barrier\n");
                        enqueue(0,2);
                    }else{
                        //printf("Train\n");
                        enqueue(0,3);
                        trainFlag =1;
                    }
                }
            }
            //Right Side: 
            if((randRight&0xf0)>>4 > emptySpaceCoef){
            //printf("Empty Space\n");
                enqueue(1,0);
            }else{
                if((randRight&0x40)>>6==0b0){
                    //printf("Tall barrier\n");
                    enqueue(1,1);
                }else{
                    if((randRight&0x20)>>5==0b0){
                        //printf("Short barrier\n");
                        enqueue(1,2);
                    }else{
                        if(!trainFlag){
                            //printf("Train\n");
                            enqueue(1,3);
                        }else{
                            //printf("Empty Space\n");
                            enqueue(1,0);
                        }
                    }
                }
            }
        }else if(obsticlePositionY[obstCount] <0 ){
            dequeue(0);
            dequeue(1);
        }
    //}
    obstCount++;
    if(obstCount>queueMaxSize){
        obstCount = 0;
    }
}

