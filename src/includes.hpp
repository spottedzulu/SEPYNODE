#ifndef INCLUDES_HPP
#define INCLUDES_HPP
#include <iostream>
#include <vector>
#include <any>
#include "spndlib.hpp"

#if defined(_WIN32)
    #include <windows.h>
    struct dlib{
        std::string name;
        HINSTANCE hDll;
        int id;
    };
#elif defined(__linux__)
    #include <dlfcn.h>
    struct dlib{
        std::string name;
        void* handle;
        int id;
    };
#endif



struct Venv;
struct Var;

static int lastid = 0;

int getnewid(){
    return lastid++;
}


//struct object{
//    Venv objectvenv;
//};

template <typename T>
bool isIndexInBounds(const std::vector<T>& vec, int i) {
    return (i >= 0 && i < vec.size());
}


template <typename T>
void removeAtIndex(std::vector<T>& vec, int index) {
    if (index >= 0 && index < vec.size()) {
        for (int i = index; i < vec.size() - 1; ++i) {
            vec[i] = vec[i + 1];
        }
        vec.pop_back();
        
    } else {
        std::cerr << "Index out of bounds." << std::endl;
    }
}


struct Token{
    //std::string type;
    std::vector<std::any> data;
    bool init = true;
    Token(bool init_state=true){
        init = init_state;
    }

    //void print(){
    //    std::cout << "Token:\n\ttype: '" << type << "'\n";
    //}
};


struct Var{
    std::string name;
    Token data;

    void print(){
        std::cout << "Var:\n\tname: '" << name << "'\n";
        //data.print();
    }
};

struct Venv{
    std::vector<Var> vars;
    std::vector<Venv> objects;
    Venv* parent;
    Token operatorINIT = Token(false);
    Token operatorSTR = Token(false);
    Token operatorPLUS = Token(false);
    Token operatorMINUS = Token(false);
    Token operatorMULT = Token(false);
    Token operatorDIV = Token(false);
    Token operatorMOD = Token(false);
    Token operatorPOWER = Token(false);
    Token operatorAND = Token(false);
    Token operatorOR = Token(false);
    Token operatorNOT = Token(false);
    //std::vector<dlib> dlibs;

    bool varin(std::string varN){
        int i = 0;
        while (i < vars.size()){
            if (varN == vars[i].name){
                return true;
            }
            i++;
        }
        return false;
    }
    
    int varindex(std::string varN){
        int i = 0;
        while (i < vars.size()){
            if (varN == vars[i].name){
                return i;
            }
            i++;
        }
        std::cout << "ERROR: VAR INDEX OUT OF RANGE\n";
        exit(-2);
    }
};

typedef transferableToken (*dlibfunc)(std::vector<transferableToken>, sepynodedata);


#define PRINT_OUTPUT 0x00
#define PRINT_ERROR 0x01

#define PRINT_BLACK 0
#define PRINT_RED 1
#define PRINT_GREEN 2
#define PRINT_YELLOW 3
#define PRINT_BLUE 4
#define PRINT_MAGENTA 5
#define PRINT_CYAN 6
#define PRINT_WHITE 7



void print(std::string message="", int color=PRINT_WHITE, int type=PRINT_OUTPUT, std::string end="\n"){
    std::string colorstr;
    if (color==PRINT_BLACK){
        colorstr = "\033[30m";
    }else if (color==PRINT_RED){
        colorstr = "\033[31m";
    }else if (color==PRINT_GREEN){
        colorstr = "\033[32m";
    }else if (color==PRINT_YELLOW){
        colorstr = "\033[33m";
    }else if (color==PRINT_BLUE){
        colorstr = "\033[34m";
    }else if (color==PRINT_MAGENTA){
        colorstr = "\033[35m";
    }else if (color==PRINT_CYAN){
        colorstr = "\033[36m";
    }else if (color==PRINT_WHITE){
        colorstr = "\033[0m";
    }

    if (type==PRINT_OUTPUT){
        std::cout << colorstr << message << "\033[0m" << end;
    }else if (type==PRINT_ERROR){
        std::cout << "\033[31m" << message << "\033[0m" << end;
    }
}

template <typename T> 
std::vector<T> getSubvector(const std::vector<T>& vec, int start, int end) {
    if (start < 0 || end >= vec.size() || start > end) { 
        throw std::out_of_range("Invalid range"); 
    }
    std::vector<T> subvector(vec.begin() + start, vec.begin() + end + 1); 
    return subvector; 
}

std::vector<Var*> getvars(Venv* venv){
    std::vector<Var*> vars;
    vars.reserve(venv->vars.size());
    int i = 0;
    while (i!=venv->vars.size()){
        vars.push_back(&venv->vars[i]);
        i++;
    }
    //vars = venv->vars;
    if (venv->parent != NULL){
        std::vector<Var*> parentvars = getvars(venv->parent);
        for (int i = 0;i!=parentvars.size();i++){
            vars.push_back(parentvars[i]);
        }
    }

    return vars;
}

bool varinlist(std::vector<Var*> vars, std::string varname){
    for(int i = 0;i!=vars.size();i++){
        if (vars[i]->name == varname){
            return true;
        }
    }
    return false;
}

int varinlistindex(std::vector<Var*> vars, std::string varname){
    for(int i = 0;i!=vars.size();i++){
        if (vars[i]->name == varname){
            return i;
        }
    }
    return 0;
}


#endif
