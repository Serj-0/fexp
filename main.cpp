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

	int x = 1;
	for(int i = 0; i < 100; i++) blocks[x].elems.push_back("@@@@@@@@@@@@" + to_string(i));

    print_borders();
	print_elements(x);

	int c;
	while(1){
		c = getch();
		if(c == 27) break;

		switch(c){
		case 'w':
			blocks[x].selec -= (blocks[x].selec > 0) - (blocks[x].elems.size() - 1) * (blocks[x].selec <= 0);
			break;
		case 's':
			blocks[x].selec += (blocks[x].selec < blocks[x].elems.size() - 1) - (blocks[x].selec >= blocks[x].elems.size() - 1) * (blocks[x].elems.size() - 1);
			break;
		}

		print_borders();
		print_elements(x);
		refresh();
	}
    
    endwin();
    return 0;
}
