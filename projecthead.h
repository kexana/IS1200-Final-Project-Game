/* projecthead.h
*/

/* Declare display-related functions from projectfunc.c */
void display_init(void);
void display_string(int line, char *s, char inv); //new inversion parameter added by Benedek
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

extern const uint8_t const bg3[90*32];
extern const uint8_t const bg2[128*32];
extern const uint8_t const bg1[128*32];


/*Declare bitmap array containing background graphic 3 frames*/
<<<<<<< Updated upstream
const uint8_t const background[3 * 128 * 8];
=======
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

>>>>>>> Stashed changes

/* Written as part of i/o: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);



//new function declarations for the project
/*displays the canvas on the display, and nothing else*/
void display_upgrade(void);

/*display data all clear, also canvas*/
void display_clear( void );

void display_smallNums(int line, int num);
void display_sprite(int xSize, int ySize, const uint8_t pxlarray[], int xOffset, int yOffset, short inf);
<<<<<<< Updated upstream
//void display_gui(void);
=======

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
>>>>>>> Stashed changes

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

extern char gamestate;

void menu(void);
void game(void);
void multiplayermenu(void);
void highscores(void);

//asdasasdssa
extern const uint8_t const cub1[];
extern const uint8_t const cub2[];

extern char cicamica;