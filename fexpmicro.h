#ifndef FEXPMICRO_H
#define FEXPMICRO_H
using namespace std;

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

#endif /* FEXPMICRO_H */

