/*
This file is part of SEPYNODE.
Copyright (C) 2026 SpottedZulu/SpottedZulu2217

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
