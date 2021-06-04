/* * FEXP * */
#include "sys/stat.h"
#include "pwd.h"
#include <iostream>
#include <csignal>
#include "ncurses.h"
#include "boost/filesystem.hpp"
#include "fexpmicro.h"
#include "fexpconfig.h"
#include "fexpblock.h"
using namespace std;
using namespace boost::filesystem;

int main(int argc, char** args){
    win = initscr();
    noecho();
    keypad(win, 1);
    start_color();
    init_colors();
    
    print_borders();
    getch();
    
    endwin();
    return 0;
}