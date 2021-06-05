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
	set_escdelay(0);
    start_color();
    init_colors();
    
	for(int i = 0; i < 4; i++) add_block();

	for(int i = 0; i < 100; i++) blocks[0].elems.push_back("@@@@@@@@@@@@" + to_string(i));
	for(int i = 0; i < 100; i++) blocks[1].elems.push_back("!!!!!!!!!!!!" + to_string(i));
	//for(int i = 0; i < 100; i++) blocks[2].elems.push_back("$$$$$$$$$$$$" + to_string(i));
	for(int i = 0; i < 100; i++) blocks[3].elems.push_back("************" + to_string(i));

    print_borders();
	print_blocks();

	int c;
	while(1){
		c = getch();
		if(c == 27) break;

		switch(c){
		//MOVE UP
		case 'w':
		case KEY_UP:
			blocks[block_selec].selec -= (blocks[block_selec].selec > 0) - (blocks[block_selec].elems.size() - 1) * (blocks[block_selec].selec <= 0);
			break;
		//MOVE DOWN
		case 's':
		case KEY_DOWN:
			blocks[block_selec].selec += (blocks[block_selec].selec < blocks[block_selec].elems.size() - 1) - (blocks[block_selec].selec >= blocks[block_selec].elems.size() - 1) * (blocks[block_selec].elems.size() - 1);
			break;
		case 'S':
			block_selec -= block_selec > 0;
			break;
		case 'W':
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
