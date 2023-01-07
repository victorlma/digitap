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

typedef struct gmst {
    char    ch;

} gmst_t ;


char* readWordList()
{
   int fdWordList = open(wordListPath, O_RDONLY);
   struct stat statWordList;
   stat(wordListPath, &statWordList);
   char* wordList = malloc(statWordList.st_size);
   read(fdWordList, wordList, statWordList.st_size);

   return wordList;
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
