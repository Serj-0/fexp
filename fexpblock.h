#ifndef FEXPBLOCK_H
#define FEXPBLOCK_H
#include <vector>
#include "fexpmicro.h"
using namespace std;

unsigned int block_count;
unsigned int block_selec;
char border_char = '~';
char block_selec_char = '^';
unsigned int high_bar_size = 1;
unsigned int low_bar_size = 1;

struct block{
    unsigned int id;
    unsigned int selec;

    vector<string> elems;

    path directory;
    vector<dir_file> files;
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
	
    string lowbar = string(block_selec * w + 1, border_char) + string(w - 1, block_selec_char);
    lowbar.append(string(win->_maxx + 1 - lowbar.size(), border_char));

    printw("%s", lowbar.c_str());
}

void print_constrained(string str, string trunc, unsigned int maxlen, bool front = false){
    if(str.size() > maxlen){
        if(front){
            str = trunc.append(str.substr(trunc.size(), maxlen - trunc.size()));
        }else{
            str = str.substr(0, maxlen - trunc.size()).append(trunc);
        }
    }

    printw(str.c_str());
}

void print_elements(int id){
    if(blocks[id].files.size() == 0) return;
    
    block& bl = blocks[id];
    int i = high_bar_size;
    int w = win->_maxx / block_count;

    int bh = win->_maxy - high_bar_size - low_bar_size - 1;
    int it = bl.selec / bh;

    for(int e = it * bh; e < (it + 1) * bh; e++){
        if(e > bl.files.size() - 1) return;
        
        move(i + 1, bl.id * w + 1);
        
        int atrind = 1 + (e == bl.selec);
        if(bl.files[e].status < 1){
            atrind += PAIR_ERR - 1;
        }else{
            atrind += (bl.files[e].link << 1);
        }
        
        attron(COLOR_PAIR(atrind));
        print_constrained(bl.files[e].entry.path().filename().string() + (abs(bl.files[e].status) == READABLE_DIRECTORY ? "/" : "")
                , "...", w - 1, false);
        attrset(A_NORMAL);
        if(++i >= win->_maxy - (low_bar_size + 1)) break;
    }
}

//TODO top and bottom bar info
void print_blocks(){
    for(int i = 0; i < block_count; i++){
        print_elements(i);
    }
}

void load_to_block(int id, path p){
    if(valid(p) < 1) return;
    
    blocks[id].directory = p;
    blocks[id].files = load_directory_files(p);
}

//TODO directory navigation functions

#endif /* FEXPBLOCK_H */

