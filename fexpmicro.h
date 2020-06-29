#ifndef FEXPMICRO_H
#define FEXPMICRO_H
#include "boost/filesystem.hpp"
using namespace std;
using namespace boost;
using namespace boost::filesystem;

struct dirent{
    string path;
    bool isdir;
    bool islink;
    bool canread;
};

void tickup(int& selec, vector<dirent>& pathentrs){
    if(selec > 0){
        selec--;
    }else{
        selec = pathentrs.size() - 1;
    }
}

void tickup3(int& selec, vector<dirent>& pathentrs){
    selec -= 3;
    if(selec < 0){
        selec = pathentrs.size() - 1 + selec;
    }
}

void tickdown(int& selec, vector<dirent>& pathentrs){
    if(selec < pathentrs.size() - 1){
        selec++;
    }else{
        selec = 0;
    }
}

void tickdown3(int& selec, vector<dirent>& pathentrs){
    selec += 3;
    if(selec > pathentrs.size() - 1){
        selec = selec - (pathentrs.size() - 1);
    }
}

void decrement(int& strpos){
    strpos = strpos == 0 ? 0 : strpos - 1;
}

void increment(int& strpos, string& input){
    strpos = strpos == input.size() ? strpos : strpos + 1;
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

#endif /* FEXPMICRO_H */

