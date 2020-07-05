#ifndef FEXPCONFIG_H
#define FEXPCONFIG_H
#include <fstream>
#include <sstream>

namespace fexpconf{
    using std::string;
    
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
        *static_cast<string*>(valptr) = trim.substr(1, trim.size() - 2);
    }
    
    //TODO implement rest of config types
    void (*optionloader[4])(void*, string&) = {
        loadconf_bool,
        loadconf_int,
        loadconf_float,
        loadconf_string
    };
    
    /* * * * */
    
    bool show_hidden = true;
    int dummy_int;
    float dummy_float;
    string dummy_string;
    
    //TODO load config option map from file? maybe
    const int confoptcnt = 4;
    confdesc confoptions[confoptcnt] = {
        {"[Show Hidden Files]", CONFTYPE_BOOL, &show_hidden},
        {"[Dummy Int Option]", CONFTYPE_INT, &dummy_int},
        {"[Dummy Float Option]", CONFTYPE_FLOAT, &dummy_float},
        {"[Dummy String Option]", CONFTYPE_STRING, &dummy_string}
    };
    
    void init_conf(){
        
    }
    
    void load_conf(){
        using namespace std;
        
        //for release
//        ifstream cfile("/usr/share/fexp/fexp.conf");
        
        //for testing
        ifstream cfile("fexp.conf");
        
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
    }
    
    void save_conf(){
        
    }
}

#endif /* FEXPCONFIG_H */

