/* * FEXP * */
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include "boost/filesystem.hpp"
#include "ncurses.h"

#define PAIR_BLANK 0
#define PAIR_NOREAD 1
#define PAIR_BLANK_SELECTED 2
#define PAIR_NOREAD_SELECTED 3

using namespace std;
using namespace boost;
using namespace boost::filesystem;

const char* DEL_CHAR = "\b \b";

struct dirent{
    string path;
    bool isdir;
    bool islink;
    bool canread;
};

void print_dir();
void list_dir();
void tickup();
void tickup3();
void tickdown();
void tickdown3();
bool comp_pentr(dirent&, dirent&);
int clamp(int, int, int);
void string_search(bool&, bool&, bool&);
inline bool can_read(const path&);
void exit_path(bool&, bool&);
void enter_path(bool&, bool&);

int selec = 0;
vector<dirent> pathentrs;
path pth;
WINDOW* win;
bool root;
map<string, int> pathselnum;

int main(int argc, char** args){
    root = !getuid();
    
    pth = argc > 1 ? string(args[1]) : current_path();
    
    win = initscr();
    start_color();
    noecho();
    keypad(win, true);
    
    init_pair(PAIR_BLANK, COLOR_WHITE, COLOR_BLACK);
    init_pair(PAIR_BLANK_SELECTED, COLOR_WHITE, COLOR_BLUE);
    init_pair(PAIR_NOREAD, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_NOREAD_SELECTED, COLOR_RED, COLOR_BLUE);
    
    int cs;

    list_dir();
    print_dir();
    
    while(cs = getch()){
        bool refr = false;
        bool lp = false;
        bool appn = false;
        
        switch(cs){
        case 'q':
            goto endo;
            break;
        //up
        case 'w':
            tickup();
            refr = true;
            break;
        case KEY_UP:
            tickup();
            refr = true;
            break;
        case 'W':
            tickup3();
            refr = true;
            break;
        //down
        case 's':
            tickdown();
            refr = true;
            break;
        case KEY_DOWN:
            tickdown();
            refr = true;
            break;
        case 'S':
            tickdown3();
            refr = true;
            break;
        //enter dir
        case '\n':
            lp = refr = appn = pathentrs[selec].isdir;
            break;
        case 'd':
            lp = refr = appn = pathentrs[selec].isdir;
            break;
        case KEY_RIGHT:
            lp = refr = appn = pathentrs[selec].isdir;
            break;
        //exit dir
        case 'a':
            exit_path(lp, refr);
            break;
        case KEY_LEFT:
            exit_path(lp, refr);
            break;
        //next page
        case KEY_NPAGE:
            selec = ceil((static_cast<float>(selec) + 1) / (win->_maxy - 1)) * (win->_maxy - 1);
            selec = selec >= pathentrs.size() ? pathentrs.size() - 1 : selec;
            refr = true;
            break;
        //prev page
        case KEY_PPAGE:
            selec = floor(static_cast<float>(selec) / (win->_maxy - 1) - 1) * (win->_maxy - 1);
            selec = selec < 0 ? 0 : selec;
            refr = true;
            break;
        //next dir
        case ']':
            for(int i = selec + 1; i < pathentrs.size(); i++){
                if(pathentrs[i].isdir){
                    selec = i;
                    break;
                }
            }
            refr = true;
            break;
        //prev dir
        case '[':
            for(int i = selec - 1; i >= 0; i--){
                if(pathentrs[i].isdir){
                    selec = i;
                    break;
                }
            }
            refr = true;
            break;
        //next file
        case '.':
            for(int i = selec + 1; i < pathentrs.size(); i++){
                if(!pathentrs[i].isdir){
                    selec = i;
                    break;
                }
            }
            refr = true;
            break;
        //prev file
        case ',':
            for(int i = selec - 1; i >= 0; i--){
                if(!pathentrs[i].isdir){
                    selec = i;
                    break;
                }
            }
            refr = true;
            break;
        //string search
        case ' ':
            string_search(lp, refr, appn);
            break;
        }
        
        //enter directory
        if(appn && pathentrs.size() > 0){
//            if(pathentrs[selec].canread){
//                if(pathentrs[selec].islink){
//                    pth /= pathentrs[selec].path;
//                    pth = canonical(pth);
//                }else{
//                    pth /= pathentrs[selec].path;
//                }
//            }else{
//                lp = false;
//            }
            enter_path(lp, refr);
        }
        if(lp) list_dir();
        if(refr) print_dir();
    }
    
    endo:;
    
    endwin();
    return 0;
}

void print_dir(){
    win->_curx = win->_cury = 0;
    
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
        int pr = ((i == selec) << 1) + !it->canread;
        
        printw("\n");
        win->_cury--;
        
        if(i == selec){
            string lk = "";
            if(it->islink){
                path lkpath = pth;
                lkpath.append(it->path);
                
                lk = " -> " + canonical(lkpath).string();
            }
            
            selecpath = it->path + lk;
        }
        
        attron(COLOR_PAIR(pr));
        string pp = it->path + (it->isdir ? "/" : "") + (it->islink ? " [link]" : "") + "\n";
        printw(pp.c_str());
        attroff(COLOR_PAIR(pr));
        i++;
    }
    
    while(i++ < ed){
        printw("\n");
    }
    
    win->_cury = win->_maxy;
    printw("\n");
    attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
    printw(selecpath.c_str());
    attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));
   
    refresh();
}

void list_dir(){
    pathentrs.clear();
    directory_iterator end;
    for(directory_iterator it(pth); it != end; it++){
        pathentrs.push_back({it->path().filename().string(), is_directory(it->path()), is_symlink(it->path()),
            can_read(it->path())});
    }
    
    sort(pathentrs.begin(), pathentrs.end(), comp_pentr);
    
//    selec = 0;
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

bool comp_pentr(dirent& first, dirent& second){
    return first.path < second.path;
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

void string_search(bool& lp, bool& refr, bool& appn){
    win->_curx = 0;
    win->_cury = win->_maxy;
    printw("\n");
    attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
    printw("||");
    attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));
    win->_curx = 2;

    refresh();

    string spinp = "";
    int spc = 0;
    int i;
    int strpos = 0;
    int found = -1;

    while(spc = getch()){
        switch(spc){
        case '\n': //RETURN KEY
            if(spinp[0] == '#'){
               string subsp = spinp.substr(1);

                if(subsp.substr(0, 5) == "goto "){
                    path gopath = path(subsp.substr(5));
                    if(is_directory(gopath) && can_read(gopath)){
                        pathselnum[pth.filename().string()] = selec;
                        
                        pth = canonical(gopath);
                        lp = true;
                        
                        selec = pathselnum[pth.filename().string()];
                    }
                }
            //COMMAND LINE
            }else if(spinp[0] == '$'){
                string inlcmd = "cd " + pth.string() + ";" + spinp.substr(1);
                std::system(inlcmd.c_str());
                clear();
                refr = true;
            }else if(found > -1){
                selec = found;
            }

            spc = -1;
            break;
        case '\\':
            if(found > -1){
                selec = found;
                appn = true;
                lp = true;
            }
            
            spc = -1;
            break;
        case '`': //ESCAPE KEY
            spc = -1;
            break;
        case 127: //BACKSPACE KEY
            if(strpos > 0){
                spinp.erase(strpos - 1, 1);
                strpos--;
            }
            found = -1;
            break;
        case KEY_LEFT:
            strpos = clamp(--strpos, 0, spinp.size());
            break;
        case KEY_RIGHT:
            strpos = clamp(++strpos, 0, spinp.size());
            break;
        default:
            spinp.insert(strpos, 1, spc);
            strpos++;
            found = -1;
            break;
        }

        if(spc == -1){
            refr = true;
            break;
        }

        if(found == -1 && spinp.size() > 0){
            i = 0;
            for(vector<dirent>::iterator it = pathentrs.begin(); it < pathentrs.end(); it++){
                if(it->path.substr(0, spinp.size()) == spinp){
                    found = i;
                    break;
                }
                i++;
            }
        }

        win->_curx = 0;
        printw("\n");
        attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
        printw("||");
        printw(spinp.c_str());

        if(found > -1){
            printw(("\t|| " + pathentrs[found].path + (pathentrs[found].isdir ? "/" : "")).c_str());
        }
        attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));

        win->_curx = strpos + 2;

        refresh();
    }
}

inline bool can_read(const path& pth){
    return root || status(pth).permissions() & perms::others_read;
}

void exit_path(bool& lp, bool& refr){
    if(exists(pth.parent_path())){
        pathselnum[pth.filename().string()] = selec;
        
        pth = pth.parent_path();
        lp = refr = true;
        
        selec = pathselnum[pth.filename().string()];
    }
}

void enter_path(bool& lp, bool& refr){
    if(pathentrs[selec].canread){
        pathselnum[pth.filename().string()] = selec;

        if(pathentrs[selec].islink){
            pth /= pathentrs[selec].path;
            pth = canonical(pth);
        }else{
            pth /= pathentrs[selec].path;
        }

        selec = pathselnum[pth.filename().string()];
    }else{
        lp = false;
        refr = false;
    }
}
