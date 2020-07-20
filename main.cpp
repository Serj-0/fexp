/* * FEXP * */
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <fstream>
#include "fexpconfig.h"
#include "fexpmicro.h"
#include <boost/algorithm/string.hpp>
using namespace std;
using namespace boost;
using namespace boost::filesystem;

#define PAIR_BLANK 0
#define PAIR_NOREAD 1
#define PAIR_BLANK_SELECTED 2
#define PAIR_NOREAD_SELECTED 3

void print_dir();
void list_dir(path& pth, vector<dirent>& pathentrs);
void string_search();
inline bool can_read(const path& pth);
inline bool can_write(const path& pth);
void exit_path();
void enter_path();
void char_result();
string get_string_input(string msg);
void print_info();

int main(int argc, char** args){
    atexit(fexpconf::save_conf);
    
    root = !getuid();

    fexpconf::validate_conf();
    if(!fexpconf::confloaddone) fexpconf::load_conf();

    pth = argc > 1 ? string(args[1]) : current_path();
    if(pth != "/") pth += "/";

    win = initscr();
    start_color();
    noecho();
    keypad(win, true);
    set_escdelay(0);

    init_pair(PAIR_BLANK, COLOR_WHITE, COLOR_BLACK);
    init_pair(PAIR_BLANK_SELECTED, COLOR_WHITE, COLOR_BLUE);
    init_pair(PAIR_NOREAD, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_NOREAD_SELECTED, COLOR_RED, COLOR_BLUE);

    push_path();
    
    int cs;

    list_dir(pth, pathentrs);
    print_dir();

    string strget;

    while(cs = getch()){
        refr = false;
        lp = false;
        appn = false;
        jmp = false;

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
            goto goup;
        case 'W':
            goup3:;
            tickup3();
            refr = true;
            break;
            //down
        case 337:
            goto goup3;
        case 's':
            godn:;
            tickdown();
            refr = true;
            break;
        case KEY_DOWN:
            goto godn;
        case 'S':
            godn3:;
            tickdown3();
            refr = true;
            break;
        case 336:
            goto godn3;
            //enter dir
        case '\n':
            go_right();
            break;
        case 'd':
            go_right();
            break;
        case KEY_RIGHT:
            go_right();
            break;
            //exit dir
        case 'a':
            exit_path();
            break;
        case KEY_LEFT:
            exit_path();
            break;
            //next page
        case KEY_NPAGE:
            selec = ceil((static_cast<float> (selec) + 1) / (win->_maxy - 1)) * (win->_maxy - 1);
            selec = selec >= pathentrs.size() ? pathentrs.size() - 1 : selec;
            refr = true;
            break;
            //prev page
        case KEY_PPAGE:
            selec = floor(static_cast<float> (selec) / (win->_maxy - 1) - 1) * (win->_maxy - 1);
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
            string_search();
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
            set_working_directory(getenv("HOME"), "", false, false, false, true, true);
            break;
        case '/':
            set_working_directory("/", "", false, false, false, true, true);
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
                            strget = "Y";
                        }

                        if(strget == "Y" || strget == "y"){
                            if(fexpconf::soft_del){
                                copy_directory(rmfile,
                                        getenv("HOME") + string("/.local/share/Trash/files/" + rmfile.filename().string()));
                                gen_trash_info(rmfile);
                            }
                            
                            remove_all(rmfile);
                            lp = true;
                        }
                    }else{
                        if(fexpconf::soft_del){
                            copy_file(rmfile,
                                    getenv("HOME") + string("/.local/share/Trash/files/" + rmfile.filename().string()));
                            gen_trash_info(rmfile);   
                        }
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
            //go back
        case '<':
            hist_go_back();
            break;
        case '>':
            hist_go_forward();
            break;
        case 15://^O
            string ext = pathentrs[selec].rpath.extension().string();
            to_lower(ext);
            string assoc_prog = fexpconf::file_assoc[ext];
            
            if(assoc_prog.empty()){
                string ncmd = get_string_input("No file association for " + ext + ", "
                        "enter a command to use for this file type: ");
                to_lower(ncmd);
                fexpconf::file_assoc[ext] = ncmd;
                assoc_prog = ncmd;
            }
            
            system_call("cd" + pth.string() + ";" + assoc_prog + " " + pathentrs[selec].rpath.string());
            
            break;
        }

        //enter directory
        if(appn && pathentrs.size() > 0){
            set_working_directory(pth, pathentrs[selec].path, true, false, false, true, true);
        }else if(jmp && pathentrs.size() > 0){
            set_working_directory(pathentrs[selec].rpath, "", false, true, pathentrs[selec].isnorm, true, true,
                    canonical(pathentrs[selec].rpath).filename().string());
        }

        if(lp){
            list_dir(pth, pathentrs);
            if(!targselec.empty()){
                for(int i = 0; i < pathentrs.size(); i++){
                    if(pathentrs[i].path == targselec){
                        selec = i;
                        targselec.clear();
                        break;
                    }
                }
                targselec.clear();
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
        case 27:
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
        win->_curx = win->_maxx - 15;
        printw("'Esc' to cancel");
        win->_curx = 0;
        attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));

        win->_curx = msg.size() + strpos;

        refresh();
    }

    over:;
    return "";
}

void print_dir(){
    win->_curx = win->_cury = 0;
    printw("\n");
    win->_curx = win->_cury = 0;

    string current = "|| " + pth.string() + " || " + to_string(selec + 1) + "\\" + to_string(pathentrs.size()) + " ||";
    
    if(current.size() >= win->_maxx - 35){
        current = current.substr(current.size() - (win->_maxx - 38));
        current = "..." + current;
    }
    
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

void print_info(){
    int halfw = win->_maxx / 2;
    dirent& selp = pathentrs[selec];
    string y = "Yes";
    string n = "No";

    win->_curx = halfw;
    win->_cury = 1;
    
    string fpstr = "# File Path: ";
    string fpfp = selp.rpath.string();
    
    if(fpstr.size() + fpfp.size() > halfw){
        fpfp = fpfp.substr((fpstr.size() + fpfp.size()) - halfw + 3);
        fpfp = "..." + fpfp;
    }
    
    printw((fpstr + fpfp).c_str());

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
    printw(("# Last Modified: " + string(asctime(localtime(&selp.lastmodtime)))).c_str());
    
    string sz;
    
    if(!selp.isdir && selp.canread){
        uintmax_t fsize = file_size(selp.rpath);
        
        int tpi = 0;
        int factor = 1024 * 1024 * 1024;
        while(factor){
            long long div = fsize / factor;
            if(div){
                double decs = static_cast<double>(fsize) / static_cast<double>(factor);
                sz = to_string(decs) + data_size_units[tpi];
                break;
            }
            tpi++;
            factor /= 1024;
        }
    }else{
        sz = "N/A";
    }
    
    win->_curx = halfw;
    win->_cury = 6;
    printw(("# File Size: " + sz).c_str());
    
    for(int i = 7; i < win->_maxy; i++){
        win->_curx = halfw;
        win->_cury = i;
        printw("#");
    }
}

void list_dir(path& pth, vector<dirent>& pathentrs){
    pathentrs.clear();
    if(!can_read(pth)) return;
    
    directory_iterator end;
    win->_curx = 0;
    win->_cury = 0;
    for(directory_iterator it(pth); it != end; it++){
        if(!fexpconf::show_hidden && it->path().filename().string()[0] == '.') continue;

        bool isnorm;
        bool isdir;
        bool islink;
        bool canread;
        long long modtime;

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

void char_result(){
    int c = 0;
    char cc;
    char* ccc = new char[2];
    ccc[1] = '\0';

    win->_cury = win->_maxy;
    win->_curx = 0;
    printw("\n");
    win->_curx = 0;

    while(c = getch()){
        if(c == '`') break;

        win->_cury = win->_maxy;
        win->_curx = 0;
        printw("\n");
        cc = c;
        ccc[0] = c;

        printw(ccc);
        printw((" : " + to_string(c)).c_str());
    }

    delete ccc;
}

void string_search(){
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
        case 27:
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

                if(srchentrs[srchsel].canread && srchentrs[srchsel].isdir){
                    fnd /= srchentrs[srchsel].path;
                    set_working_directory(fnd, "", false, true, false, true, true);
                }else{
                    pathentrs = srchentrs;
                    set_working_directory(fnd, "", false, true, false, false, true);
                    selec = srchsel;
                }

                goto over;
            }else if(input == "/"){
                set_working_directory("/", "", false, false, false, true, true);
                goto over;
            }else if(input == "~"){
                set_working_directory(getenv("HOME"), "", false, false, false, true, true);
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
                
                system_call("cd " + pth.string() + ";" + cmd);
                
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
        bool gooddir = false;
        
        if(!fullpath.empty()){
            if(fullpath[fullpath.size() - 1] == '/'){
                int slashpos = fullpath.find_last_of('/', fullpath.size() - 2) + 1;
                fname = fullpath.substr(slashpos, fullpath.size() - slashpos - 1);
                if(srchpth.string() != "/") srchpth = srchpth.parent_path().parent_path();
            }else{
                fname = fullpath.substr(fullpath.find_last_of('/', fullpath.size() - 1) + 1);
                srchpth = srchpth.parent_path();
            }

            if((gooddir = can_read(srchpth) && is_directory(srchpth), gooddir)){
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
        win->_curx = win->_maxx - 15;
        printw("'Esc' to cancel");
        win->_curx = 0;
        attroff(COLOR_PAIR(PAIR_BLANK_SELECTED));

        printw(("||" + input).c_str());

        attron(COLOR_PAIR(PAIR_BLANK_SELECTED + !gooddir));
        printw(("||" + srchpth.string()).c_str());
        if(srchsel > -1){
            printw((" : " + srchentrs[srchsel].path).c_str());
            if(srchentrs[srchsel].islink){
                printw(canon_selec(srchpth, srchentrs, srchsel).c_str());
            }else if(srchentrs[srchsel].isdir){
                printw("/");
            }
        }
        attroff(COLOR_PAIR(PAIR_BLANK_SELECTED + !gooddir));
        win->_curx = strpos + 2;
        refresh();
    }
    over:;
}
