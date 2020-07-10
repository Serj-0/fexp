#ifndef FEXPMICRO_H
#define FEXPMICRO_H
#include <vector>
#include "boost/filesystem.hpp"
#include "ncurses.h"
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
};

vector<path> path_stack;

int selec = 0;
vector<dirent> pathentrs;
vector<path> srchentrs;
path pth;
WINDOW* win;
bool root;
map<string, int> pathselnum;
std::ofstream dbglog;
string targselec = "";

void tickup(){
    if(selec > 0){
        selec--;
    }else{
        selec = pathentrs.size() - 1;
    }
}

void tickup3(){
    selec -= 3;
    if(selec < 0){
        selec = pathentrs.size() - 1 + selec;
    }
}

void tickdown(){
    if(selec < pathentrs.size() - 1){
        selec++;
    }else{
        selec = 0;
    }
}

void tickdown3(){
    selec += 3;
    if(selec > pathentrs.size() - 1){
        selec = selec - (pathentrs.size() - 1);
    }
}

void decrement(int& strpos){
    if(strpos > 0) strpos--;
}

void increment(int& strpos, string& input){
    if(strpos < input.size()) strpos++;
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

void save_selec(){
    pathselnum[pth.string()] = selec;
}

void load_selec(){
    selec = pathselnum[pth.string()];
}

void go_right(bool& lp, bool& refr, bool& appn, bool& jmp){
    if(pathentrs[selec].canread){
        if(pathentrs[selec].isdir){
            lp = refr = appn = true;
        }else if(pathentrs[selec].islink){
            lp = refr = jmp = true;
        }
    }
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

void jump_to(path& to, bool slash, bool& refr, bool& lp){
    pathselnum[pth.string()] = selec;
    pth = to;
    if(slash) pth += "/";
    refr = lp = true;
    selec = pathselnum[pth.string()];
}

void jump_to(string& to, bool slash, bool& refr, bool& lp){
    pathselnum[pth.string()] = selec;
    pth = to;
    if(slash) pth += "/";
    refr = lp = true;
    selec = pathselnum[pth.string()];
}

void jump_to(const char* to, bool slash, bool& refr, bool& lp){
    pathselnum[pth.string()] = selec;
    pth = to;
    if(slash) pth += "/";
    refr = lp = true;
    selec = pathselnum[pth.string()];
}

void push_path(){
    path_stack.push_back(pth);
//    if(path_stack.size() > fexpconf::path_stack_max){
}

void pop_path(){
    if(!path_stack.empty()){
        pth = *path_stack.end();
        path_stack.erase(path_stack.end());
    }
}

#endif /* FEXPMICRO_H */

