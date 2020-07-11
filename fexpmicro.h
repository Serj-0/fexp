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
string jfile = "";

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

void push_path(){
    path_stack.push_back(pth);
//    if(path_stack.size() > fexpconf::path_stack_max){
}

void pop_path(){
    if(!path_stack.empty()){
        pathselnum[pth.string()] = selec;
        
        pth = *(path_stack.end() - 1);
//        cout << pth.string();
        path_stack.erase((path_stack.end() - 1));
        
        selec = pathselnum[pth.string()];
        
        lp = true;
        refr = true;
    }
}

void set_working_directory(path npth, string appn, bool slashappn, bool canon, bool parent, bool dolp, string findafter = ""){
    save_selec();
    push_path();
    
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

    if(pth != "/") pth += "/";
    
    if(findafter.empty()){
        load_selec();
    }else{
        targselec = findafter;
    }
    
    lp = dolp;
    refr = true;
}

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

void go_right(){
    if(pathentrs[selec].canread){
        if(pathentrs[selec].isdir){
            lp = refr = appn = true;
        }else if(pathentrs[selec].islink){
            lp = refr = jmp = true;
        }
    }
}

void exit_path(){
    if(exists(pth.parent_path())){
        set_working_directory(pth.parent_path().parent_path(), "", false, false, false, true);
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

//void jump_to(path& to, bool slash, bool& refr, bool& lp){
//    pathselnum[pth.string()] = selec;
//    pth = to;
//    if(slash) pth += "/";
//    refr = lp = true;
//    selec = pathselnum[pth.string()];
//}
//
//void jump_to(string& to, bool slash, bool& refr, bool& lp){
//    pathselnum[pth.string()] = selec;
//    pth = to;
//    if(slash) pth += "/";
//    refr = lp = true;
//    selec = pathselnum[pth.string()];
//}
//
//void jump_to(const char* to, bool slash, bool& refr, bool& lp){
//    pathselnum[pth.string()] = selec;
//    pth = to;
//    if(slash) pth += "/";
//    refr = lp = true;
//    selec = pathselnum[pth.string()];
//}

#endif /* FEXPMICRO_H */

