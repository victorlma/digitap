#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "time.h"
#include "ncurses.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/stat.h"

#define minX    40
#define minY    10
#define wordListPath    "word.list"

int    winX, winY;
int    centerX, centerY;

typedef enum {
    MENU,
    GAME,
    MODES_COUNT
} modes;

typedef enum {
    CORRECT = 1,
    WRONG,
    PAIRS_CONT
} pairs;

typedef struct wordList {
    size_t  size;
    char    *string;
    size_t  posSize;
    size_t  *positions;
    size_t  cwBegin;
    size_t  cwEnd;
    size_t  cwCursor;
} wordList_t;

typedef struct gmst {
    int     ch;
    int     mode;
    int     score;
    int     tries;
    bool     isInit;
    bool     firstType;
    bool     redoWord;
    wordList_t words;
} gmst_t ;

void randomSetup()
{
    time_t clck = time(NULL);
    srand(clck);

}

bool makeWordList(wordList_t *words)
{
    int fdWordList = open(wordListPath, O_RDONLY);
    if (fdWordList > -1)
    {
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
    {
        return false;
    }
}

int checkScrSize()
{
        getmaxyx(stdscr, winY,winX);
        centerX = winX/2;
        centerY = winY/2;
        if (winX < minX || winY < minY)
        {
            clear();
            mvprintw(0+centerY, 0+centerX-10, "min scr size: %d x %d",minX, minY);
            return false;
        }

        return true;
}


void pcolorword(int y, int x, int color, char word)
{
    attron(COLOR_PAIR(color)|A_BOLD|A_STANDOUT);
    mvprintw(y, x,"%c",word);
    attroff(COLOR_PAIR(color)|A_BOLD|A_STANDOUT);
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
    mvprintw(0+centerY, centerX-10,"Press ENTER to Begin");
}


void setupNextWord(wordList_t *words)
{
    int wBegin = rand() % words->posSize;
    wBegin = wBegin % 2 == 0 ? wBegin : wBegin -1;
    words->cwBegin = words->positions[wBegin] == 0 ? words->positions[wBegin] : words->positions[wBegin] +1;
    words->cwEnd = words->positions[wBegin +1];
    words->cwCursor = words->cwBegin;
}

void processGame(gmst_t *game)
{
    if (game->isInit){
        game->ch = getch();
        if (game->redoWord){
            game->redoWord = false;
            game->words.cwCursor = game->words.cwBegin;
        }
        if (game->firstType) game->firstType = false;

        if (game->ch == game->words.string[game->words.cwCursor]){
            if (game->words.cwCursor == game->words.cwEnd -1){
                game->isInit = false;
                game->score += 100;

            }
            else {
                ++game->words.cwCursor;
                game->firstType = true;
            }
        }
        else {
            if (game->tries > 0){
                game->redoWord = true;
                game->score -=25;
                game->tries -= 1;
            }
            else{
                game->isInit = false;
            }
        }

    }
    if(!game->isInit){
        game->ch = 0;
        game->firstType = true;
        setupNextWord(&game->words);
        game->isInit = true;
        game->redoWord = false; 
        game->tries = 4;
    }
    if (game->ch == '\n') {
        game->isInit = false;
    }
}

void drawGame(gmst_t *game)
{
    clear();
    mvprintw(0+centerY-4, 0+centerX-14, "You Pressed: %c",game->ch);
    mvprintw(2+centerY-4, 0+centerX-14, "Tries Left in this Word: %d", game->tries);
    mvprintw(4+centerY-4, 0+centerX-14, "Score: %d", game->score);


    int xRef = -(game->words.cwEnd - game->words.cwBegin)/2;
    int xReal;
    for (int c = game->words.cwBegin; c < game->words.cwEnd; ++c)
    {
        char cChar = game->words.string[c];
        xReal = xRef + (c - game->words.cwBegin);
        int wpCursor = game->words.cwCursor;

        if (game->isInit)
        {
            if (c > wpCursor || (c == wpCursor && game->firstType))
            {
                mvprintw(7+centerY-4, xReal+centerX, "%c", cChar);
            }
            else if (c == wpCursor && game->ch != cChar)
            {
                pcolorword(7+centerY-4, xReal+centerX, WRONG,cChar);
            }
            else
            {
                pcolorword(7+centerY-4, xReal+centerX, CORRECT,cChar);
            }
        }
        else
        {
            mvprintw(7+centerY-4,xReal+centerX-10, "%c", cChar);
        }
    }
}

int main(void)
{
    randomSetup();
    bool shouldClose = false;

    gmst_t  game = {
        .ch = 0,
        .mode = MENU,
        .isInit = false
    };


    if(!makeWordList(&game.words)){
        printf("Could not Read Word List\n");
        printf("Make sure a valid word.list file exists\n");
        return -1;

    }

    initscr();
    cbreak();
    noecho();
    raw();
    start_color();
    init_pair(CORRECT, COLOR_GREEN, COLOR_BLACK);
    init_pair(WRONG, COLOR_RED, COLOR_BLACK);


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
        curs_set(0);
        refresh();
    }
    endwin();
    return 0;
}
