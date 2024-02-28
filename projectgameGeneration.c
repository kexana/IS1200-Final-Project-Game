/* projectgameGeneration.c
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "projecthead.h"  /* Declatations for the project */

uint8_t queueHead[] ={-1, -1};
uint8_t queueTail[] ={-1, -1};

uint8_t obsticlePositionXBuffer[4];
uint8_t obsticlePositionYBuffer[4];
uint8_t obsticlePositionX[4];
uint8_t obsticlePositionY[4];

uint8_t queueMaxSize = 4;

// decides how many empty spaces there are on the field; goes from 0 to 15 (0 is almost all empty; 15 is almost no empty)
int emptySpaceCoef=15;

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
            obsticlePositionX[queueTail[lane]] = 8;
            obsticlePositionY[queueTail[lane]] = 70;
            obsticlePositionXBuffer[queueTail[lane]] = 8;
            obsticlePositionYBuffer[queueTail[lane]] = 70;
        }
    }
// lane 0 = left; 1=right
void dequeue(uint8_t lane){
    if(queueHead[lane] == queueTail[lane]){
      //Empty
   }else{
      queueHead[lane]++;
      if(queueHead[lane] == queueTail[lane])
	 queueHead[lane] = queueTail[lane] = -1;
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
        obsticlePositionYBuffer[i] = 70;
    }
    queueHead[0] = -1;
    queueHead[1] = -1;
    queueTail[0] = -1;
    queueTail[1] = -1;
}

void moveObsticles(uint8_t switchState){
    int i;
    for (i = 0; i < queueMaxSize; i++){
        switch(currentFieldQueue[0][i]){
            case 1:
                change_sprite_by_vector(16, 20, (int*)&obsticlePositionX[i], (int*)&obsticlePositionY[i], 0, -1);
                break;
            case 2:
                change_sprite_by_vector(16, 12, (int*)&obsticlePositionX[i], (int*)&obsticlePositionY[i], 0, -1);
                break;
            case 3:
                change_sprite_by_vector(24, 28, (int*)&obsticlePositionX[i], (int*)&obsticlePositionY[i], 0, -1);
                break;
        }
        switch(currentFieldQueue[1][i]){
            case 1:
                change_sprite_by_vector(16, 20, (int*)&obsticlePositionX[i], (int*)&obsticlePositionY[i], 0, -1);
                break;
            case 2:
                change_sprite_by_vector(16, 12, (int*)&obsticlePositionX[i], (int*)&obsticlePositionY[i], 0, -1);
                break;
            case 3:
                change_sprite_by_vector(24, 28, (int*)&obsticlePositionX[i], (int*)&obsticlePositionY[i], 0, -1);
                break;
        }
	}
}

void drawObsticles(uint8_t switchState){
    int i;
    for (i = 0; i < queueMaxSize; i++){

        switch(currentFieldQueue[0][i]){
			case 1:
				display_sprite(16, 20, tallBarrier, (-1*obsticlePositionX[i] + 16*switchState), obsticlePositionY[i], 0);
				break;
			case 2:
				display_sprite(16, 12, shortBarrier, (-1*obsticlePositionX[i] + 16*switchState), obsticlePositionY[i], 0);
				break;
			case 3:
				display_sprite(24, 28, train, (-1*obsticlePositionX[i] -4 + 16*switchState), obsticlePositionY[i], switchState<<8);
				break;
		}
		switch(currentFieldQueue[1][i] & 0x0003){
			case 1:
				display_sprite(16, 20, tallBarrier, (obsticlePositionX[i] + 16*switchState), obsticlePositionY[i], 0);
				break;
			case 2:
				display_sprite(16, 12, shortBarrier, (obsticlePositionX[i] + 16*switchState), obsticlePositionY[i], 0);
				break;
			case 3:
				display_sprite(24, 28, train, (obsticlePositionX[i] -4 + 16*switchState), obsticlePositionY[i], switchState<<8);
				break;
		}
    }
}

//function to assign obsticles to the game field
void fieldGeneration() {
    int i;
    for (i = 0; i < queueMaxSize;i++){ // while(currentFieldQueue[0][4]==0xffff){
        if(currentFieldQueue[0][i]==0xffff){
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
        }else if(obsticlePositionY[i]<0){
            dequeue(0);
            dequeue(1);
        }
    }
}

