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
const uint8_t const background[3 * 128 * 8];

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
//void display_gui(void);

/*bit map of the whole display, each pixel is the least significant bit of the corresponding char*/
extern char canvas[32][128];

extern char gamestate;

void menu(void);
void game(void);
void multiplayermenu(void);
void highscores(void);

//asdasasdssa
extern const uint8_t const cub1[];
extern const uint8_t const cub2[];