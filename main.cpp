/* * FEXP * */
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <fstream>
#include "sys/stat.h"
#include "fexpconfig.h"
#include "fexpmicro.h"

#define PAIR_BLANK 0
#define PAIR_NOREAD 1
#define PAIR_BLANK_SELECTED 2
#define PAIR_NOREAD_SELECTED 3

using namespace std;
using namespace boost;
using namespace boost::filesystem;

//const char* DEL_CHAR = "\b \b";

void print_dir();
void list_dir(path& pth, vector<dirent>& pathentrs);
void string_search(bool& lp);
inline bool can_read(const path& pth);
inline bool can_write(const path& pth);
void exit_path(bool& lp, bool& refr);
void enter_path(bool& lp, bool& refr);
void char_result();
string get_string_input(string msg);
void print_info();

int main(int argc, char** args){
    root = !getuid();
    
    fexpconf::validate_conf();
    fexpconf::load_conf();
    
    pth = argc > 1 ? string(args[1]) : current_path();
    if(pth != "/") pth += "/";
    
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
        
        /* * KEYS * */
        switch(cs){
        case 'q':
            goto endo;
            break;
        //up
        case 'w':
            goup:;
            tickup();
            refr = true;
            break;
        case KEY_UP:
//            tickup();
//            refr = true;
//            break;
            goto goup;
        case 'W':
            tickup3();
            refr = true;
            break;
        //down
        case 's':
            godn:;
            tickdown();
            refr = true;
            break;
        case KEY_DOWN:
//            tickdown();
//            refr = true;
//            break;
            goto godn;
        case 'S':
            tickdown3();
            refr = true;
            break;
        //enter dir
        case '\n':
            go_right(lp, refr, appn, jmp);
            break;
        case 'd':
            go_right(lp, refr, appn, jmp);
            break;
        case KEY_RIGHT:
            go_right(lp, refr, appn, jmp);
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
        case 4: //^d
            strget = get_string_input("Create directory: ");
            
            if(!strget.empty()){
                path mkpath = pth;
                mkpath /= strget;
                create_directory(mkpath);
                lp = true;
            }
            refr = true;
            break;
        case 6: //^f
            strget = get_string_input("Create file: ");
            
            if(!strget.empty()){
                path mkpath = pth;
                mkpath /= strget;
                filesystem::ofstream nfile(mkpath.string());
                lp = true;
            }
            refr = true;
            break;
        case KEY_HOME:
            jump_to(getenv("HOME"), true, refr, lp);
            break;
        case '/':
            jump_to("/", false, refr, lp);
            break;
        case 18: //^r
            delfile:;
            strget = get_string_input("Delete? [y/n] ");
            
            if(strget == "Y" || strget == "y"){
                path rmfile = pth;
                rmfile /= pathentrs[selec].path;
                        
                if(can_write(rmfile)){
                    if(!filesystem::is_empty(rmfile)){
                        if(fexpconf::prompt_delall){
                            strget = get_string_input("Directory is not empty. Delete anyway? [y/n] ");
                        }else{
                            strget = "y";
                        }
                        
                        if(strget == "Y" || strget == "y"){
                            remove_all(rmfile);
                            lp = true;
                        }
                    }else{
                        remove(rmfile);
                        lp = true;
                    }
                }
            }
            refr = true;
            break;
        case KEY_DC:
            goto delfile;
        case 263: //^h
            targselec = pathentrs[selec].path;
            fexpconf::show_hidden = !fexpconf::show_hidden;
            lp = refr = true;
            break;
        //TODO add go back command
        //go back
//        case 0407:
//            goback:;
//            pop_path;
//            lp = refr = true;
//            break;
//        case '\b':
//            goto goback;
        //TODO add open file command
        }
        
        //enter directory
        string jfile;
        
        if(appn && pathentrs.size() > 0){
            push_path();
            enter_path(lp, refr);
        }else if(jmp && pathentrs.size() > 0){
            save_selec();
            push_path();
            
            pth /= pathentrs[selec].path;
            pth = canonical(pth);
            jfile = pth.filename().string();
            pth = pth.parent_path();
            pth += "/";
            
            load_selec();
        }
        
        if(lp){
            list_dir(pth, pathentrs);
            if(!targselec.empty()){
                for(int i = 0; i < pathentrs.size() - 1; i++){
                    if(pathentrs[i].path == targselec){
                        selec = i;
                        targselec.clear();
                        break;
                    }
                }
            }
        }
        
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
    
    clear();
    endwin();
    
    return 0;
}

string get_string_input(string msg){
    string input;
    int strpos = 0, c;
    
    win->_curx = 0;
    win->_cury = win->_maxy;
    printw("\n");
    
    attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
    printw(msg.c_str());
    attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));
    
    refresh();
    
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
    
    print_info();
    
    //bot line
    win->_cury = win->_maxy;
    win->_curx = 0;
    
    printw("\n");
    attron(COLOR_PAIR(PAIR_BLANK_SELECTED));
    printw(selecpath.c_str());
    attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));
    
    refresh();
}

//TODO finish file/dir information section
void print_info(){
    int halfw = win->_maxx / 2;
    dirent& selp = pathentrs[selec];
    string y = "Yes";
    string n = "No";
    
    win->_curx = halfw;
    win->_cury = 1;
    printw(("# File Path: " + selp.rpath.string()).c_str());
    
    win->_curx = halfw;
    win->_cury = 2;
    printw(("# Normal File: " + (selp.isnorm ? y : n)).c_str());
    
    win->_curx = halfw;
    win->_cury = 3;
    printw(("# Directory: " + (selp.isdir ? y : n)).c_str());
    
    win->_curx = halfw;
    win->_cury = 4;
    printw(("# Symlink: " + (selp.islink ? y : n)).c_str());
    
    win->_curx = halfw;
    win->_cury = 5;
    printw(("# Last Modified: " + to_string(selp.lastmodtime)).c_str());
}

void list_dir(path& pth, vector<dirent>& pathentrs){
    pathentrs.clear();
    directory_iterator end;
    win->_curx = 0;
    win->_cury = 0;
    for(directory_iterator it(pth); it != end; it++){
        if(!fexpconf::show_hidden && it->path().filename().string()[0] == '.') continue;
        
        bool isnorm;
        bool isdir;
        bool islink;
        bool canread;
        long modtime;
        
        try{
            isnorm = is_regular(it->path());
            isdir = is_directory(it->path());
            islink = is_symlink(it->path());
            canread = can_read(it->path());
            modtime = last_write_time(it->path());
        }catch(...){
            isnorm = false;
            isdir = false;
            islink = false;
            canread = false;
            modtime = 0;
        }

        pathentrs.push_back({it->path(), it->path().filename().string(), isnorm, isdir, islink,
            canread, modtime});
    }
    
    sort(pathentrs.begin(), pathentrs.end(), comp_pentr);
}

//TODO fix char result
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

                save_selec();
                    
                if(srchentrs[srchsel].isdir){
                    fnd /= srchentrs[srchsel].path;
//                    fnd += "/";
                    selec = pathselnum[fnd.string()];
                    lp = true;
                }else{
                    pathentrs = srchentrs;
                    selec = srchsel;
                }

                push_path();
                pth = canonical(fnd);
                pth += "/";
                goto over;
            }else if(input == "/"){
                push_path();
                pth = "/";
                lp = true;
                goto over;
            }else if(input == "~"){
                push_path();
                pth = getenv("HOME");
                pth += "/";
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
                //TODO replace with fork
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

inline bool can_write(const path& pth){
    return exists(pth) && (root || status(pth).permissions() & perms::group_write);
}

void exit_path(bool& lp, bool& refr){
    if(exists(pth.parent_path())){
        save_selec();
        
        pth = pth.parent_path().parent_path();
        if(pth != "/") pth += "/";
        lp = refr = true;
        
        load_selec();
    }
}

void enter_path(bool& lp, bool& refr){
    if(pathentrs[selec].canread){
        save_selec();

        if(pathentrs[selec].islink){
            pth /= pathentrs[selec].path;
            pth = canonical(pth);
        }else{
            pth /= (pathentrs[selec].path);
        }
        
        if(pth != "/") pth += "/";

        load_selec();
    }else{
        lp = false;
        refr = false;
    }
}
