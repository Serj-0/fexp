#ifndef FEXPBLOCK_H
#define FEXPBLOCK_H
#include <vector>
#include "fexpmicro.h"
using namespace std;

unsigned int block_count;
unsigned int block_selec;
char border_char = '#';
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
    int w = win->_maxx / block_count;
    
    move(high_bar_size, 0);

    mvprintw(high_bar_size, 0, "%s", string(win->_maxx + 1, border_char).c_str());
    
    for(int i = 0; i < block_count; i++){
        move(high_bar_size, w * i);
        string bldir = _constrained(blocks[i].directory.string(), "...", w - 1, true);
        attron(COLOR_PAIR(PAIR_SELEC));
        _print_constrained(bldir, "...", w - 1, true);
    }
    attrset(A_NORMAL);
    
    string brd = string(win->_maxx + 1, ' ');
    brd[0] = border_char;
    brd[brd.size() - 1] = border_char;

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

void print_filename(dir_file file, int id, print_constraint cst = {-1}, int e = -1){
    block& bl = blocks[id];
    
    int atrind = 1 + (e == bl.selec);
    if(file.status < 1){
        atrind += PAIR_ERR - 1;
    }else{
        atrind += (file.islink << 1);
    }

    attron(COLOR_PAIR(atrind));
    print_constrained(file.entry.path().filename().string() + (abs(file.status) == READABLE_DIRECTORY ? "/" : ""), cst);
    attrset(A_NORMAL);
}

void print_filelink(dir_file_link file, print_constraint cst = {-1}, bool selec = false){
    int atrind = 1 + selec;
    if(file.status < 1){
        atrind += PAIR_ERR - 1;
    }

    attron(COLOR_PAIR(atrind));
    print_constrained(file.pth.string() + (abs(file.status) == READABLE_DIRECTORY ? "/" : ""), cst);
    attrset(A_NORMAL);
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
        
        print_filename(bl.files[e], id, {w - 1, "...", false}, e);
        if(++i >= win->_maxy - (low_bar_size + 1)) break;
    }
}

void print_lowbar(){
    block& bl = blocks[block_selec];
    move(win->_maxy, 0);
    clrtoeol();
    
    if(bl.files.size() > 0){
        dir_file& sfile = bl.files[bl.selec];
        print_filename(sfile, block_selec);
        if(sfile.islink){
            printw("%s", " -> ");
            print_filelink(sfile.canonical);
        }
    }
}

//TODO top and bottom bar info
void print_blocks(){
    for(int i = 0; i < block_count; i++){
        print_elements(i);
    }
    print_lowbar();
}

//TODO print error function
void load_to_block(int id, path p){
    if(valid(p) < 1) return;
    
    blocks[id].directory = p;
    blocks[id].files = load_directory_files(p);
}

//TODO directory navigation functions
void enter_directory(int id, dir_file& e){
    if(e.status != READABLE_DIRECTORY) return;
    load_to_block(id, e.entry.path());
    blocks[block_selec].selec = 0;
}

#endif /* FEXPBLOCK_H */

