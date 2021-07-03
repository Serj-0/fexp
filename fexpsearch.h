#ifndef FEXPSEARCH_H
#define FEXPSEARCH_H
#include <vector>
#include <ncurses.h>
#include "fexpmicro.h"
#include "fexpblock.h"
using namespace std;

int search(string in, vector<string> options, bool igncase = false){
    int r = -1;
    int score = 0;
    
    for(int i = 0; i < options.size(); i++){
        int cmpscore = 0;
        
        for(int j = 0; j < min(in.size(), options[i].size()); j++){
            int c = options[i][j];
            int c2 = in[j];
            if(igncase && c >= 'A' && c <= 'Z') c += 32;
            if(igncase && c2 >= 'A' && c2 <= 'Z') c2 += 32;
            
            if(c2 != c){
                cmpscore = 0;
                break;
            }
            cmpscore++;
        }
        
        if(cmpscore > score){
            score = cmpscore;
            r = i;
        }
    }
    
    return r;
}

const int 
        AFTER_KEY_NORMAL = 0,
        AFTER_KEY_EXIT = 1,
        AFTER_KEY_REFRESH = 2
;

void string_prompt(string prompt, int y, int (*after_keys)(string&, string&, int&, int&) = nullptr){
    move(y, 0);
    clrtoeol();
    printw("%s", prompt.c_str());
    
    string in;
    int pos = 0;
    
    curs_set(1);
    
    int c;
    while(1){
        c = getch();
        if(c == 27){
            break;
        }else if(c == KEY_BACKSPACE && pos && in.size()){
            in.erase(pos-- - 1, 1);
            
            win->_curx = prompt.size();
            printw("%s", in.c_str());
            clrtoeol();
        }else if(c >= 32 && c <= 126){
            in.insert(pos++, 1, c);
            
            win->_curx = prompt.size();
            printw("%s", in.c_str());
            clrtoeol();
        }else if(c == KEY_LEFT){
            pos -= pos > 0;
        }else if(c == KEY_RIGHT){
            pos += pos < in.size();
        }
        
        if(after_keys){
            int r = after_keys(prompt, in, pos, c);
            
            switch(r){
            case AFTER_KEY_REFRESH:
                print_borders();
                print_blocks();
                
                move(y, 0);
                clrtoeol();
                printw("%s", prompt.c_str());
                break;
            case AFTER_KEY_EXIT:
                goto over;
            }
        }
        
        win->_curx = prompt.size() + pos;
        refresh();
    }
    
    over:;
    curs_set(0);
}

//TODO return true on sigint
int search_in_files(string& prompt, string& in, int& pos, int& c){
    vector<string> paths;
    for(dir_file f : blocks[block_selec].files){
        paths.push_back(f.entry.path().filename().string());
    }
    int i = search(in, paths, true);
    
    if(i > -1){
        if(c == '\t'){
            in = paths[i];
            pos = in.size();
            win->_curx = prompt.size();
            printw("%s", in.c_str());
        }else if(c == '\n'){
            blocks[block_selec].selec = i;
            
            if(enter_directory(block_selec, blocks[block_selec].files[i])){
                in.clear();
                pos = 0;
                return AFTER_KEY_REFRESH;
            }
            return AFTER_KEY_EXIT;
        }
        
        win->_curx = win->_maxx - paths[i].size() - 1;
        print_filename(blocks[block_selec].files[i], block_selec);
        refresh();
    }else if(c == '\n' && in == ".."){
        exit_directory(block_selec);
        in.clear();
        pos = 0;
        
//        win->_curx = win->_maxx - paths[i].size() - 1;
//        print_filename(blocks[block_selec].files[i], block_selec);
//        refresh();
        return AFTER_KEY_REFRESH;
    }
    
    return AFTER_KEY_NORMAL;
}

#endif /* FEXPSEARCH_H */

