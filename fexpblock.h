#ifndef FEXPBLOCK_H
#define FEXPBLOCK_H
#include <vector>
#include "fexpmicro.h"
using namespace std;

int block_count;
int block_selec;
char border_char = '~';
int high_bar_size = 0;
int low_bar_size = 1;

struct block{
    int id;
    int selec;
    vector<string> elems;
};

vector<block> blocks;

void add_block(){
    blocks.push_back({block_count++, 0});
}

void print_borders(){
    mvprintw(high_bar_size, 0, "%s", string(win->_maxx + 1, border_char).c_str());
    
	string brd = string(win->_maxx + 1, ' ');
	brd[0] = border_char;
	brd[brd.size() - 1] = border_char;

	int w = win->_maxx / block_count;

	for(int i = 1; i < block_count; i++){
		brd[w * i] = border_char;
	}

    int y = high_bar_size;
    while(++y < win->_maxy - low_bar_size){
        mvprintw(y, 0, "%s", brd.c_str());
    }
	
	//TODO block selection indicator
    printw("%s", string(win->_maxx + 1, border_char).c_str());
}

void print_constrained(string str, string trunc, int maxlen){
	//TODO impl begining or ending truncation
	if(str.size() > maxlen) str = str.substr(0, maxlen - trunc.size()).append(trunc);
	printw(str.c_str());
}

void print_elements(int id){
	if(blocks[id].elems.size() == 0) return;
	block& bl = blocks[id];
	int i = high_bar_size;
	int w = win->_maxx / block_count;

	int bh = win->_maxy - high_bar_size - low_bar_size - 1;
	int it = blocks[id].selec / bh;

	for(int e = it * bh; e < (it + 1) * bh; e++){
		move(i + 1, bl.id * w + 1);
		if(e == blocks[id].selec) attron(COLOR_PAIR(PAIR_SELEC));
		print_constrained(blocks[id].elems[e], "...", w - 1);
		attrset(A_NORMAL);
		if(++i >= win->_maxy - (low_bar_size + 1)) break;
	}
}

void print_blocks(){
	for(int i = 0; i < block_count; i++){
		print_elements(i);
	}
}

#endif /* FEXPBLOCK_H */

