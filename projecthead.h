/* projecthead.h
*/

/* Declare display-related functions from projectfunc.c */
void display_init(void);

/*writes on the canvas a maximum 4 char long string,

the inverter is used to flip the displayed pixels from black to white and vica versa
e.g. if inv=0xC the first two letters will be written in black on white background,
the rest in white on black background*/
void display_string(int line, char *s, char inv);
uint8_t spi_send_recv(uint8_t data);

/* Declare lab-related functions from projectfunc.c */
void projectwork(void);
void quicksleep(int cyc);

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];

/* Declare bitmap array containing small numbers font */
extern const uint8_t const smallNumbers[11*5];

/* Declare bitmap array containing GUI sprite */
extern const uint8_t const gui[36*32];

/*Declare bitmap array containing background graphic 3 frames*/
extern const uint8_t const bg3[90*32/8];                            //NOTE: we ought to check the header size declarations, we got
extern const uint8_t const bg2[90*32/8];                            //into the habit of declaring 8 times the size for bitmaps
extern const uint8_t const bg1[90*32/8];                            //we are wasting memory space, makes flashing process slow!

/*Declare bitmap arrays containing character animations*/
extern const uint8_t const chrRun[2][6*16];
extern const uint8_t const chrJump[4][6*16];
extern const uint8_t const chrRoll[6][6*16];

/*Declare bitmap arrays containing game object sprites*/
extern const uint8_t const coin[20];
extern const uint8_t const shortBarrier[3*16];
extern const uint8_t const tallBarrier[5*16];
extern const uint8_t const train[7*16];


/* Written as part of i/o: getbtns, getsw, enable_interrupt */
int getbtns(void);
uint8_t getsw(void);
void enable_interrupt(void);



//new function declarations for the project
/*displays the canvas on the display, and nothing else*/
void display_upgrade(void);

/*loops through all canvas bytes, sets all to 0*/
void display_clear( void );

/*writes on the canvas a maximum 6 number long integer
int line is tricky, needs some polishing*/
void display_smallNums(int line, int num);

/*Displays a Sprite of size xSize by ySize, with the xOffset and yOffset
In order to work properly the actual sprite x dimension should be divisible 
by 8 (probably, might work otherwise also ¯\\_(ツ)_/¯)

info 16bit int, least 8 bits are for the canvas information 
(that is the 7 bits above the lowest) the other 8 are for flipping x (or y axis, not implemented);
bit N.o. 8 is flip around y axis ((inf >> 7) & 0b1)*/
void display_sprite(int xSize, int ySize, const uint8_t pxlarray[], int xOffset, int yOffset, short inf);

/*we draw and set the reserved background bit to the relevant background; 
we also change the Sprite Data and Sprite type bits of the canvas to represent the background
remember backgrounds are always 32x90, and always placed at 0, 0*/
void display_background(const uint8_t pxlarray[], short inf);

/*we move sprite location on canvas by vector x and vector y 
the remaining artifacts are cleared, and background is shown instead;
this does not draw the sprite, just updates its position!
use this whenever you change the position of a sprite!

note: do not change a sprite's position during an interrupt,
put it on a buffer, and deal with it in main( )*/
void change_sprite_by_vector(int xSize, int ySize, int *x, int *y, int vectorX, int vectorY);

/*Takes in a playercharacter bitmap array;
cuts out the unneeded bits, flips it if needed, and displays it
it also sets the 3rd LSB to the player bits
remember the player sprite size is always 16x24 pixels*/
void display_player(const uint8_t pxlarray[], int xPos, int yPos, short inf);

/*bit map of the whole display, each pixel is the least significant bit of the corresponding char

PLEASE TAKE NOTE OF CANVAS BIT LAYOUT (a canvas[x][y] is 1 char, that is 8 bits)
LSB 0:      Display bit - this bit is the data which is sent by display_upgrade unto the oled display
LSB 1:      Background bit - this bit is reserved for the background, it maps the current background's pixel status in that x,y pos
LSB 2:      Sprite/Layer Data bit
LSB 3-6:    4bit Encoding of what type of sprite/layer is loaded in LSB 2 (Sprite/Layer Data bit)

            0x0: Background
            0x1: Coin Sprite
            0x2: Train Sprite
            0x3: ShortBarrier Sprite
            0x4: TallBarrier Sprite

LSB 7:      Player bit - this is essentially player hitbox, its the cutout player sprite,
            it is also used when player pos is updated by vector to clean artifacts

(remember LSB 7 is the same as MSB 0, the largest bit in a char)*/
extern char canvas[32][128];

extern const uint8_t const titleScreen[128*4];

/*useful 8bit number keeping track of the state of the program

state 1: uhhh
state 2: uh huhhh
state 3: hahahahah
...*/
extern char gamestate;

void menu(void);
void game(void);
void multiplayermenu(void);
void highscores(void);
void gameOver(int score);

//function to randomly generate the game obsticles
void fieldGeneration(void);

//function to seed the random generation
extern inline void srand(int x);

/*contains the current obsticles to be put in game
encoding: LS 2 Bits : 0 - empty space, 01 - tall barrier, 01- short barrier, 11 - train 
MS 14 bits are MS 7 bits obsticle position x, next 7 bits obsticle position y*/
int currentFieldQueue[2][4];

extern char timeToGen;

//sets initial valuse to the currentFieldQueue
void initializeFieldQueue(void);

void moveObsticles(uint8_t switchState);
void applyMoveObsticles(uint8_t swichState);
void drawObsticles(uint8_t switchState);
void calculateFlipObsticleOffset(uint8_t switchState);

//asdasasdssa
extern const uint8_t const cub1[];
extern const uint8_t const cub2[];

extern char cicamica;

/*when 0, no collision, otherwise 1 - collision with obstacle; 2 - collision with coin*/
extern char collisionflag;
/*0 - running, 1 - jumping, 2 - rolling, 3 - switching to other side*/
extern char invincibilitystate;


void highscoreinit( void );
char scorekeepername[9][5];
int scorekeeppoint[9];
void highscoreinit( void );
void appendhighscore(int score, char name[4]);
void displayhighscore( void );
/*when player dies wait two frames then take in a new highscore, then wait until he presses select button, then exit to main*/
void takehighscore( int newscore );

extern uint8_t newFrameFlag;
extern uint8_t buttonmap;

extern int obstaclegeneratorIndex;