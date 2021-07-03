/* * FEXP * */
#include "sys/stat.h"
#include "pwd.h"
#include <iostream>
#include <csignal>
#include "ncurses.h"
#include "boost/filesystem.hpp"
#include "fexpmicro.h"
#include "fexpsearch.h"
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
    
    //load pwd or from args
    if(argc > 1){
        for(int i = 1; i < argc; i++){
            add_block();
            load_to_block(i - 1, args[i]);
        }
    }else{
        add_block();
        load_to_block(0, current_path());
    }

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
        case 'A':
        case 'H':
        case KEY_SLEFT:
            block_selec -= block_selec > 0;
            break;
        case 'D':
        case 'L':
        case KEY_SRIGHT:
            block_selec += block_selec < (block_count - 1);
            break;
        case 'd':
        case 'l':
        case KEY_RIGHT:
        case KEY_ENTER:
            enter_selected_directory();
            break;
        case 'a':
        case 'h':
        case KEY_LEFT:
//            load_to_block(block_selec, bl.directory.parent_path());
            exit_directory(block_selec);
            break;
        case KEY_NPAGE:
            bl.selec = min((unsigned long) bl.selec + block_height(), bl.files.size() - 1);
            break;
        case KEY_PPAGE:
            bl.selec = max(0, (int) bl.selec - (block_height()));
            break;
        case KEY_HOME:
            bl.selec = 0;
            break;
        case KEY_END:
            bl.selec = bl.files.size() - 1;
            break;
        case 'E':
            if(selected_entry()){
                if(selected_entry()->status == UNREADABLE_DIRECTORY){
                    set_err_msg("Not Readable!");
                }else if(selected_entry()->status != READABLE_DIRECTORY){
                    set_err_msg("Not a Directory!");
                }else{
                    add_block();
                    load_to_block(block_count - 1, selected_entry()->entry.path());
                    block_selec = block_count - 1;
                }
            }else{
                set_err_msg("No Selected Entry!");
            }
            break;
        case 'C':
            if(block_count > 1) close_block(block_selec);
            break;
        case ' ':
            string_prompt("", win->_maxy, search_in_files);
            break;
        }
        
        //TODO create, delete, move files, etc.
        
        print_borders();
        print_blocks();
        refresh();
    }
    
    endwin();

    return 0;
}
