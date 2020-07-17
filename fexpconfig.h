#ifndef FEXPCONFIG_H
#define FEXPCONFIG_H
#include <fstream>
#include <sstream>
#include <boost/filesystem/operations.hpp>
#include "curl/curl.h"

namespace fexpconf{
using namespace std;
using namespace boost::filesystem;

//const char* FEXP_CONF_PATH = "/usr/share/fexp/fexp.conf";//for release
const char* FEXP_CONF_PATH = "fexp.conf";//for testing
const char* FEXP_CONF_URL = "https://raw.githubusercontent.com/Serj-0/fexp/master/fexp.conf";

enum confvaltype{
    CONFTYPE_BOOL = 0,
    CONFTYPE_INT = 1,
    CONFTYPE_FLOAT = 2,
    CONFTYPE_STRING = 3
};

struct confdesc{
    string id;
    confvaltype type;
    void* val;
};

bool confloaddone = false;

string trimmed(string& str){
    return str.substr(str.find_first_not_of(' '), str.find_last_not_of(' '));
}

void loadconf_bool(void* valptr, string& str){
    string trim = trimmed(str);
    *static_cast<bool*>(valptr) = trim == "true" || trim == "1";
}

void loadconf_int(void* valptr, string& str){
    string trim = trimmed(str);
    *static_cast<int*>(valptr) = stoi(trim);
}

void loadconf_float(void* valptr, string& str){
    string trim = trimmed(str);
    *static_cast<float*>(valptr) = stof(trim);
}

void loadconf_string(void* valptr, string& str){
    string trim = trimmed(str);
    *static_cast<string*>(valptr) = trim;
}

template<typename T>
void saveconf_opt(void* valptr, ofstream& ost){
    ost << *static_cast<T*>(valptr);
}

void (*optionloader[4])(void*, string&) = {
    loadconf_bool,
    loadconf_int,
    loadconf_float,
    loadconf_string
};

void (*optionsaver[4])(void*, ofstream&) = {
    saveconf_opt<bool>,
    saveconf_opt<int>,
    saveconf_opt<float>,
    saveconf_opt<string>
};

/* * * * */

string conf_version;
bool show_hidden = true;
bool prompt_delall = true;
int hist_max_size;

map<string, string> file_assoc;

/* * * * */
void load_conf();
void save_conf();

//TODO more configuration options
const int confoptcnt = 4;
confdesc confoptions[confoptcnt] = {
    {"[Config Version]", CONFTYPE_STRING, &conf_version},
    {"[Show Hidden Files]", CONFTYPE_BOOL, &show_hidden},
    {"[Prompt Recursive Deletion]", CONFTYPE_BOOL, &prompt_delall},
//move deleted files to ~/local/share/Trash    {""}
    {"[Max History Size]", CONFTYPE_INT, &hist_max_size}
};

string cfg_pulled;
size_t cfg_curl_write(char* ptr, size_t size, size_t nmemb, void* data){
    cfg_pulled.append(ptr);
    return size * nmemb;
}

void validate_conf(){
    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, FEXP_CONF_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cfg_curl_write);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    istringstream cfgstrst(cfg_pulled);
    string cfg_pull_ver;
    
    cfgstrst.ignore(16);
    cfgstrst >> cfg_pull_ver;

    if(!exists("/usr/share/fexp")) create_directories("/usr/share/fexp");

    bool dlconf = false;

    if(!exists(FEXP_CONF_PATH)){
        dlconf = true;
    }else{
        load_conf();
        dlconf = *static_cast<string*>(confoptions[0].val) != cfg_pull_ver;
        *static_cast<string*>(confoptions[0].val) = cfg_pull_ver;
    }

    if(dlconf){
        ofstream confdlst(FEXP_CONF_PATH);
        confdlst << cfg_pulled;
        confdlst.close();
    }
}

void load_conf(){
    ifstream cfile(FEXP_CONF_PATH);

    string line;
    while(getline(cfile, line)){
        int brpos = line.find_first_of(']') + 1;
        string confid = line.substr(0, brpos);

        for(int i = 0; i < confoptcnt; i++){
            if(confid == confoptions[i].id){
                string confval = line.substr(brpos);
                optionloader[(int) confoptions[i].type](confoptions[i].val, confval);
                break;
            }
        }
    }

    cfile.close();
    confloaddone = true;
}

void save_conf(){
    ofstream cfile(FEXP_CONF_PATH);
    
    for(int i = 0; i < confoptcnt; i++){
        cfile << confoptions[i].id << " ";
        optionsaver[confoptions[i].type](confoptions[i].val, cfile);
        cfile << "\n";
    }
    
    cfile.close();
}

}

#endif /* FEXPCONFIG_H */
