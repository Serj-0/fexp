#ifndef FEXPMICRO_H
#define FEXPMICRO_H
#include "sys/stat.h"
#include "pwd.h"
#include <vector>
#include <fstream>
#include <ncurses.h>
#include "boost/filesystem.hpp"
using namespace std;
using namespace boost::filesystem;

const int 
        PAIR_NONE = 1,
        PAIR_SELEC = 2,
        PAIR_LINK = 3,
        PAIR_LINK_SELEC = 4,
        PAIR_EXE = 5,
        PAIR_EXE_SELEC = 6,
        PAIR_ERR = 7,
        PAIR_ERR_SELEC = 8;

void init_colors(){
    init_pair(PAIR_NONE, COLOR_WHITE, COLOR_BLACK);
    init_pair(PAIR_SELEC, COLOR_BLACK, COLOR_BLUE);
    init_pair(PAIR_LINK, COLOR_CYAN, COLOR_BLACK);
    init_pair(PAIR_LINK_SELEC, COLOR_BLACK, COLOR_CYAN);
    init_pair(PAIR_EXE, COLOR_YELLOW, COLOR_BLACK);
    init_pair(PAIR_EXE_SELEC, COLOR_BLACK, COLOR_YELLOW);
    init_pair(PAIR_ERR, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_ERR_SELEC, COLOR_WHITE, COLOR_RED);
}

struct dir_file{
    directory_entry entry;
    struct stat fstat;
    bool owned;
};

WINDOW* win = nullptr;

#endif /* FEXPMICRO_H */
