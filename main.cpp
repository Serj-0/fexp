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
    cout << valid("/lost+found") << "\n";
    
    
    return 0;
    //
    win = initscr();
    noecho();
    keypad(win, 1);
    curs_set(0);
    set_escdelay(0);
    start_color();
    init_colors();
    
    //TODO load pwd entries
    for(int i = 0; i < 10; i++) add_block();

    print_borders();
    print_blocks();

    int c;
    while(1){
        c = getch();
        if(c == 27) break;

        switch(c){
        //MOVE UP
        case 'w':
        case 'k':
        case KEY_UP:
            blocks[block_selec].selec -= (blocks[block_selec].selec > 0) - (blocks[block_selec].elems.size() - 1) * (blocks[block_selec].selec <= 0);
            break;
        //MOVE DOWN
        case 's':
        case 'j':
        case KEY_DOWN:
            blocks[block_selec].selec += (blocks[block_selec].selec < blocks[block_selec].elems.size() - 1)
                - (blocks[block_selec].selec >= blocks[block_selec].elems.size() - 1) * (blocks[block_selec].elems.size() - 1);
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
