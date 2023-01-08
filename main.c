#include "stdlib.h"
#include "stdbool.h"
#include "time.h"
#include "ncurses.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/stat.h"

#define minX    40
#define minY    20
#define wordListPath    "word.list"

int    winX, winY;

typedef enum {
    MENU,
    GAME,
    MODES_COUNT
} modes;

typedef struct wordList {
    size_t  size;
    char    *string;
    size_t  posSize;
    size_t  *positions;
    size_t  cwBegin;
    size_t  cwEnd;
} wordList_t;

typedef struct gmst {
    int    ch;
    modes     mode;
    bool     isInit;
    wordList_t words;
} gmst_t ;

bool makeWordList(wordList_t *words)
{
   int fdWordList = open(wordListPath, O_RDONLY);
   struct stat statWordList;
   stat(wordListPath, &statWordList);

   words->size = statWordList.st_size;
   words->string = malloc(words->size);
   read(fdWordList, words->string, words->size);
   close(fdWordList);
   words->posSize = 1;

   for (size_t i = 0; i < words->size; ++i){

       if (words->string[i] == '\n'){

           words->posSize += 1;
       }
   }

   words->positions = malloc(words->posSize*sizeof(size_t));
   words->positions[0] = 0;
   size_t posCursor = 1;

   for (size_t i = 0; i < words->size; ++i){

       if (words->string[i] == '\n'){

           words->positions[posCursor] = i;
           ++posCursor;
       }
   }


   return true;
}

int checkScrSize()
{
        getmaxyx(stdscr, winY,winX);
        if (winX < minX || winY < minY)
        {
            clear();
            mvprintw(0, 0, "min scr size: %d x %d",minX, minY);
            return false;
        }

        return true;
}


void processMenu(gmst_t *game)
{
    if (game->isInit){
            game->ch = getch();
    }
    if(!game->isInit){
        game->ch = 0;
        game->isInit = true;
    }
    if (game->ch == '\n') {
        game->mode = GAME;
        game->isInit = false;
    }
}

void drawMenu()
{
    clear();
    mvprintw(0, 20,"Press ENTER to Begin");
}

void processGame(gmst_t *game)
{
    if (game->isInit){
            game->ch = getch();
    }
    if(!game->isInit){
        game->ch = 0;
        game->isInit = true;
    }
    if (game->ch == '\n') {
        game->mode = MENU;
        game->isInit = false;
    }
}

void drawGame(gmst_t *game)
{
    clear();
    mvprintw(0, 20, "GAME: %c",game->ch);
}
int main(void)
{
    bool shouldClose = false;

    gmst_t  game = {
        .ch = 0,
        .mode = MENU,
        .isInit = false
    };


    if(!makeWordList(&game.words)){

    }

    initscr();
    cbreak();
    noecho();
    while(!shouldClose){

        if (game.ch == 27){
            shouldClose = 1;
        }

        if (checkScrSize())
        {
            switch(game.mode){
                case MENU:
                    processMenu(&game);
                    drawMenu();
                    break;

                case GAME:
                    processGame(&game);
                    drawGame(&game);
                    break;
                default:
                    break;
            }
        }
        refresh();
    }
    endwin();
    return 0;
}
