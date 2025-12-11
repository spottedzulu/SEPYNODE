#ifndef SPNDLLLIB_HPP
#define SPNDLLLIB_HPP
#include <iostream>
#include <vector>
#include <any>

int SPNDLLLIB_HPP_VERSION = 1;

struct transferableToken{
    //std::string type;
    //std::string data;
    std::vector<std::any> data;
    //int originLn;
    //int originCol;
    
    
};

struct sepynodedata{
    int spnversion;
    void* interpreterpointer;
    void* evalpointer;
    int spndlibversion = SPNDLLLIB_HPP_VERSION;
    //void* venv;
    void* Mvenv;

};



#endif
