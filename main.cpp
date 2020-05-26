/* * FEXP * */
#include <iostream>
#include <vector>
#include <cmath>
#include "boost/filesystem.hpp"
#include "ncurses.h"

#define PAIR_BLANK 0
#define PAIR_GREEN 1
#define PAIR_BLANK_SELECTED 2
#define PAIR_GREEN_SELECTED 3

using namespace std;
using namespace boost;
using namespace boost::filesystem;

struct dirent{
    string path;
    bool isdir;
    bool islink;
};

void print_dir();
void list_dir();
void tickup();
void tickdown();
bool comp_pentr(dirent&, dirent&);

int selec = 0;
vector<dirent> pathentrs;
path pth;
WINDOW* win;

int main(int argc, char** args){
    pth = argc > 1 ? string(args[1]) : current_path();
    
    win = initscr();
    start_color();
    noecho();
    keypad(win, true);
    
    init_pair(PAIR_BLANK, COLOR_WHITE, COLOR_BLACK);
    init_pair(PAIR_BLANK_SELECTED, COLOR_WHITE, COLOR_BLUE);
    
    int cs;

    list_dir();
    print_dir();
    
    //TODO name search
    while(cs = getch()){
        bool refr = false;
        bool lp = false;
        bool appn = false;
        
        //TODO add more key macros
        switch(cs){
        case 'q':
            goto endo;
            break;
        case 'w':
            tickup();
            refr = true;
            break;
        case KEY_UP:
            tickup();
            refr = true;
            break;
        case 's':
            tickdown();
            refr = true;
            break;
        case KEY_DOWN:
            tickdown();
            refr = true;
            break;
        case 'e':
            lp = refr = appn = pathentrs[selec].isdir;
            break;
        case 'd':
            lp = refr = appn  = pathentrs[selec].isdir;
            break;
        case KEY_RIGHT:
            lp = refr = appn  = pathentrs[selec].isdir;
            break;
        case 'a':
            if(pth != "/"){
                pth = pth.parent_path();
                lp = refr = true;
            }
            break;
        }
        
        if(appn){
            if(pathentrs[selec].islink){
                pth.append(pathentrs[selec].path);
                pth = canonical(pth);
            }else{
                pth /= pathentrs[selec].path;
            }
        }
        if(lp) list_dir();
        if(refr) print_dir();
    }
    
    endo:;
    
    endwin();
    return 0;
}

void print_dir(){
    clear();
    
    string current = "|| " + pth.string() + " || " + to_string(selec + 1) + "\\" + to_string(pathentrs.size()) + " ||\n";
    printw(current.c_str());

    int mxy = win->_maxy - 1;
    
    int m = 0;
    for(int j = mxy; j < pathentrs.size(); j += mxy){
        if(selec < j) break;
        m++;
    }
    
    int bg = m * mxy;
    int ed = (m + 1) * mxy;
    
    string selecpath;
    
    int i = bg;
    for(vector<dirent>::iterator it = pathentrs.begin() + bg;
            it < (ed >= pathentrs.size() ? pathentrs.end() : pathentrs.begin() + ed); it++){
        if(i == selec){
            attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
            
            string lk = "";
            if(it->islink){
                path lkpath = pth;
                lkpath.append(it->path);
                
                lk = " -> " + canonical(lkpath).string();
            }
            
            selecpath = it->path + lk;
        }
        string pp = it->path + (it->isdir ? "/" : "") + (it->islink ? " [link]" : "") + "\n";
        printw(pp.c_str());
        if(i == selec) attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));
        i++;
    }
    
    win->_cury = win->_maxy;
    attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
    printw(selecpath.c_str());
    attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));
    
    refresh();
}

void list_dir(){
    pathentrs.clear();
    directory_iterator end;
    for(directory_iterator it(pth); it != end; it++){
        pathentrs.push_back({it->path().filename().string(), is_directory(it->path()), is_symlink(it->path())});
    }
    
    sort(pathentrs.begin(), pathentrs.end(), comp_pentr);
    
    selec = 0;
}

void tickup(){
    if(selec > 0){
        selec--;
    }else{
        selec = pathentrs.size() - 1;
    }
}

void tickdown(){
    if(selec < pathentrs.size() - 1){
        selec++;
    }else{
        selec = 0;
    }
}

bool comp_pentr(dirent& first, dirent& second){
    return first.path < second.path;
}
