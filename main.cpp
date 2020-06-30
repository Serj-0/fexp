/* * FEXP * */
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <fstream>
#include "boost/filesystem.hpp"
#include "fexpmicro.h"
#include "ncurses.h"

#define PAIR_BLANK 0
#define PAIR_NOREAD 1
#define PAIR_BLANK_SELECTED 2
#define PAIR_NOREAD_SELECTED 3

using namespace std;
using namespace boost;
using namespace boost::filesystem;

const char* DEL_CHAR = "\b \b";

void print_dir();
void list_dir(path& pth, vector<dirent>& pathentrs);
void string_search(bool& lp);
inline bool can_read(const path& pth);
void exit_path(bool& lp, bool& refr);
void enter_path(bool& lp, bool& refr);

int selec = 0;
vector<dirent> pathentrs;
vector<path> srchentrs;
path pth;
WINDOW* win;
bool root;
map<string, int> pathselnum;
std::ofstream dbglog;

int main(int argc, char** args){
    root = !getuid();
    
    dbglog.open("fexp.log");
    
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

    list_dir(pth, pathentrs);
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
            tickup(selec, pathentrs);
            refr = true;
            break;
        case KEY_UP:
            tickup(selec, pathentrs);
            refr = true;
            break;
        case 'W':
            tickup3(selec, pathentrs);
            refr = true;
            break;
        //down
        case 's':
            tickdown(selec, pathentrs);
            refr = true;
            break;
        case KEY_DOWN:
            tickdown(selec, pathentrs);
            refr = true;
            break;
        case 'S':
            tickdown3(selec, pathentrs);
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
            string_search(lp);
            refr = true;
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
            
        if(lp) list_dir(pth, pathentrs);
        if(refr) print_dir();
    }
    
    endo:;
    
    endwin();
    dbglog.close();
    
    return 0;
}

void print_dir(){
    win->_curx = win->_cury = 0;
    printw("\n");
    win->_curx = win->_cury = 0;
    
    string current = "|| " + pth.string() + " || " + to_string(selec + 1) + "\\" + to_string(pathentrs.size()) + " ||";
    printw(current.c_str());
    
    attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
    win->_curx = win->_maxx - 35;
    printw("'q' to quit | space to start search\n");
    attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));
            
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
            bool dne = false;
            if(it->islink){
                path lkpath = pth;
                lkpath.append(it->path);
                
                if(exists(lkpath)){
                    lk = " -> " + canonical(lkpath).string();
                }else{
                    dne = true;
                }
            }
            
            selecpath = dne ? "[Does not exist]" : it->path + lk;
//            selecpath = it->path + lk;
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

void list_dir(path& pth, vector<dirent>& pathentrs){
    pathentrs.clear();
    directory_iterator end;
    for(directory_iterator it(pth); it != end; it++){
        bool isdir;
        bool canread;
        bool islink;
        
        try{
            isdir = is_directory(it->path());
            canread = can_read(it->path());
            islink = is_symlink(it->path());
        }catch(...){
            isdir = false;
            canread = false;
            islink = false;
        }

        pathentrs.push_back({it->path().filename().string(), isdir, islink,
            canread});
    }
    
    sort(pathentrs.begin(), pathentrs.end(), comp_pentr);
}

void string_search(bool& lp){
    string input;
    int strpos = 0;
    int c;
    
    win->_curx = 0;
    win->_cury = win->_maxy;

    printw("\n");
    printw(("||" + input + "||").c_str());
    win->_curx = strpos + 2;
    refresh();
    
    vector<dirent> srchentrs;
    path srchpth;
    path fnd;
    int srchsel = -1;
    
    while(c = getch()){
        switch(c){
        case '`':
            goto over;
        case KEY_LEFT:
            decrement(strpos);
            break;
        case KEY_RIGHT:
            increment(strpos, input);
            break;
        case KEY_BACKSPACE:
            delchar(strpos, input);
            break;
        case '\b':
            delchar(strpos, input);
            break;
        case 127:
            delchar(strpos, input);
            break;
        case '\t':
            if(srchsel > -1){
                input = srchpth.string();
                if(input != "/") input.append("/");
                input.append(srchentrs[srchsel].path);

                if(is_directory(path(input))) input.append("/");
                
                srchentrs.clear();
                strpos = input.size();
            }
            break;
        case '\n':
            if(srchsel > -1){
                fnd = srchpth;

                pathselnum[pth.string()] = selec;
                    
                if(srchentrs[srchsel].isdir){
                    fnd /= srchentrs[srchsel].path;
//                    fnd += "/";
                    selec = pathselnum[fnd.string()];
                    lp = true;
                }else{
                    pathentrs = srchentrs;
                    selec = srchsel;
                }

                pth = canonical(fnd);
                pth += "/";
                goto over;
            }else if(srchpth == "/"){
                pth = "/";
                lp = true;
                goto over;
            }
            break;
        default:
            input.insert(strpos, 1, c);
            strpos++;
        }
        
        //evaluate string
        string fullpath = input[0] == '/' ? input : pth.string() + input;
        
        string fname = "";
        srchpth = fullpath;
        srchsel = -1;
        
        if(!fullpath.empty()){
            if(fullpath[fullpath.size() - 1] == '/'){
                int slashpos = fullpath.find_last_of('/', fullpath.size() - 2) + 1;
                fname = fullpath.substr(slashpos, fullpath.size() - slashpos - 1);
                if(srchpth.string() != "/") srchpth = srchpth.parent_path().parent_path();
            }else{
                fname = fullpath.substr(fullpath.find_last_of('/', fullpath.size() - 1) + 1);
                srchpth = srchpth.parent_path();
            }
            
            if(is_directory(srchpth)){
                list_dir(srchpth, srchentrs);

                if(!fname.empty()){
                    for(int i = 0; i < srchentrs.size(); i++){
                        if(srchentrs[i].path.substr(0, fname.size()) == fname){
                            srchsel = i;
                            break;
                        }
                    }
                }
            }
        }
        
        //draw search line
        dsl:;
        win->_curx = 0;
        win->_cury = win->_maxy;
        printw("\n");
        
        attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
        win->_curx = win->_maxx - 13;
        printw("'`' to cancel");
        win->_curx = 0;
        attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));
        
        printw(("||" + input).c_str());
        
        //TODO display more information about search find e.g. is link/directory
        attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
        printw(("||" + srchpth.string()).c_str());
        if(srchsel > -1) printw((" => " + srchentrs[srchsel].path).c_str());
        attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));
        win->_curx = strpos + 2;
        refresh();
    }
    over:;
}

inline bool can_read(const path& pth){
    return exists(pth) && (root || status(pth).permissions() & perms::others_read);
}

void exit_path(bool& lp, bool& refr){
    if(exists(pth.parent_path())){
        pathselnum[pth.string()] = selec;
        
        pth = pth.parent_path().parent_path();
        if(pth != "/") pth += "/";
        lp = refr = true;
        
        selec = pathselnum[pth.string()];
    }
}

void enter_path(bool& lp, bool& refr){
    if(pathentrs[selec].canread){
        pathselnum[pth.string()] = selec;

        if(pathentrs[selec].islink){
            pth /= pathentrs[selec].path;
            pth = canonical(pth);
            pth;
        }else{
            pth /= (pathentrs[selec].path);
        }
        
        if(pth != "/") pth += "/";

        selec = pathselnum[pth.string()];
    }else{
        lp = false;
        refr = false;
    }
}
