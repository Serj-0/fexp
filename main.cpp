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
    curs_set(0);
    set_escdelay(0);
    start_color();
    init_colors();
    
    //TODO load pwd entries
    for(int i = 0; i < 2; i++) add_block();
    
    load_to_block(0, "/");
    load_to_block(1, "/lsrc");

    print_borders();
    print_blocks();

    int c;
    while(1){
        c = getch();
        if(c == 27) break;

        block& bl = blocks[block_selec];
        
        switch(c){
        //MOVE UP
        case 'w':
        case 'k':
        case KEY_UP:
            bl.selec -= (bl.selec > 0) - (bl.files.size() - 1) * (bl.selec <= 0);
            break;
        //MOVE DOWN
        case 's':
        case 'j':
        case KEY_DOWN:
            bl.selec += (bl.selec < bl.files.size() - 1)
                - (bl.selec >= bl.files.size() - 1) * (bl.files.size() - 1);
            break;
        case 'S':
        case 'J':
            block_selec -= block_selec > 0;
            break;
        case 'W':
        case 'K':
            block_selec += block_selec < (block_count - 1);
            break;
        }

        print_borders();
        print_blocks();
        refresh();
    }
    
    endwin();
    return 0;
}
