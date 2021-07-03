#ifndef FEXPMICRO_H
#define FEXPMICRO_H
#include "sys/stat.h"
#include "pwd.h"
#include <vector>
#include <fstream>
#include <ncurses.h>
#include "boost/filesystem.hpp"
using namespace std;
using namespace boost::filesystem;

std::ofstream dbgst;

const bool DO_DEBUG_LOG = true;
void debug_log(string str){
    if(!DO_DEBUG_LOG) return;
    dbgst.open("debug_log", ios::app);
    dbgst << str << "\n";
    dbgst.close();
}

const int 
        PAIR_NONE = 1,
        PAIR_SELEC = 2,
        PAIR_LINK = 3,
        PAIR_LINK_SELEC = 4,
        PAIR_EXE = 5,
        PAIR_EXE_SELEC = 6,
        PAIR_ERR = 7,
        PAIR_ERR_SELEC = 8
;

const int
        UNREADABLE_DIRECTORY = -2,
        UNREADABLE_FILE = -1,
        INVALID = 0,
        READABLE_FILE = 1,
        READABLE_DIRECTORY = 2
;

void init_colors(){
    init_pair(PAIR_NONE, COLOR_WHITE, COLOR_BLACK);
    init_pair(PAIR_SELEC, COLOR_BLACK, COLOR_BLUE);
    init_pair(PAIR_LINK, COLOR_CYAN, COLOR_BLACK);
    init_pair(PAIR_LINK_SELEC, COLOR_BLACK, COLOR_CYAN);
    init_pair(PAIR_EXE, COLOR_YELLOW, COLOR_BLACK);
    init_pair(PAIR_EXE_SELEC, COLOR_BLACK, COLOR_YELLOW);
    init_pair(PAIR_ERR, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_ERR_SELEC, COLOR_WHITE, COLOR_RED);
}

WINDOW* win = nullptr;

struct dir_file_link{
    path pth;
    int status;
};


struct dir_file{
    directory_entry entry;
//    struct stat fstat;
//    bool owned;
    int status;
    bool islink;
    
//    path canonical;
    dir_file_link canonical;
};

int valid(path p){
//    uint8_t bits;
    
    try{
        bool regfile = is_regular_file(p);
        bool dir = is_directory(p);

        if(!regfile && !dir){
            return INVALID;
        }

//        bool sym = is_symlink(p);
    
        if(dir){
            boost::system::error_code c;
            directory_iterator di(p, c);
            if(c.failed()){
                return UNREADABLE_DIRECTORY;
            }
            return READABLE_DIRECTORY;
        }else{
            std::ifstream ist(p.string());
            if(ist.is_open()){
                ist.close();
                return READABLE_FILE;
            }else{
                return UNREADABLE_FILE;
            }
        }
    }catch(...){}
    
    return INVALID;
}

string uncased(string str){
    transform(str.begin(), str.end(), str.begin(), [](char c){
       if(c >= 'A' || c <= 'Z') c += 32;
       return c;
    });
    return str;
}

//TODO ignore case in dirfile sorting
bool compare_dirfile(const dir_file& a, const dir_file& b){
    string s_a = uncased(a.entry.path().filename().string());
    string s_b = uncased(b.entry.path().filename().string());
    return s_a < s_b;
}

vector<dir_file> load_directory_files(path p){
    if(valid(p) < 1) return vector<dir_file>();
    
    vector<dir_file> files;
    
    directory_iterator it(p);
    for(directory_entry e : it){
        dir_file f = {e, valid(e.path()), is_symlink(e)};
        if(f.islink){
            boost::system::error_code err;
            path canon = canonical(f.entry.path(), err);
            f.canonical = {canon, valid(canon)};
        }
        
        files.push_back(f);
    }
    
    sort(files.begin(), files.end(), compare_dirfile);
    
    return files;
}

//string slashed_path(string str){
//    if(*str.rbegin() != '/') str.append(1, '/');
//    return str;
//}

/* PRINT CONSTRAINED */

struct print_constraint{
    int maxlen;
    string trunc;
    bool front;
};

void _print_constrained(string str, string trunc, unsigned int maxlen, bool front = false){
    if(str.size() > maxlen){
        if(front){
            str = trunc.append(str.substr(str.size() - (maxlen - trunc.size()), maxlen - trunc.size()));
        }else{
            str = str.substr(0, maxlen - trunc.size()).append(trunc);
        }
    }

    printw(str.c_str());
}

void print_constrained(string str, print_constraint cst){
    if(cst.maxlen == -1) cst.maxlen = win->_maxx + 1 - win->_curx;
    _print_constrained(str, cst.trunc, cst.maxlen, cst.front);
}

string _constrained(string str, string trunc, unsigned int maxlen, bool front = false){
    if(str.size() > maxlen){
        if(front){
            str = trunc.append(str.substr(str.size() - (maxlen - trunc.size()), maxlen - trunc.size()));
        }else{
            str = str.substr(0, maxlen - trunc.size()).append(trunc);
        }
    }
    
    return str;
}

string constrained(string str, print_constraint cst){
    if(cst.maxlen == -1) cst.maxlen = win->_maxx + 1 - win->_curx;
    return _constrained(str, cst.trunc, cst.maxlen, cst.front);
}

#endif /* FEXPMICRO_H */
