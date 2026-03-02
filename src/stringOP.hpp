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

#ifndef STRINGOP_HPP
#define STRINGOP_HPP
#include <iostream>
#include <vector>
#include <string>
#include <any>
#include "includes.hpp"

struct index{
    char open;
    char close;
    int index1;
    int index2;
    bool error;
    std::string errorMSG;
    void printD(){
        std::cout << "char1: '" << open << "' char2: '" << close << "' start: " << index1 << " end: " << index2 << "\n";
    }
};

index cutCH(char open, char close, int indexst, std::vector<Token> code){
    int i = indexst;
    int size = code.size();
    index ret;
    ret.open = open;
    ret.close = close;
    ret.error = false;
    int i_c = 0;
    while (i<size){
        if (std::any_cast<std::string>(code[i].data[1])[0] == open && std::any_cast<std::string>(code[i].data[0]) == "keyword"){
            if (!i_c){
                ret.index1 = i+1;
            }
            i_c++;
        }else if(std::any_cast<std::string>(code[i].data[1])[0] == close && std::any_cast<std::string>(code[i].data[0]) == "keyword"){
            i_c--;
            if (!i_c){
                ret.index2 = i-1;
                return ret;
            }
        }
        i++;
    }
    ret.error = true;
    ret.errorMSG = "SYNTAX ERROR";
    return ret;
}

#endif
