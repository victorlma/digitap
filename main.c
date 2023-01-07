#include "stdlib.h"
#include "ncurses.h"
#include "stdbool.h"
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

typedef struct gmst {
    int    ch;
    modes     mode;
    bool     isInit;

} gmst_t ;

typedef struct wordList {
    size_t  size;
    char    *string;
    size_t  posSize;
    size_t  *positions;
} wordList_t;

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

int main(void)
{

    bool shouldClose = false;

    gmst_t  game = {
        .ch = 0
    };
    char* wordList = readWordList();
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    while(!shouldClose){
        
        game.ch = getch();
        if (game.ch == 27){
            shouldClose = 1;
        }

        if (checkScrSize())
        {
            clear();
        }
        refresh();
    }
    endwin();
    return 0;
}
