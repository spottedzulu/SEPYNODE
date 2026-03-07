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

#include "./../spndlib.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <cmath>
#include <filesystem>

void SpnWarning(std::string message){
    std::cerr << "SEPYNODE RUNTIME WARNING: " << message << "\n";
}

void SpnError(std::string message){
    std::cerr << "SEPYNODE RUNTIME ERROR: " << message << "\n";
    exit(-1);
}

std::any GetTypeFromToken(transferableToken t){
    return t.data.at(0);
}

bool IsValidFloat(const std::string& str) {
    if (str.empty()) return false;
    char* endptr;
    errno = 0;
    std::strtof(str.c_str(), &endptr);
    return endptr != str.c_str() && *endptr == '\0' && errno != ERANGE;
}

#if defined(_WIN32)
#include <windows.h>

extern "C" __declspec(dllexport) transferableToken SPN_STD_TYPE(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"str");
    Token.data.push_back(GetTypeFromToken(input.at(0)));
    return Token;
}

extern "C" __declspec(dllexport) transferableToken SPN_STD_STOI(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    transferableToken Token;
    std::string TType = std::any_cast<std::string>(GetTypeFromToken(input.at(0)));
    if(TType == "int" || TType == "str"){
        if (!IsValidFloat(std::any_cast<std::string>(input.at(0).data.at(1)))){
            SpnError("DLIB runtime error: Conversion cannot be completed!");
        }
        Token.data.push_back((std::string)"int");
        Token.data.push_back(input.at(0).data.at(1));
    }else{
        SpnError("DLIB runtime error: Unsupported conversion type!");
    }
    return Token;
}

extern "C" __declspec(dllexport) transferableToken SPN_STD_ITOS(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"str");
    Token.data.push_back(input.at(0).data.at(1));
    return Token;
}

extern "C" __declspec(dllexport) transferableToken SPN_STD_SYSTEM_WARNING(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    if (std::any_cast<std::string>(input.at(0).data.at(0)) == "str" || std::any_cast<std::string>(input.at(0).data.at(0)) == "int"){
        SpnWarning(std::any_cast<std::string>(input.at(0).data.at(1)));
    }else{
        SpnError("DLIB runtime error: Unsupported warning print type!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"str");
    Token.data.push_back((std::string)"None");
    return Token;
}

extern "C" __declspec(dllexport) transferableToken SPN_STD_SYSTEM_ERROR(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    if (std::any_cast<std::string>(input.at(0).data.at(0)) == "str" || std::any_cast<std::string>(input.at(0).data.at(0)) == "int"){
        SpnError(std::any_cast<std::string>(input.at(0).data.at(1)));
    }else{
        SpnError("DLIB runtime error: Unsupported error print type!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"str");
    Token.data.push_back((std::string)"None");
    return Token;
}

extern "C" __declspec(dllexport) transferableToken SPN_STD_SYSTEM_EXIT(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    if (std::any_cast<std::string>(input.at(0).data.at(0)) == "int"){
        exit(std::stoi(std::any_cast<std::string>(input.at(0).data.at(1))));
    }else{
        SpnError("DLIB runtime error: Unsupported exit type code, expected int!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"str");
    Token.data.push_back((std::string)"None");
    return Token;
}

extern "C" __declspec(dllexport) transferableToken SPN_STD_GET_INPUT(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    transferableToken Token;
    std::string inputdata;
    std::getline(std::cin, inputdata);
    Token.data.push_back((std::string)"str");
    Token.data.push_back(inputdata);
    return Token;
}

extern "C" __declspec(dllexport) transferableToken SPN_STD_SQRT(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"int");
    transferableToken firstinput = input.at(0);
    float data;
    if (std::any_cast<std::string>(GetTypeFromToken(firstinput)) == "int"){
        data = std::stof(std::any_cast<std::string>(firstinput.data.at(1)));
    }else{
        SpnError("DLIB runtime error: Expected int!");
    }
    float result;
    try {
        result = sqrt(data);
        if (std::isnan(result)) {
            SpnError("Negative number!");
        }
    } catch (const std::exception& e) {
        SpnError("Error: " + (std::string)e.what());
    }
    Token.data.push_back((std::string)std::to_string(result));
    return Token;
}

extern "C" __declspec(dllexport) transferableToken SPN_STD_FILESYSTEM_EXISTS(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"int");
    transferableToken path = input.at(0);
    int result;
    if (std::any_cast<std::string>(GetTypeFromToken(path)) == "str"){
        std::filesystem::path fspath(std::any_cast<std::string>(path.data.at(1)));
        result = std::filesystem::exists(fspath);
    }else{
        result = -1;
        SpnError("DLIB runtime error: Expected string!");
    }
    Token.data.push_back(std::to_string(result));
    return Token;
}

#elif defined(__linux__)
#include <cstdlib>

extern "C" transferableToken SPN_STD_TYPE(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"str");
    Token.data.push_back(GetTypeFromToken(input.at(0)));
    return Token;
}

extern "C" transferableToken SPN_STD_STOI(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    transferableToken Token;
    std::string TType = std::any_cast<std::string>(GetTypeFromToken(input.at(0)));
    if(TType == "int" || TType == "str"){
        if (!IsValidFloat(std::any_cast<std::string>(input.at(0).data.at(1)))){
            SpnError("DLIB runtime error: Conversion cannot be completed!");
        }
        Token.data.push_back((std::string)"int");
        Token.data.push_back(input.at(0).data.at(1));
    }else{
        SpnError("DLIB runtime error: Unsupported conversion type!");
    }
    return Token;
}

extern "C" transferableToken SPN_STD_ITOS(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"str");
    Token.data.push_back(input.at(0).data.at(1));
    return Token;
}

extern "C" transferableToken SPN_STD_WARNING(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    if (std::any_cast<std::string>(input.at(0).data.at(0)) == "str" || std::any_cast<std::string>(input.at(0).data.at(0)) == "int"){
        SpnWarning(std::any_cast<std::string>(input.at(0).data.at(1)));
    }else{
        SpnError("DLIB runtime error: Unsupported warning print type!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"str");
    Token.data.push_back((std::string)"None");
    return Token;
}

extern "C" transferableToken SPN_STD_ERROR(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    if (std::any_cast<std::string>(input.at(0).data.at(0)) == "str" || std::any_cast<std::string>(input.at(0).data.at(0)) == "int"){
        SpnError(std::any_cast<std::string>(input.at(0).data.at(1)));
    }else{
        SpnError("DLIB runtime error: Unsupported error print type!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"str");
    Token.data.push_back((std::string)"None");
    return Token;
}

extern "C" transferableToken SPN_STD_EXIT(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    if (std::any_cast<std::string>(input.at(0).data.at(0)) == "int"){
        exit(std::stoi(std::any_cast<std::string>(input.at(0).data.at(1))));
    }else{
        SpnError("DLIB runtime error: Unsupported exit type code, expected int!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"str");
    Token.data.push_back((std::string)"None");
    return Token;
}

extern "C" transferableToken SPN_STD_SQRT(std::vector<transferableToken> input, sepynodedata spndata){
    if (spndata.spnversion < 2.0){
        SpnError("DLIB runtime error: Unsupported sepynode version (req: spn_v>=2, got: spn<2)!");
    }
    transferableToken Token;
    Token.data.push_back((std::string)"int");
    transferableToken firstinput = input.at(0);
    float data;
    if (std::any_cast<std::string>(GetTypeFromToken(firstinput)) == "int"){
        data = std::stof(std::any_cast<std::string>(firstinput.data.at(1)));
    }else{
        SpnError("DLIB runtime error: Expected int!");
    }
    
    float result;
    try {
        result = sqrt(data);
        if (std::isnan(result)) {
            SpnError("Negative number!");
        }
    } catch (const std::exception& e) {
        SpnError("Error: " + (std::string)e.what());
    }
    Token.data.push_back((std::string)std::to_string(result));
    return Token;
}

#endif
