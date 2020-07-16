#ifndef FEXPMICRO_H
#define FEXPMICRO_H
#include <vector>
#include "boost/filesystem.hpp"
#include "ncurses.h"
#include "fexpconfig.h"
using namespace std;
using namespace boost;
using namespace boost::filesystem;

#define FEXP_VERSION 0.4

struct dirent{
    typedef boost::filesystem::path boostpath;
    boostpath rpath;
    string path;
    bool isnorm, isdir, islink, canread;
    long lastmodtime;
    long filesize;
};

vector<path> path_stack;
int pathhisti = -1;

int selec = 0;
vector<dirent> pathentrs;
vector<path> srchentrs;
path pth;
WINDOW* win;
bool root;
map<string, int> pathselnum;
std::ofstream dbglog;
string targselec = "";

const string data_size_units[4] = {" GBs", " MBs", " KBss", " Bytes"};

bool refr = false;
bool lp = false;
bool appn = false;
bool jmp = false;

void save_selec(){
    pathselnum[pth.string()] = selec;
}

void load_selec(){
    selec = pathselnum[pth.string()];
}

void decrement(int& strpos){
    if(strpos > 0) strpos--;
}

void increment(int& strpos, string& input){
    if(strpos < input.size()) strpos++;
}

void push_path(){
    if(pathhisti < path_stack.size() - 1){
        path_stack.erase(path_stack.begin() + pathhisti, path_stack.end());
    }
    
    if(pathhisti >= fexpconf::hist_max_size - 1){
        path_stack.erase(path_stack.begin());
    }else{
        pathhisti++;
    }

    path_stack.push_back(pth);
}

void set_working_directory(path npth, string appn, bool slashappn, bool canon, bool parent, bool dolp, bool push, string findafter = ""){
    save_selec();
    
    if(!appn.empty()){
        if(slashappn && pth != "/"){
            npth /= appn;
        }else{
            npth += appn;
        }
    }
    
    pth = npth;
    
    if(canon) pth = canonical(pth);
    if(parent){
        pth = pth.parent_path();
    }

    if(*pth.string().rbegin() != '/') pth += "/";
    
    if(push) push_path();
    
    if(findafter.empty()){
        load_selec();
    }else{
        targselec = findafter;
    }
    
    lp = dolp;
    refr = true;
}

void hist_go_back(){
    if(pathhisti > 0){
        set_working_directory(path_stack[--pathhisti], "", false, false, false, true, false);
    }
//    cout << pathhisti;
//    getchar();
}

void hist_go_forward(){
    if(pathhisti < path_stack.size() - 1){
        set_working_directory(path_stack[++pathhisti], "", false, false, false, true, false);
    }
}

void tickup(){
    if(pathentrs.empty()) return;
    
    if(selec > 0){
        selec--;
    }else{
        selec = pathentrs.size() - 1;
    }
}

void tickup3(){
    if(pathentrs.empty()) return;
    
    selec -= 3;
    if(selec < 0){
        selec = pathentrs.size() - 1 + selec;
    }
}

void tickdown(){
    if(pathentrs.empty()) return;
    
    if(selec < pathentrs.size() - 1){
        selec++;
    }else{
        selec = 0;
    }
}

void tickdown3(){
    if(pathentrs.empty()) return;
    
    selec += 3;
    if(selec > pathentrs.size() - 1){
        selec = selec - (pathentrs.size() - 1);
    }
}

void delchar(int& strpos, string& input){
    int delpos = strpos == 0 ? 0 : strpos - 1;
    input.erase(delpos, 1);
    decrement(strpos);
}

bool comp_pentr(dirent& first, dirent& second){
    return first.path < second.path;
}

bool comp_path(path& first, path& second){
    return first.filename().string() < second.filename().string();
}

int clamp(int value, int min, int max){
    if(value < min){
        return min;
    }else if(value > max){
        return max;
    }else{
        return value;
    }
}

void go_right(){
    if(pathentrs[selec].canread){
        if(pathentrs[selec].islink){
            lp = refr = jmp = true;
        }else if(pathentrs[selec].isdir){
            lp = refr = appn = true;
        }
    }
}

void exit_path(){
    if(exists(pth.parent_path())){
        set_working_directory(pth.parent_path().parent_path(), "", false, false, false, true, true);
    }
}

inline bool can_read(const path& pth){
    return exists(pth) && (root || status(pth).permissions() & perms::group_read);
}

inline bool can_write(const path& pth){
    return exists(pth) && (root || status(pth).permissions() & perms::group_write);
}

string canon_selec(path& pth, vector<dirent>& pathentrs, int& selec){
    string lk;
    
    path lkpath = pth;
    lkpath.append(pathentrs[selec].path);

    if(exists(canonical(lkpath))){
        lk = " -> " + canonical(lkpath).string();
    }else{
        lk = " [Link does not exist]";
    }
    
    return lk;
}

int system_call(string cmd){
    clear();
    int r = std::system(cmd.c_str());
    clear();
    noecho();
    keypad(win, true);
    return r;
}

#endif /* FEXPMICRO_H */
