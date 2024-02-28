/* projectgameGeneration.c
*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "projecthead.h"  /* Declatations for the project */

uint8_t queueHead[] ={-1, -1};
uint8_t queueTail[] ={-1, -1};

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
        }
    }
// lane 0 = left; 1=right, value = type of obstcle 0=empty, 1=tall barrier, 2=short barrier, 3=train
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

// Linear congruantial Generator
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
    }
    queueHead[0] = -1;
    queueHead[1] = -1;
    queueTail[0] = -1;
    queueTail[1] = -1;
}

//function to assign obsticles to the game field
void fieldGeneration() {
    int i;
    for (i = 0; i < 4;i++){ // while(currentFieldQueue[0][4]==255){
        int randValue =rand();
        int randLeft = randValue&0xff;
        int randRight = (randValue&0xfff00)>>8;
        
        char trainFlag=0;
        //printf("\n Left Side: \n");
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
        //printf("\n Right Side: \n");
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
    }

}

