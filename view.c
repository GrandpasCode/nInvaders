#include "view.h"
#include "globals.h"
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>

#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define CYAN 5
#define MAGENTA 6
#define WHITE 7

WINDOW *wBattleField;
WINDOW *wEmpty;
WINDOW *wScores;	

WINDOW *wPlayer;
WINDOW *wPlayerMissile;
WINDOW *wAliens;
WINDOW *wAliensMissile;	
WINDOW *wBunkers;
WINDOW *wGameOver;
WINDOW *wUfo;

/**
 * initialize player sprites
 */
static void playerInit()
{
	wPlayer = newpad(1, PLAYERWIDTH);	// new pad with appropriate size
	wclear(wPlayer);			// clear pad
        wattrset(wPlayer,COLOR_PAIR(YELLOW));	// set color
        waddstr(wPlayer,"/-^-\\");	// set sprite
}


/**
 * display player sprite
 */
void playerDisplay(int x, int y) 
{
	copywin(wPlayer,wBattleField,0,0,y,x,y,x+PLAYERWIDTH-1,0);
}


/**
 * clear player sprite
 */
void playerClear(int x, int y) 
{
	copywin(wEmpty,wBattleField,0,0,y,x,y,x+PLAYERWIDTH-1,0);
}


/**
 * initialize missile sprites
 */
static void playerMissileInit()
{
	wPlayerMissile = newpad(1, 1);		// new pad with appropriate size
	wclear(wPlayerMissile);			// clear pad
	wattrset(wPlayerMissile,COLOR_PAIR(WHITE));	// set color
	waddch(wPlayerMissile,'!');		// set sprite
	wrefresh(wPlayerMissile);
}


/**
 * display missile sprite
 */
void playerMissileDisplay(int x, int y) 
{
	copywin(wPlayerMissile,wBattleField,0,0,y,x,y,x,0);
}


/**
 * clear missile sprite
 */
void playerMissileClear(int x, int y) 
{
	copywin(wEmpty,wBattleField,0,0,y,x,y,x,0);
}


/**
 * some explosion animation
 */
void playerExplode(int x, int y)
{
	WINDOW* wPlayerExplosion;
	char playerExplosionChars[16+1]="@~`.,^#*-_=\\/%{}";
	int t,s;
	
	wPlayerExplosion=newpad(1,PLAYERWIDTH);		// new pad
	wattrset(wPlayerExplosion,COLOR_PAIR(YELLOW));	// set color
	
	for(t=0;t<5;t++){ 			// 5 frames
		wclear(wPlayerExplosion);	// clear pad
		for(s=0;s<PLAYERWIDTH;s++){
			waddch(wPlayerExplosion,playerExplosionChars[rand()%16]);	// sprite
		}

		copywin(wPlayerExplosion,wBattleField,0,0,y,x,y,x+PLAYERWIDTH-1,0); 	// display explostion
		wrefresh(wBattleField); 	// refresh battelfield to display explosion frames
		doSleep(100000);		// play animation not too fast
	}
	

} // todo: kann man bestimmt noch besser machen.


/**
 * initialize aliens sprites
 */
static void aliensInit()
{
	wAliens = newpad(ALIENS_MAX_NUMBER_Y*2,ALIENS_MAX_NUMBER_X*3);
	wclear(wAliens);
}


/**
 * display aliens sprite
 */
void aliensDisplay(int x, int y, int wid, int hgt) 
{
	copywin(wAliens,wBattleField,0,0,y,x,y+hgt,x+wid+2,0);
}


/**
 * clear aliens sprite
 */
void aliensClear(int x, int y, int wid, int hgt) 
{
	copywin(wEmpty,wBattleField,0,0,y,x,y+hgt,x+wid+2,0);
}


/**
 * initialize missile sprites
 */
static void aliensMissileInit()
{
	wAliensMissile = newpad(1, 1);		// new pad
	wclear(wAliensMissile);			// clear pad
	wattrset(wAliensMissile, COLOR_PAIR(CYAN));	// set color
	waddch(wAliensMissile, ':');			// set sprite
}


/**
 * display missile sprite
 */
void aliensMissileDisplay(int x, int y) 
{
	copywin(wAliensMissile,wBattleField,0,0,y,x,y,x,0);
}


/**
 * clear missile sprite
 */
void aliensMissileClear(int x, int y) 
{
	copywin(wEmpty,wBattleField,0,0,y,x,y,x,0);
}


/**
 * refresh aliens sprite
 */
void aliensRefresh(int level) 
{
	static int frame = 0; // used for animation; mod 2 == 0: frame1, mod2 == 1: frame2
	int k,row;
	int c = 0;
	int alienType = 0;
	char ships[2][9][3+1] = {
		{",^,", "_O-", "-o-",  "o=o", "<O>", "_x_", "*^*", "\\_/", "o o"},
		{".-.", "-O_", "/o\\", "o-o", "<o>", "-x-", "o^o", "/~\\", "oo "}
	};
	int colors[9] = {RED, GREEN, BLUE, RED, YELLOW, WHITE, WHITE, YELLOW, RED};

	wclear(wAliens);						// clear pad
	wattrset(wAliens,COLOR_PAIR(RED));				// set color
	
	frame++;						// next frame
	
	// draw alien if there is one
	for (row = 0; row < ALIENS_MAX_NUMBER_Y*2; row++) {			
		for (k=0;k<ALIENS_MAX_NUMBER_X;k++) {
			if ((row % 2) == 0) {			// display aliens every even row
				alienType=alienBlock[c][k]; 	// get type of alien
				
				if (alienType != 0) {		// if there is an alien to display
					wattrset(wAliens,COLOR_PAIR(colors[alienType-1]));		  // set color
					waddch(wAliens,ships[frame%2][alienType-1+(3*((level-1)%3))][0]);  // set char1
					waddch(wAliens,ships[frame%2][alienType-1+(3*((level-1)%3))][1]);  // set char2
					waddch(wAliens,ships[frame%2][alienType-1+(3*((level-1)%3))][2]);  // set char3
					if (alienType>4) {alienBlock[c][k]=(alienType+1)%9;} // todo: what's that? If alien_type > 4 then do a modulo operation???
				} else {
					waddstr(wAliens,"   ");	// no alien
				}
				
			} else {
				waddstr(wAliens,"   ");		// no aliens at odd rows
			}
		}
		if ((row % 2) == 1) {c++;} // goto next row at alienblock
	}
}


/**
 * initialize bunkers sprites
 */
static void bunkersInit()
{
	wBunkers = newpad(BUNKERHEIGHT, BUNKERWIDTH);		// new pad data
	wclear(wBunkers);
}


/**
 * display bunkers sprite
 * needs pointer to bunker-array
 */
void bunkersDisplay(int *pBunker) 
{
	int l, k;
	wclear(wBunkers);
	wattrset(wBunkers,COLOR_PAIR(CYAN));
	for (l=0;l<BUNKERHEIGHT;l++) {
		for (k=0;k<BUNKERWIDTH;k++) {
			if (*(pBunker + (l * (BUNKERWIDTH + 1)) + k) == 1) {	//if (pBunker[l][k]==1) {
				waddch(wBunkers,'#');
			} else {
				waddch(wBunkers,' ');
			}
		}	
	}
	
	copywin(wBunkers, wBattleField, 0, 0, BUNKERY, BUNKERX, BUNKERY + BUNKERHEIGHT - 1, BUNKERX + BUNKERWIDTH - 1, 0);
}


/**
 * clear bunkers sprite
 */
void bunkersClear() 
{
	copywin(wEmpty, wBattleField, 0, 0, BUNKERY, BUNKERX, BUNKERY + BUNKERHEIGHT - 1, BUNKERX + BUNKERWIDTH - 1, 0);
}


/**
 * clear one element of bunkers sprite at position (x, y)
 */
void bunkersClearElement(int x, int y) 
{
	copywin(wEmpty, wBattleField, 0, 0, y, x, y, x, 0);
}


/**
 * set actual sprite for ufo animation
 */
void ufoRefresh()
{
	char ufo[4][5] = {"<o o>", "<oo >", "<o o>", "< oo>"};
	static int frame = 0;

	wclear(wUfo);
        wattrset(wUfo,COLOR_PAIR(RED));
	waddstr(wUfo, ufo[frame % 4]);

	frame++;
}


/**
 * initialize ufo sprite
 */
static void ufoInit()
{
	wUfo = newpad(1, UFOWIDTH);	 // new pad with appropriate size
	wclear(wUfo);    		 // clear pad
        wattrset(wUfo,COLOR_PAIR(RED));	 // set color
}


/**
 * display ufo sprite
 */
void ufoDisplay(int x, int y)
{
	copywin(wUfo,wBattleField,0,0,y,x,y,x+UFOWIDTH-1,0);
}


/**
 * clear ufo sprite
 */
void ufoClear(int x, int y) 
{
	copywin(wEmpty,wBattleField,0,0,y,x,y,x+UFOWIDTH-1,0);
}


/**
 * initialize gameover graphic
 */
static void gameOverInit()
{
	// init game-over banner
	wGameOver = newpad(13, 31);
	wclear(wGameOver);
	wattrset(wGameOver, COLOR_PAIR(7));
	waddstr(wGameOver, "                               ");
	waddstr(wGameOver, "  #####   ####  ##   ## ###### ");
	waddstr(wGameOver, " ##      ##  ## ####### ##     ");
	waddstr(wGameOver, " ## ###  ###### ## # ## #####  ");
	waddstr(wGameOver, " ##  ##  ##  ## ##   ## ##     ");
	waddstr(wGameOver, "  #####  ##  ## ##   ## ###### ");
	waddstr(wGameOver, "                               ");
	waddstr(wGameOver, "  ####  ##   ## ###### ######  ");
	waddstr(wGameOver, " ##  ## ##   ## ##     ##   ## ");
	waddstr(wGameOver, " ##  ##  ## ##  #####  ######  ");
	waddstr(wGameOver, " ##  ##  ## ##  ##     ##  ##  ");
	waddstr(wGameOver, "  ####    ###   ###### ##   ## ");
	waddstr(wGameOver, "                               ");
}

/**
 * display gameover graphic
 */ 
void gameOverDisplay(int x, int y)
{
	copywin(wGameOver, wBattleField, 0, 0, y, x, y + 12, x + 30, 0);
	wrefresh(wBattleField);
}


/**
 * display scores
 */
void statusDisplay(int level, int score, int lives)
{
	int t, xOffset;
	xOffset = (SCREENWIDTH / 2) - 24;
	
	attrset(COLOR_PAIR(RED));			// set color
	
	// display level, score and lives
	mvprintw(SCREENHEIGHT - 1, xOffset, "Level: %2.2d   Score: %7.7ld   Lives: ", level, score);
	for (t = 0; ((t < 5) && (t < lives)); t++){
		mvprintw(SCREENHEIGHT - 1, xOffset + 36 + (4 * t), "/-\\ ");
	}
	for (; t < 5; t++){
		mvprintw(SCREENHEIGHT - 1, xOffset + 36 + (4 * t), "    ");
	}
}

/**
 * initialize battlefield
 */
static void battleFieldInit()
{
	wEmpty = newpad(SCREENHEIGHT -1, SCREENWIDTH);
	wclear(wEmpty);
	
	wBattleField = newwin(SCREENHEIGHT - 1, SCREENWIDTH, 0, 0);	// new window
	wclear(wBattleField);						// clear window
	mvwin(wBattleField, 0, 0);					// move window
}

/**
 * refresh screen so that modified graphic buffers get visible
 */
void refreshScreen()
{
	redrawwin(wBattleField); // needed to display graphics properly at startup on some terminals
	wrefresh(wBattleField);

}	

/**
 * do proper cleanup
 */
static void finish(int sig)
{
	endwin();	// <curses.h> reset terminal into proper non-visual mode
	exit(0);
}

/**
 * initialize n_courses
 */
void graphicEngineInit()
{
	(void) signal(SIGINT, finish);	// <signal.h> on signal "SIGINT" call method "finish"
	(void) initscr();		// <curses.h> do initialization work 
	keypad(stdscr, TRUE);		// <curses.h> enable keypad for input
	(void) nonl();			// <curses.h> disable translation return/ newline for detection of return key
	(void) cbreak();		// <curses.h> do not buffer typed characters, use at once
	(void) noecho();		// <curses.h> do not echo typed characters
	start_color();			// <curses.h> use colors
	init_pair(RED, COLOR_RED, COLOR_BLACK);		// <curses.h> define color-pair
	init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);	// <curses.h> define color-pair
	init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);	// <curses.h> define color-pair
	init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);	// <curses.h> define color-pair
	init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);	// <curses.h> define color-pair
	init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);	// <curses.h> define color-pair
	init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);	// <curses.h> define color-pair
	
	timeout(0); 			// <curses.h> do not wait for input

	// initialize sprites 
	battleFieldInit();
	playerInit();
	playerMissileInit();
	aliensInit();
	aliensMissileInit();
	bunkersInit();
	ufoInit();
	gameOverInit();

}

