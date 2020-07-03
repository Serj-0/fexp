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
void char_result();
string get_string_input(string msg);

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
    
//    dbglog.open("fexp.log");
    
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
    
    string strget;
    
    while(cs = getch()){
        bool refr = false;
        bool lp = false;
        bool appn = false;
        bool jmp = false;
        
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
            go_right(lp, refr, appn, jmp, pathentrs, selec);
            break;
        case 'd':
            go_right(lp, refr, appn, jmp, pathentrs, selec);
            break;
        case KEY_RIGHT:
            go_right(lp, refr, appn, jmp, pathentrs, selec);
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
        //char testing
        case KEY_F(9):
            char_result();
            refr = true;
            break;
        case 4:
            strget = get_string_input("Make directory: ");
            //TODO make directory when string entered
            
//            permiss
            if(!strget.empty()){
                create_directories(path(strget));
            }
            refr = true;
            break;
        case 6:
            
            break;
        }
        
        //enter directory
        string jfile;
        
        if(appn && pathentrs.size() > 0){
            enter_path(lp, refr);
        }else if(jmp && pathentrs.size() > 0){
            pathselnum[pth.string()] = selec;
            
            pth /= pathentrs[selec].path;
            pth = canonical(pth);
            jfile = pth.filename().string();
            pth = pth.parent_path();
            pth += "/";
        }
        
        if(lp) list_dir(pth, pathentrs);
        
        if(jmp){
            for(int i = 0; i < pathentrs.size(); i++){
                if(pathentrs[i].path == jfile){
                    selec = i;
                    break;
                }
            }
        }
        
        if(refr) print_dir();
    }
    
    endo:;
    
    endwin();
//    dbglog.close();
    
    return 0;
}

string get_string_input(string msg){
    string input;
    int strpos = 0, c;
    
    win->_curx = 0;
    win->_cury = win->_maxy;
    printw("\n");
    
    printw(msg.c_str());
    
    refresh();
    
    while(c = getch()){
        switch(c){
        case '`':
          goto over;
        case KEY_LEFT:
            if(strpos > 0) strpos--;
            break;
        case KEY_RIGHT:
            if(strpos < input.size()) strpos++;
            break;
        case 127:
            delchar(strpos, input);
            break;
        case KEY_BACKSPACE:
            delchar(strpos, input);
            break;
        case '\b':
            delchar(strpos, input);
            break;
        case KEY_ENTER:
            return input;
            break;
        case '\n':
            return input;
            break;
        default:
            input.insert(strpos, 1, c);
            strpos++;
        }
        
        win->_cury = win->_maxy;
        win->_curx = msg.size();
        printw("\n");
        
        printw(input.c_str());
        
        attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
        win->_curx = win->_maxx - 13;
        printw("'`' to cancel");
        win->_curx = 0;
        attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));
        
        win->_curx = msg.size() + strpos;
        
        refresh();
    }
    
    over: return "";
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

void char_result(){
    int c = 0;
    char cc;
    const char* ccc = &cc;
    
    win->_cury = win->_maxy;
    win->_curx = 0;
    printw("\n");
    win->_curx = 0;
    
    while(c = getch()){
        if(c == '`') break;
        
        win->_curx = 0;
        printw("\n");
        win->_curx = 0;
        cc = c;
        printw(ccc);
        printw((" : " + to_string(c)).c_str());
    }
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
            /* * JUMPS * */
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
            }else if(input == "/"){
                pth = "/";
                lp = true;
                goto over;
            }else if(input == "~"){
                pth = getenv("HOME");
                lp = true;
                goto over;
            /* * COMMANDS * */
            }else if(input.find_first_of('?') != input.npos){
                int qpos = input.find_first_of('?');
                string srchstr = input.substr(0, qpos);
                string cmd = input.substr(qpos + 1);
                
                while(srchstr[srchstr.size() - 1] == ' ') srchstr.pop_back();
                while(cmd[0] == ' ') cmd.erase(0, 1);
                
                if(srchstr.empty()) srchstr = pth.string() + pathentrs[selec].path;
                
                int dpos = 0;
                while((dpos = cmd.find_first_of('$'), dpos != cmd.npos)){
                    cmd.replace(dpos, 1, srchstr);
                }
                
                clear();
                std::system(("cd " + pth.string() + ";" + cmd).c_str());
                clear();
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
        
        attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
        printw(("||" + srchpth.string()).c_str());
        if(srchsel > -1){
            printw((" : " + srchentrs[srchsel].path).c_str());
            if(srchentrs[srchsel].islink){
                printw(canon_selec(srchpth, srchentrs, srchsel).c_str());
            }else if(srchentrs[srchsel].isdir){
                printw("/");
            }
        }
        attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));
        win->_curx = strpos + 2;
        refresh();
    }
    over:;
}

inline bool can_read(const path& pth){
    return exists(pth) && (root || status(pth).permissions() & perms::group_read);
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
