#ifndef FEXPBLOCK_H
#define FEXPBLOCK_H
#include <vector>
#include "fexpmicro.h"
using namespace std;

int block_count;
int block_selec;

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
    printw("%s", string(win->_maxx + 1, '.').c_str());
    
    string brd = ".";
    for(int i = block_count; i--;){
        brd.append(string(win->_maxx / block_count - (!i), ' '));
        brd.append(".");
    }
    
    //remove extra spaces
    int d = brd.size() - (win->_maxx + 1);
    if(brd.size() > win->_maxx + 1) brd.erase(brd.size() - (d + 1), d);
    
    int y = 0;
    while(++y < win->_maxy - 1){
        mvprintw(y, 0, "%s", brd.c_str());
    }

    printw("%s", string(win->_maxx + 1, '.').c_str());
}

#endif /* FEXPBLOCK_H */

