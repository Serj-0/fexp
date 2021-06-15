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

//const uint8_t
//        READABLE = 1,
//        DIRECTORY = 2,
//        SYMLINK = 4
//;

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

struct dir_file{
    directory_entry entry;
//    struct stat fstat;
//    bool owned;
    int status;
    bool link;
    path canonical;
};

WINDOW* win = nullptr;

//TODO maybe change return value to bitmap
int valid(path p){
//    uint8_t bits;
    
    try{
        bool regfile = is_regular_file(p);
        bool dir = is_directory(p);

        if(!regfile && !dir) return INVALID;

//        bool sym = is_symlink(p);
    
        if(dir){
            boost::system::error_code c;
            directory_iterator di(p, c);
            if(c.failed()) return UNREADABLE_DIRECTORY;
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

bool compare_dirfile(const dir_file& a, const dir_file& b){
    return a.entry.path().filename().string() < b.entry.path().filename().string();
}

vector<dir_file> load_directory_files(path p){
    if(valid(p) < 1) return vector<dir_file>();
    
    vector<dir_file> files;
    
    boost::system::error_code err;
    
    directory_iterator it(p);
    for(directory_entry e : it){
        files.push_back({e, valid(e.path()), is_symlink(e), read_symlink(e.path(), err)});
    }
    
    sort(files.begin(), files.end(), compare_dirfile);
    
    return files;
}

#endif /* FEXPMICRO_H */
