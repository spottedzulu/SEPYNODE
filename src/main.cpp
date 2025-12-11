#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <iomanip>
#include <libgen.h>
#include "includes.hpp"
#include "stringOP.hpp"
#include "spndlib.hpp"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
    #include <dlfcn.h>
    #include <unistd.h>
#endif


static std::vector<dlib> dlibs;
static sepynodedata spnglobaldata;

void interpret(std::vector<Token> code, Venv* venv, std::string path);

void freedlibs(){
    for (dlib element: dlibs) {
        //std::cout << "name: " << element.name << ", id: " << element.id << "\n";
        #if defined(_WIN32)
            //FreeLibrary(dlibs.at(i).hDll);
            FreeLibrary(element.hDll);
        #elif defined(__linux__)
            //dlclose(dlibs.at(i).handle);
            //if (dlclose(element.handle) != 0) {
            //    std::cerr << "Failed to close library: " << dlerror() << std::endl;
            //}
            dlclose(element.handle);
            //std::cout << element.handle << "\n";
        #endif
    }
}

Token eval(std::vector<Token> code, Venv* venv, std::string path){
    int i = 0;
    int x = 0;
    //std::cout << "printing data\n";
    //for (Token element : code) {
    //    std::cout << "data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << ", index: " << i << "\n";
    //    i++;
    //}
    //i = 0;

    std::vector<Var*> venvvars = getvars(venv);
    while (i<code.size()){
        if (std::any_cast<std::string>(code[i].data[0]) == "word"){
            if (varinlist(venvvars, std::any_cast<std::string>(code[i].data[1]))){
                bool nonobjectvar = true;
                if (isIndexInBounds(code, i-1) && std::any_cast<std::string>(code[i-1].data[0]) == "keyword" && std::any_cast<std::string>(code[i-1].data[1]) == "."){
                    nonobjectvar = false;
                }

                if (nonobjectvar){
                    code[i] = venvvars[varinlistindex(venvvars, std::any_cast<std::string>(code[i].data[1]))]->data;//venv->vars[venv->varindex(std::any_cast<std::string>(code[i].data[1]))].data;
                }
            }
            //if (venv->varin(std::any_cast<std::string>(code[i].data[1]))){
            //    bool nonobjectvar = true;
            //    if (isIndexInBounds(code, i-1) && std::any_cast<std::string>(code[i-1].data[0]) == "keyword" && std::any_cast<std::string>(code[i-1].data[1]) == "."){
            //        nonobjectvar = false;
            //    }
//
            //    if (nonobjectvar){
            //        code[i] = venv->vars[venv->varindex(std::any_cast<std::string>(code[i].data[1]))].data;
            //    }
            //}
        }
        i++;
    }

    i = 0;
    while (i<code.size()){
        if (std::any_cast<std::string>(code[i].data[0]) == "keyword" && std::any_cast<std::string>(code[i].data[1]) == "."){
            if (!isIndexInBounds(code, i-1) && !isIndexInBounds(code, i+1)){
                print("ERROR at " + std::any_cast<std::string>(code[i].data[2]) + ": Expected venv and word.", PRINT_WHITE, PRINT_ERROR);
                freedlibs();
                exit(-1);
            }

            x = i-1;
            bool end = false;
            int start;
            int end_;

            while (!end){
                if (x==-1){
                    end = true;
                    x = 0;
                    break;
                }

                if (std::any_cast<std::string>(code[x].data[0]) == "keyword"){
                    if (std::any_cast<std::string>(code[x].data[1]) == "+" || std::any_cast<std::string>(code[x].data[1]) == "-" || std::any_cast<std::string>(code[x].data[1]) == "*" || std::any_cast<std::string>(code[x].data[1]) == "/" || std::any_cast<std::string>(code[x].data[1]) == "/" || std::any_cast<std::string>(code[x].data[1]) == "=" || std::any_cast<std::string>(code[x].data[1]) == "!"){
                        end = true;
                        x++;
                        break;
                    }
                }

                if (std::any_cast<std::string>(code[x].data[0]) == "word"){
                    if (std::any_cast<std::string>(code[x].data[1]) == "or" || std::any_cast<std::string>(code[x].data[1]) == "and" || std::any_cast<std::string>(code[x].data[1]) == "not"){
                        end = true;
                        x++;
                        break;
                    }
                }

                x--;
            }
            Token venvtoken = eval(getSubvector(code, x, i-1), venv, path);
            start = x;
            if (std::any_cast<std::string>(venvtoken.data[0]) != "object"){
                print("ERROR at " + std::any_cast<std::string>(venvtoken.data[2]) + ": Expected object.", PRINT_WHITE, PRINT_ERROR);
                freedlibs();
                exit(-1);
            }
            Venv* objectvenv = &venv->objects[std::any_cast<int>(venvtoken.data[1])];

            x = i+1;
            end = false;

            while (!end){
                if (x==code.size()){
                    end = true;
                    x--;
                    break;
                }

                if (std::any_cast<std::string>(code[x].data[0]) == "keyword"){
                    if (std::any_cast<std::string>(code[x].data[1]) == "+" || std::any_cast<std::string>(code[x].data[1]) == "-" || std::any_cast<std::string>(code[x].data[1]) == "*" || std::any_cast<std::string>(code[x].data[1]) == "/" || std::any_cast<std::string>(code[x].data[1]) == "/" || std::any_cast<std::string>(code[x].data[1]) == "=" || std::any_cast<std::string>(code[x].data[1]) == "!"){
                        end = true;
                        x--;
                        break;
                    }
                }

                if (std::any_cast<std::string>(code[x].data[0]) == "word"){
                    if (std::any_cast<std::string>(code[x].data[1]) == "or" || std::any_cast<std::string>(code[x].data[1]) == "and" || std::any_cast<std::string>(code[x].data[1]) == "not"){
                        end = true;
                        x--;
                        break;
                    }
                }

                x++;
            }

            std::vector<Token> argumentvector = getSubvector(code, i+1, x);
            //std::cout << objectvenv << "\n";
            //for(Token t: argumentvector){
            //    std::cout << std::any_cast<std::string>(t.data[1]) << "\n";
            //}

            end_ = x;

            Token result = eval(argumentvector, objectvenv, path);

            int del = end_-start;
            while (del != 0){
                removeAtIndex(code, start+1);
                del--;
            }

            code[start] = result;

            //for(int ik=0;ik!=code.size();ik++){
            //    std::cout << std::any_cast<std::string>(code[ik].data[1]) << "\n";
            //}

            i = x;
        }
        i++;
    }

    //i = 0;
    //while (i<code.size()){
    //    if (std::any_cast<std::string>(code[i].data[0]) == "object"){
    //        if (isIndexInBounds(code, i+1) && std::any_cast<std::string>(code[i+1].data[0]) == "keyword" && std::any_cast<std::string>(code[i+1].data[1]) == "."){
    //            int venvindex = std::any_cast<int>(code[i].data[1]);
    //            int x = i+2;
    //            bool finished = false;
    //            while(!finished){
    //                if(x==code.size()){
    //                    finished = true;
    //                    x--;
    //                    break;
    //                }
    //                if (std::any_cast<std::string>(code[x].data[0]) == "keyword" && std::any_cast<std::string>(code[x].data[1]) == ";"){
    //                    finished = true;
    //                    break;
    //                }
//
    //                x++;
    //            }
//
    //            if(x-i+2>0){
    //                std::vector<Token> objectcodevector = getSubvector(code, i+2, x);
    //                Token extraToken;
    //                extraToken.data.push_back((std::string)"keyword");
    //                extraToken.data.push_back((std::string)";");
    //                objectcodevector.push_back(extraToken);
    //                std::cout << "pointer: " <<  &venv->objects[venvindex] << "\n";
    //                interpret(objectcodevector, &venv->objects[venvindex], path);
    //                
    //            }else{
    //                print("ERROR: Expected word.", PRINT_WHITE, PRINT_ERROR);
    //                exit(-1);
    //            }
    //            
    //        }
    //    }
    //    i++;
    //}



    i = 0;
    while (i<code.size()){
        if (std::any_cast<std::string>(code[i].data[0]) == "word"){
            if (std::any_cast<std::string>(code[i].data[1]) == "LOADDLIB"){
                if (i+1!=code.size()){
                    std::vector<Token> libnamevector = getSubvector(code, i+1, code.size()-1);
                    //for (Token element: libnamevector) {
                    //    std::cout << "--data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                    //}
                    Token libname = eval(libnamevector, venv, path);
                    
                    int libid = 0;
                    if(std::any_cast<std::string>(libname.data[0]) == "str"){
                        int x = 0;
                        bool newlib = false;
                        while(x!=dlibs.size()){
                            if(dlibs[x].name==std::any_cast<std::string>(libname.data[1])){
                                libid = dlibs[x].id;
                                newlib = true;
                            }
                            x++;
                        }
                        if(!newlib){
                            std::cout << "loading lib: '" << std::any_cast<std::string>(libname.data[1]) << "'\n";
                            dlib DLIB;
                            DLIB.name = std::any_cast<std::string>(libname.data[1]);
                            libid = getnewid();
                            //std::cout << "DLIB ID: " << libid << "\n";
                            DLIB.id = libid;
                            #if defined(_WIN32)
                                HINSTANCE hDll = LoadLibraryA(std::any_cast<std::string>(libname.data[1]).c_str());
                               if (!hDll) {
                                    std::cerr << "Failed to load DLL at " + std::any_cast<std::string>(code[i].data[2]) + ": " << std::any_cast<std::string>(libname.data[1]) << std::endl;
                                    freedlibs();
                                    exit(-1);
                                }
                            
                                DLIB.hDll = hDll;
                            #elif defined(__linux__)
                                void* handle = dlopen(std::any_cast<std::string>(libname.data[1]).c_str(), RTLD_LAZY);
                                if (!handle) {
                                    std::cerr << "Failed to load shared library at " + std::any_cast<std::string>(code[i].data[2]) + ": " << dlerror() << std::endl;
                                    freedlibs();
                                    exit(-1);
                                }
                                dlerror();
                                DLIB.handle = handle;
                            #endif
                            dlibs.push_back(DLIB);
                        }

                    code[i].data[0] = (std::string)"DYNAMICLIB";
                    code[i].data[1] = std::to_string(libid);
                    }
                }
            }
        }
        i++;
    }




    i = 0;
    while (i<code.size()){
        if (std::any_cast<std::string>(code.at(i).data.at(0)) == "keyword" && std::any_cast<std::string>(code.at(i).data.at(1)) == "("){
            if(isIndexInBounds(code, i-1)){
                if (std::any_cast<std::string>(code.at(i-1).data.at(0)) == "func" || std::any_cast<std::string>(code.at(i-1).data.at(0)) == "class" || std::any_cast<std::string>(code.at(i-1).data.at(0)) == "word"){
                    i++;
                    continue;
                }
                //if(isIndexInBounds(code, i-2)){
                //    if (std::any_cast<std::string>(code.at(i-2).data.at(0)) == "keyword" || std::any_cast<std::string>(code.at(i-2).data.at(1)) == "."){
                //        i++;
                //        continue;
                //    }
                //}
            }
            index indexdata = cutCH('(', ')', i, code);
            Token result;
            if (indexdata.index1-1!=indexdata.index2){

                std::vector<Token> resultsubvector = getSubvector(code, indexdata.index1, indexdata.index2);
                result = eval(resultsubvector, venv, path);
            }else{
                result.data.push_back((std::string)"str");
                result.data.push_back((std::string)"None");
            }
            x = 0;
            //std::cout << "start: " << indexdata.index1 << " end: " << indexdata.index2 << "\n";
            while (x!=code.size()){
                if (x >= indexdata.index1 && x <= indexdata.index2+1){
                    //std::cout << "removing: " << std::any_cast<std::string>(code[x].data[1]) << " x: " << x << "\n";
                    removeAtIndex(code, x);
                    indexdata.index2--;
                }else{
                    x++;
                }
            }
            //std::cout << "removing finished!\n";
            code[indexdata.index1-1] = result;
        }
        i++;
    }
    //std::cout << "printing list\n";
    //i = 0;
    //for (Token element : code) {
    //    if (std::any_cast<std::string>(element.data.at(0)) == "class"){
    //        std::cout << "class\n";
    //    }else if (std::any_cast<std::string>(element.data.at(0)) == "object"){
    //        std::cout << "object\n";
    //    }else{
    //        std::cout << "---type: " << std::any_cast<std::string>(element.data.at(0)) << ", data: " << std::any_cast<std::string>(element.data.at(1)) << ", index: " << i << "\n";
    //    }
    //    i++;
    //}

    i = 0;
    while(code.size()>i){//functions
        if (std::any_cast<std::string>(code.at(i).data.at(0)) == "keyword" && std::any_cast<std::string>(code.at(i).data.at(1)) == "("){
            if(isIndexInBounds(code, i-1)){
                if (std::any_cast<std::string>(code.at(i-1).data.at(0)) == "func"){
                    std::string FunctionName = std::any_cast<std::string>(code.at(i-1).data.at(1));
                    int FunctionIndex = venv->varindex(FunctionName); 
                    Venv FunctionVenv;
                    //Var FunctionResultVar;
                    //FunctionResultVar.name = "Function_result_return_value";
                    //FunctionResultVar.data.data.push_back((std::string)"str");
                    //FunctionResultVar.data.data.push_back((std::string)"None");
                    //FunctionVenv.vars.push_back(FunctionResultVar);
//
                    //Var MVenvOSVar;
                    //MVenvOSVar.name = "CURRENT_RUNNING_OS";
                    //MVenvOSVar.data.data.push_back((std::string)"str");
//
                    //#if defined(_WIN32)
                    ////    #include <windows.h>
                    //    MVenvOSVar.data.data.push_back((std::string)"WINDOWS");
                    //#elif defined(__linux__)
                    ////    #include <dlfcn.h>
                    //    MVenvOSVar.data.data.push_back((std::string)"LINUX");
                    //#else
                    //    print("WARNING: UNKNOWN OS, NO DLIB SUPPORT");
                    //    MVenvOSVar.data.data.push_back((std::string)"UNKNOWN");
                    //#endif
//
                    //FunctionVenv.vars.push_back(MVenvOSVar);
//
                    //Var MVenvExecFunctionReturnVar;
                    //MVenvExecFunctionReturnVar.name = "ExecFunctionReturnVar";
                    //MVenvExecFunctionReturnVar.data.data.push_back((std::string)"str");
                    //MVenvExecFunctionReturnVar.data.data.push_back((std::string)"None");
//
                    //FunctionVenv.vars.push_back(MVenvExecFunctionReturnVar);
                    //
                    //Var MVenvSPNVersion;
                    //MVenvSPNVersion.name = "SPNVERSION";
                    //MVenvSPNVersion.data.data.push_back((std::string)"int");
                    //MVenvSPNVersion.data.data.push_back((std::string)"2");

                    //FunctionVenv.vars.push_back(MVenvSPNVersion);
                    FunctionVenv.parent = venv;



                    index indexdata = cutCH('(', ')', i, code);
                    //std::vector<Token> resultsubvector = getSubvector(code, indexdata.index1, indexdata.index2);
                    Token result;
                    std::vector<std::vector<Token>> argvector;
                    if (indexdata.index1-1!=indexdata.index2){
                        
                        std::vector<Token> resultsubvector = getSubvector(code, indexdata.index1, indexdata.index2);
                        int x = 0;
                        int last = 0;
                        while(x!=resultsubvector.size()){
                            if (x+1==resultsubvector.size()){
                                argvector.push_back(getSubvector(resultsubvector, last, x));
                            }
                            if (std::any_cast<std::string>(resultsubvector[x].data[0]) == "keyword" && std::any_cast<std::string>(resultsubvector[x].data[1]) == ","){
                                argvector.push_back(getSubvector(resultsubvector, last, x-1));
                                last = x+1;
                            }
                            x++;
                        }
                    }

                    
                    Token extraToken;
                    extraToken.data.push_back((std::string)"keyword");
                    extraToken.data.push_back((std::string)";");
                    std::vector<std::vector<Token>> FunctionInitArgVector = std::any_cast<std::vector<std::vector<Token>>>(venv->vars[FunctionIndex].data.data[3]);
                    for (std::vector<Token> elements: FunctionInitArgVector){
                        //std::cout << "start\n";
                        elements.push_back(extraToken);
                        //for (Token element: elements) {
                        //    std::cout << "--data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                        //}
                        interpret(elements, &FunctionVenv, path);
                        //std::cout << "end\n";
                    }


                    bool WordsActive = false;
                    int argindex = 0;//why not 0? because 0 is the return standard function var, 1 is the os name, 2 is execute fucnction return and 3 is sepynode version// new change 0 is the new ...
                    for (std::vector<Token> elements: argvector){
                        //std::cout << "start\n";
                        //elements.push_back(extraToken);
                        ////for (Token element: elements) {
                        ////    std::cout << "--data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                        ////}
                        int split = 0;
                        while (split < elements.size() && std::any_cast<std::string>(elements[split].data[0]) != "keyword" && std::any_cast<std::string>(elements[split].data[1]) != "="){
                            split++;
                        }

                        if (split == elements.size()){
                            if (!WordsActive){
                                if (argindex >= FunctionVenv.vars.size()){
                                    std::cout << "size: " << FunctionVenv.vars.size() << " index: " << argindex << "\n";
                                    
                                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": got too many arguments", PRINT_WHITE, PRINT_ERROR);
                                    freedlibs();
                                    exit(-1);
                                }

                                std::vector<Token> ArgCodeVector;
                                ArgCodeVector.reserve(4);
                                std::string ArgVarName = FunctionVenv.vars[argindex].name;
                                //std::cout << ArgVarName << "\n";
                                Token ArgVarToken;
                                ArgVarToken.data.push_back((std::string)"word");
                                ArgVarToken.data.push_back(ArgVarName);
                                Token EqualToken;
                                EqualToken.data.push_back((std::string)"keyword");
                                EqualToken.data.push_back((std::string)"=");
                                //elements.insert(elements.begin(), {ArgVarToken, EqualToken});
                                Token ResultToken = eval(elements, venv, path);
                                ArgCodeVector.push_back(ArgVarToken);
                                ArgCodeVector.push_back(EqualToken);
                                ArgCodeVector.push_back(ResultToken);
                                ArgCodeVector.push_back(extraToken);
                                //std::cout << "start\n";
                                //for (Token element: ArgCodeVector) {
                                //    std::cout << "--data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                                //}
                                
                                interpret(ArgCodeVector, &FunctionVenv, path);
                                //std::cout << std::any_cast<std::string>(FunctionVenv.vars[FunctionVenv.varindex("dlib")].data.data[1]) << "\n";
                                argindex++;
                            }else{
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected var name", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }
                        }else{
                            WordsActive = true;
                            std::vector<Token> ArgCodeVector;
                            ArgCodeVector.reserve(4);
                            Token ArgVarToken = elements[0];
                            Token EqualToken;
                            EqualToken.data.push_back((std::string)"keyword");
                            EqualToken.data.push_back((std::string)"=");
                            //elements.insert(elements.begin(), {ArgVarToken, EqualToken});
                            std::vector<Token> ResultVector = getSubvector(elements, 2, elements.size()-1);
                            Token ResultToken = eval(ResultVector, venv, path);
                            ArgCodeVector.push_back(ArgVarToken);
                            ArgCodeVector.push_back(EqualToken);
                            ArgCodeVector.push_back(ResultToken);
                            ArgCodeVector.push_back(extraToken);

                            interpret(ArgCodeVector, &FunctionVenv, path);
                        }




                        //interpret(elements, FunctionVenv);
                        //for (Token element: elements) {
                            
                            //std::cout << "--data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                        //}
                        //std::cout << "end\n";
                    }

                    std::vector<Token> FunctionCode = std::any_cast<std::vector<Token>>(venv->vars[FunctionIndex].data.data[2]);
                    interpret(FunctionCode, &FunctionVenv, path);

                    int x = 0;
                    while (code.size() != x){
                        if (x >= indexdata.index1-1 && x <= indexdata.index2+1){
                            removeAtIndex(code, x);
                            indexdata.index2--;
                        }else{
                            x++;
                        }

                    }

                    std::vector<Var*> venvvars = getvars(venv);

                    code[i-1] = venvvars[varinlistindex(venvvars, "Function_result_return_value")]->data;
                    //FunctionVenv.vars[0].data;

                    
                }else if (std::any_cast<std::string>(code.at(i-1).data.at(0)) == "class"){
                    //std::cout << i << " " << code.size() << "\n";
                    venv->objects.push_back(Venv());
                    int venvindex = venv->objects.size()-1;
                    Venv* objectvenv = &venv->objects[venvindex];
                    objectvenv->parent = venv;

                    interpret(std::any_cast<std::vector<Token>>(code.at(i-1).data[1]), objectvenv, path); //&venv->objects[venvindex]

 

                    Token extraToken;
                    extraToken.data.push_back((std::string)"keyword");
                    extraToken.data.push_back((std::string)";");

                    int before_init_arg_index = objectvenv->vars.size()-1;

                    //std::cout << objectvenv << "\n";
                    //std::cout << objectvenv.operatorINIT.data.size() << "\n";
                    //if (objectvenv->operatorINIT.data.size() != 0){
                    if (objectvenv->operatorINIT.init){
                        Token classinittoken = objectvenv->operatorINIT;
                        std::vector<std::vector<Token>> classInitArgVector = std::any_cast<std::vector<std::vector<Token>>>(classinittoken.data[1]);
                        std::vector<Token> classInitCode = std::any_cast<std::vector<Token>>(classinittoken.data[0]);

                        for (std::vector<Token> elements: classInitArgVector){
                            //std::cout << "start\n";
                            elements.push_back(extraToken);
                            //for (Token element: elements) {
                            //    std::cout << "--data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                            //}
                            interpret(elements, objectvenv, path);

                            //std::cout << "end\n";
                        }





                        index indexdata = cutCH('(', ')', i, code);
                        Token result;
                        std::vector<std::vector<Token>> argvector;
                        if (indexdata.index1-1!=indexdata.index2){

                            std::vector<Token> resultsubvector = getSubvector(code, indexdata.index1, indexdata.index2);
                            int x = 0;
                            int last = 0;
                            while(x!=resultsubvector.size()){
                                if (x+1==resultsubvector.size()){
                                    argvector.push_back(getSubvector(resultsubvector, last, x));
                                }
                                if (std::any_cast<std::string>(resultsubvector[x].data[0]) == "keyword" && std::any_cast<std::string>(resultsubvector[x].data[1]) == ","){
                                    argvector.push_back(getSubvector(resultsubvector, last, x-1));
                                    last = x+1;
                                }
                                x++;
                            }
                        }



                        //index indexdata = cutCH('(', ')', i, code);
                        //std::vector<std::vector<Token>> argvector;
                        //if(indexdata.index2+1!=indexdata.index1){
                        //    std::vector<Token> resultsubvector = getSubvector(code, indexdata.index1, indexdata.index2);
                        //    int x = 0;
                        //    int last = 0;
                        //    while(x!=resultsubvector.size()){
                        //        if (x+1==resultsubvector.size()){
                        //            argvector.push_back(getSubvector(resultsubvector, last, x));
                        //        }
                        //        if (std::any_cast<std::string>(resultsubvector[x].data[0]) == "keyword" && std::any_cast<std::string>(resultsubvector[x].data[1]) == ","){
                        //            argvector.push_back(getSubvector(resultsubvector, last, x-1));
                        //            last = x+1;
                        //        }
                        //        x++;
                        //    }
                        //    //print("ERROR: Init arguments are not yet supported.", PRINT_WHITE, PRINT_ERROR);
                        //    //exit(-1);
                        //}


                        //for(std::vector<Token> args: argvector){
                        //    std::cout << "start:\n";
                        //    for (Token arg: args){
                        //        std::cout << std::any_cast<std::string>(arg.data[1]) << "\n";
                        //    }
                        //    std::cout << "end\n";
                        //}

                        bool WordsActive = false;
                        int argindex = before_init_arg_index+1;
                        for (std::vector<Token> elements: argvector){
                            //std::cout << "start\n";
                            //elements.push_back(extraToken);
                            ////for (Token element: elements) {
                            ////    std::cout << "--data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                            ////}
                            int split = 0;
                            while (split < elements.size() && std::any_cast<std::string>(elements[split].data[0]) != "keyword" && std::any_cast<std::string>(elements[split].data[1]) != "="){
                                split++;
                            }

                            if (split == elements.size()){
                                if (!WordsActive){
                                    if (argindex >= objectvenv->vars.size()){
                                        std::cout << "size: " << objectvenv->vars.size() << " index: " << argindex << "\n";

                                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(1)) + ": got too many arguments", PRINT_WHITE, PRINT_ERROR);
                                        freedlibs();
                                        exit(-1);
                                    }

                                    std::vector<Token> ArgCodeVector;
                                    ArgCodeVector.reserve(4);
                                    std::string ArgVarName = objectvenv->vars[argindex].name;
                                    Token ArgVarToken;
                                    ArgVarToken.data.push_back((std::string)"word");
                                    ArgVarToken.data.push_back(ArgVarName);
                                    Token EqualToken;
                                    EqualToken.data.push_back((std::string)"keyword");
                                    EqualToken.data.push_back((std::string)"=");
                                    //elements.insert(elements.begin(), {ArgVarToken, EqualToken});
                                    Token ResultToken = eval(elements, venv, path);
                                    ArgCodeVector.push_back(ArgVarToken);
                                    ArgCodeVector.push_back(EqualToken);
                                    ArgCodeVector.push_back(ResultToken);
                                    ArgCodeVector.push_back(extraToken);
                                    //std::cout << "start\n";
                                    //for (Token element: ArgCodeVector) {
                                    //    std::cout << "--data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                                    //}

                                    interpret(ArgCodeVector, objectvenv, path);

                                    argindex++;
                                }else{
                                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected var name", PRINT_WHITE, PRINT_ERROR);
                                    freedlibs();
                                    exit(-1);
                                }
                            }else{
                                WordsActive = true;
                                std::vector<Token> ArgCodeVector;
                                ArgCodeVector.reserve(4);
                                Token ArgVarToken = elements[0];
                                Token EqualToken;
                                EqualToken.data.push_back((std::string)"keyword");
                                EqualToken.data.push_back((std::string)"=");
                                //elements.insert(elements.begin(), {ArgVarToken, EqualToken});
                                std::vector<Token> ResultVector = getSubvector(elements, 2, elements.size()-1);
                                Token ResultToken = eval(ResultVector, venv, path);
                                ArgCodeVector.push_back(ArgVarToken);
                                ArgCodeVector.push_back(EqualToken);
                                ArgCodeVector.push_back(ResultToken);
                                ArgCodeVector.push_back(extraToken);

                                interpret(ArgCodeVector, objectvenv, path);
                            }




                            //interpret(elements, FunctionVenv);
                            //for (Token element: elements) {

                                //std::cout << "--data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                            //}
                            //std::cout << "end\n";
                        }




                        interpret(classInitCode, objectvenv, path);
                    }

                    //Var FunctionResultVar;
                    //FunctionResultVar.name = "Function_result_return_value";
                    //FunctionResultVar.data.data.push_back((std::string)"str");
                    //FunctionResultVar.data.data.push_back((std::string)"None");
                    //objectvenv.vars.push_back(FunctionResultVar);
//
                    //Var MVenvOSVar;
                    //MVenvOSVar.name = "CURRENT_RUNNING_OS";
                    //MVenvOSVar.data.data.push_back((std::string)"str");
//
                    //#if defined(_WIN32)
                    ////    #include <windows.h>
                    //    MVenvOSVar.data.data.push_back((std::string)"WINDOWS");
                    //#elif defined(__linux__)
                    ////    #include <dlfcn.h>
                    //    MVenvOSVar.data.data.push_back((std::string)"LINUX");
                    //#else
                    //    print("WARNING: UNKNOWN OS, NO DLIBS WILL WORK");
                    //    MVenvOSVar.data.data.push_back((std::string)"UNKNOWN");
                    //#endif
//
                    //objectvenv.vars.push_back(MVenvOSVar);
//
                    //Var MVenvExecFunctionReturnVar;
                    //MVenvExecFunctionReturnVar.name = "ExecFunctionReturnVar";
                    //MVenvExecFunctionReturnVar.data.data.push_back((std::string)"str");
                    //MVenvExecFunctionReturnVar.data.data.push_back((std::string)"None");
//
                    //objectvenv.vars.push_back(MVenvExecFunctionReturnVar);
                    //
                    //Var MVenvSPNVersion;
                    //MVenvSPNVersion.name = "SPNVERSION";
                    //MVenvSPNVersion.data.data.push_back((std::string)"int");
                    //MVenvSPNVersion.data.data.push_back((std::string)"2");
//
                    //objectvenv.vars.push_back(MVenvSPNVersion);
                    //std::cout << "pointer: " << &venv->objects[venvindex] << "\n";

                    

                    //std::cout << "class venv vars:\n";
                    //for (Var t: objectvenv.vars){
                    //    if (t.data.data.size() == 2 && std::any_cast<std::string>(t.data.data.at(0)) != "class"){
                    //        std::cout << "name: \"" << t.name << "\" data: " << std::any_cast<std::string>(t.data.data.at(1)) << " type: " << std::any_cast<std::string>(t.data.data.at(0)) << "\n"; 
                    //    }else if(t.data.data.size() == 3 || std::any_cast<std::string>(t.data.data.at(0)) == "class"){
                    //        std::cout << "name: \"" << t.name << "\" type: " << std::any_cast<std::string>(t.data.data.at(0)) << "\n"; 
                    //    }
                    //}
                    //std::cout << "\n";


                    Token objectToken;
                    objectToken.data.push_back((std::string)"object");
                    objectToken.data.push_back(venvindex);
                    //std::cout << code.at(i-1).data.at(2).type().name() << "\n";
                    objectToken.data.push_back(std::any_cast<std::string>(code.at(i-1).data.at(2)));
                    //objectToken.data.push_back(&objectvenv);
                    //std::cout << "pointer: " << &objectvenvpointer << "\n";
                    code[i-1] = objectToken;
                    removeAtIndex(code, i);
                    removeAtIndex(code, i);


                }//else{
                //    print("ERROR: Expected function", PRINT_WHITE, PRINT_ERROR);
                //    exit(-1);
                //}
            }
        }
        i++;
    }

    //i = 0;
    //while (code.size()>i){
    //    if (std::any_cast<std::string>(code[i].data[0]) == "keyword" && std::any_cast<std::string>(code[i].data[1]) == "."){
    //        if (isIndexInBounds(code, i-1) && isIndexInBounds(code, i+1)){
    //            Token venvtoken = code[i-1];
    //            Token keywordtoken = code[i+1];
    //            if (std::any_cast<std::string>(venvtoken.data[0]) == "object" && std::any_cast<std::string>(keywordtoken.data[0]) == "word"){
    //                
    //            }
    //        }
    //    }
//
    //    i++;
    //}
    
    //std::cout << "printing list\n";
    //i = 0;
    //for (Token element : code) {
    //    std::cout << "---data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << ", index: " << i << "\n";
    //    i++;
    //}


    i = 0;
    while (code.size() > i){
        if (std::any_cast<std::string>(code.at(i).data.at(0)) == "keyword"){
            if (std::any_cast<std::string>(code.at(i).data.at(1)) == "*"){
                if (isIndexInBounds(code, i-1) && isIndexInBounds(code, i+1)){
                    if (std::any_cast<std::string>(code.at(i-1).data.at(0)) == "int"){
                        if (std::any_cast<std::string>(code.at(i+1).data.at(0)) == "int"){
                            code.at(i-1).data.at(1) = std::to_string(std::stof(std::any_cast<std::string>(code.at(i-1).data.at(1)))*std::stof(std::any_cast<std::string>(code.at(i+1).data.at(1))));
                            removeAtIndex(code, i+1);
                            removeAtIndex(code, i);
                            i -= 2;
                        }else if(std::any_cast<std::string>(code.at(i+1).data.at(0)) == "str"){
                            std::string strout = "";
                            for (int i = 0;i != std::stoi(std::any_cast<std::string>(code.at(i-1).data.at(1)));i++){
                                strout += std::any_cast<std::string>(code.at(i+1).data.at(1));
                            }
                            code.at(i-1).data.at(1) = strout;
                            code.at(i-1).data.at(0) = (std::string)"str";
                            removeAtIndex(code, i+1);
                            removeAtIndex(code, i);
                            i -= 2;
                        }else if(std::any_cast<std::string>(code.at(i+1).data.at(0)) == "object"){
                            int objectvenvindex = std::any_cast<int>(code.at(i+1).data.at(1));
                            Venv* objectvenv = &venv->objects[objectvenvindex];
                            if (!objectvenv->operatorMULT.init){
                                print("ERROR at " + std::any_cast<std::string>(code.at(i+2).data.at(2)) + ": class type " + std::any_cast<std::string>(code.at(i+1).data.at(2)) + " does not support multiplicatiopn!", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }
                            std::vector<Token> code = std::any_cast<std::vector<Token>>(objectvenv->operatorMULT.data[0]);
                            std::vector<std::vector<Token>> argvector = std::any_cast<std::vector<std::vector<Token>>>(objectvenv->operatorMULT.data[1]);

                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Not implomented yet!", PRINT_WHITE, PRINT_ERROR);
                            //std::cerr << "Not implemented yet!";
                            freedlibs();
                            exit(-1);
                        }
                    }else if(std::any_cast<std::string>(code.at(i-1).data.at(0)) == "str"){
                        if (std::any_cast<std::string>(code.at(i+1).data.at(0)) == "int"){
                            std::string strout = "";
                            for (int i = 0;i != std::stoi(std::any_cast<std::string>(code.at(i+1).data.at(1)));i++){
                                strout += std::any_cast<std::string>(code.at(i-1).data.at(1));
                            }
                            code.at(i-1).data.at(1) = strout;
                            removeAtIndex(code, i+1);
                            removeAtIndex(code, i);
                            i -= 2;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Not implomented yet!", PRINT_WHITE, PRINT_ERROR);
                            //std::cerr << "Not implemented yet!";
                            freedlibs();
                            exit(-1);
                        }
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Not implomented yet!", PRINT_WHITE, PRINT_ERROR);
                        //std::cerr << "Not implemented yet!";
                        freedlibs();
                        exit(-1);
                    }
                }
            }else if (std::any_cast<std::string>(code.at(i).data.at(1)) == "/"){
                if (isIndexInBounds(code, i-1) && isIndexInBounds(code, i+1)){
                    if (std::any_cast<std::string>(code.at(i-1).data.at(0)) == "int"){
                        if (std::any_cast<std::string>(code.at(i+1).data.at(0)) == "int"){
                            code.at(i-1).data.at(1) = std::to_string(std::stof(std::any_cast<std::string>(code.at(i-1).data.at(1)))/std::stof(std::any_cast<std::string>(code.at(i+1).data.at(1))));
                            removeAtIndex(code, i+1);
                            removeAtIndex(code, i);
                            i -= 2;
                        }else if(std::any_cast<std::string>(code.at(i+1).data.at(0)) == "str"){
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Strings don't support division!", PRINT_WHITE, PRINT_ERROR);
                            //std::cerr << "str don't support division";
                            freedlibs();
                            exit(-1);
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Not implomented yet!", PRINT_WHITE, PRINT_ERROR);
                            //std::cerr << "Not implemented yet!";
                            freedlibs();
                            exit(-1);
                        }
                    }else if(std::any_cast<std::string>(code.at(i-1).data.at(0)) == "str"){
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Strings don't support division!", PRINT_WHITE, PRINT_ERROR);
                            //std::cerr << "str don't support division";
                            freedlibs();
                            exit(-1);
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Not implomented yet!", PRINT_WHITE, PRINT_ERROR);
                        //std::cerr << "Not implemented yet!";
                        freedlibs();
                        exit(-1);
                    }
                }
            }
            else if (std::any_cast<std::string>(code.at(i).data.at(1)) == "%")
            {
                if (isIndexInBounds(code, i - 1) && isIndexInBounds(code, i + 1))
                {
                    if (std::any_cast<std::string>(code.at(i - 1).data.at(0)) == "int")
                    {
                        if (std::any_cast<std::string>(code.at(i + 1).data.at(0)) == "int")
                        {
                            int left = std::stoi(std::any_cast<std::string>(code.at(i - 1).data.at(1)));
                            int right = std::stoi(std::any_cast<std::string>(code.at(i + 1).data.at(1)));

                            code.at(i - 1).data.at(1) = std::to_string(left % right);

                            removeAtIndex(code, i + 1);
                            removeAtIndex(code, i);
                            i -= 2;
                        }
                        else if (std::any_cast<std::string>(code.at(i + 1).data.at(0)) == "str")
                        {
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Strings don't support modulo!", PRINT_WHITE, PRINT_ERROR);
                            // std::cerr << "str don't support division";
                            freedlibs();
                            exit(-1);
                        }
                        else
                        {
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Not implemented yet!", PRINT_WHITE, PRINT_ERROR);
                            // std::cerr << "Not implemented yet!";
                            freedlibs();
                            exit(-1);
                        }
                    }
                    else if (std::any_cast<std::string>(code.at(i - 1).data.at(0)) == "str")
                    {
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Strings don't support modulo!", PRINT_WHITE, PRINT_ERROR);
                        // std::cerr << "str don't support modulo";
                        freedlibs();
                        exit(-1);
                    }
                    else
                    {
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Not implemented yet!", PRINT_WHITE, PRINT_ERROR);
                        // std::cerr << "Not implemented yet!";
                        freedlibs();
                        exit(-1);
                    }
                }
            }
        }
        i++;
    }

    i = 0;
    while (code.size() > i){
        if (std::any_cast<std::string>(code.at(i).data.at(0)) == "keyword"){
            if(std::any_cast<std::string>(code.at(i).data.at(1)) == "+"){
                if (isIndexInBounds(code, i-1) && isIndexInBounds(code, i+1)){
                    if (std::any_cast<std::string>(code.at(i-1).data.at(0)) == "int"){
                        if (std::any_cast<std::string>(code.at(i+1).data.at(0)) == "int"){
                            code.at(i-1).data.at(1) = std::to_string(std::stof(std::any_cast<std::string>(code.at(i-1).data.at(1)))+std::stof(std::any_cast<std::string>(code.at(i+1).data.at(1))));
                            removeAtIndex(code, i+1);
                            removeAtIndex(code, i);
                            i -= 2;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Not implomented yet!", PRINT_WHITE, PRINT_ERROR);
                            //std::cerr << "Not implemented yet!";
                            freedlibs();
                            exit(-1);
                        }
                    }else if(std::any_cast<std::string>(code.at(i-1).data.at(0)) == "str"){
                        if (std::any_cast<std::string>(code.at(i+1).data.at(0)) == "str"){
                            code.at(i-1).data.at(1) = std::any_cast<std::string>(code.at(i-1).data.at(1))+std::any_cast<std::string>(code.at(i+1).data.at(1));
                            removeAtIndex(code, i+1);
                            removeAtIndex(code, i);
                            i -= 2;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Not implomented yet!", PRINT_WHITE, PRINT_ERROR);
                            //std::cerr << "Not implemented yet!";
                            freedlibs();
                            exit(-1);
                        }
                    }
                }
            }else if(std::any_cast<std::string>(code.at(i).data.at(1)) == "-"){
                if (isIndexInBounds(code, i-1) && isIndexInBounds(code, i+1)){
                    if (std::any_cast<std::string>(code.at(i-1).data.at(0)) == "int"){
                        if (std::any_cast<std::string>(code.at(i+1).data.at(0)) == "int"){
                            code.at(i-1).data.at(1) = std::to_string(std::stof(std::any_cast<std::string>(code.at(i-1).data.at(1)))-std::stof(std::any_cast<std::string>(code.at(i+1).data.at(1))));
                            removeAtIndex(code, i+1);
                            removeAtIndex(code, i);
                            i -= 2;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Not implomented yet!", PRINT_WHITE, PRINT_ERROR);
                            //std::cerr << "Not implemented yet!";
                            freedlibs();
                            exit(-1);
                        }
                    }
                }
            }
        }

        i++;
    }
    

    i = 0;
    while(i<code.size()){
        if (std::any_cast<std::string>(code[i].data[0]) == "keyword" && std::any_cast<std::string>(code[i].data[1]) == "=" && isIndexInBounds(code, i+1)){
            if (std::any_cast<std::string>(code[i+1].data[0]) == "keyword" && std::any_cast<std::string>(code[i+1].data[1]) == "="){
                if(isIndexInBounds(code, i-1) && isIndexInBounds(code, i+2)){
                    bool result;
                    if(std::any_cast<std::string>(code[i-1].data[0]) == std::any_cast<std::string>(code[i+2].data[0])){
                        if (std::any_cast<std::string>(code[i-1].data[0]) == "int"){
                            if (std::stof(std::any_cast<std::string>(code[i-1].data[1])) == std::stof(std::any_cast<std::string>(code[i+2].data[1]))){
                                result = true;
                            }else{
                                result = false;
                            }
                        }else if(std::any_cast<std::string>(code[i-1].data[0]) == "str"){
                            if (std::any_cast<std::string>(code[i-1].data[1]) == std::any_cast<std::string>(code[i+2].data[1])){
                                result = true;
                            }else{
                                result = false;
                            }
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Incompatable type!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: inclompatable type.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }
                        
                    }else{
                        result = false;
                    }

                    if (result){
                        code[i-1].data[0] = (std::string)"int";
                        code[i-1].data[1] = (std::string)"1";
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        i -= 3;
                    }else{
                        code[i-1].data[0] = (std::string)"int";
                        code[i-1].data[1] = (std::string)"0";
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        i -= 3;
                    }
                    
                }
            }
        }else if (std::any_cast<std::string>(code[i].data[0]) == "keyword" && std::any_cast<std::string>(code[i].data[1]) == "!" && isIndexInBounds(code, i+1)){
            if (std::any_cast<std::string>(code[i+1].data[0]) == "keyword" && std::any_cast<std::string>(code[i+1].data[1]) == "="){
                if(isIndexInBounds(code, i-1) && isIndexInBounds(code, i+2)){
                    bool result;
                    if(std::any_cast<std::string>(code[i-1].data[0]) == std::any_cast<std::string>(code[i+2].data[0])){
                        if (std::any_cast<std::string>(code[i-1].data[0]) == "int"){
                            if (std::stof(std::any_cast<std::string>(code[i-1].data[1])) == std::stof(std::any_cast<std::string>(code[i+2].data[1]))){
                                result = true;
                            }else{
                                result = false;
                            }
                        }else if(std::any_cast<std::string>(code[i-1].data[0]) == "str"){
                            if (std::any_cast<std::string>(code[i-1].data[1]) == std::any_cast<std::string>(code[i+2].data[1])){
                                result = true;
                            }else{
                                result = false;
                            }
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Incompatable type!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: inclompatable type.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }
                        
                    }else{
                        result = false;
                    }

                    if (!result){
                        code[i-1].data[0] = (std::string)"int";
                        code[i-1].data[1] = (std::string)"1";
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        i -= 3;
                    }else{
                        code[i-1].data[0] = (std::string)"int";
                        code[i-1].data[1] = (std::string)"0";
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        i -= 3;
                    }
                    
                }
            }
        }


        i++;
    }

    i = 0;
    while(i<code.size()){
        if(std::any_cast<std::string>(code[i].data[0]) == "word" && std::any_cast<std::string>(code[i].data[1]) == "not"){
            if(isIndexInBounds(code, i+1)){
                std::string result;
                if(std::any_cast<std::string>(code[i+1].data[0]) == "word"){
                    if (std::any_cast<std::string>(code[i+1].data[1]) == "true"){
                        result = "0";
                    }else if(std::any_cast<std::string>(code[i+1].data[1]) == "false"){
                        result = "1";
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected true or false!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected true or false.", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }
                }else if(std::any_cast<std::string>(code[i+1].data[0]) == "int"){
                    if (std::any_cast<std::string>(code[i+1].data[1]) == "1"){
                        result = "0";
                    }else if(std::any_cast<std::string>(code[i+1].data[1]) == "0"){
                        result = "1";
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected 1 or 0!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected 1 or 0.", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }
                }else{
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected bool!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected bool.", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }

                code[i].data[0] = (std::string)"int";
                code[i].data[1] = result;
            }else{
                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected bool after not!", PRINT_WHITE, PRINT_ERROR);
                //print("ERROR: Expected bool after not.", PRINT_WHITE, PRINT_ERROR);
                freedlibs();
                exit(-1);
            }
        }
        i++;
    }
                              

    i = 0;
    while (i<code.size()){
        if(std::any_cast<std::string>(code[i].data[0]) == "word"){
            if(std::any_cast<std::string>(code[i].data[1]) == "and"){
                if (isIndexInBounds(code, i-1) && isIndexInBounds(code, i+1)){
                    bool arg1;
                    bool arg2;
                    if(std::any_cast<std::string>(code[i-1].data[0]) == "word"){
                        if (std::any_cast<std::string>(code[i-1].data[1]) == "true"){
                            arg1 = true;
                        }else if(std::any_cast<std::string>(code[i-1].data[1]) == "false"){
                            arg1 = false;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected true or false!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected true or false.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }
                    }else if(std::any_cast<std::string>(code[i-1].data[0]) == "int"){
                        if (std::any_cast<std::string>(code[i-1].data[1]) == "1"){
                            arg1 = true;
                        }else if(std::any_cast<std::string>(code[i-1].data[1]) == "0"){
                            arg1 = false;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected 1 or 0!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected 1 or 0.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected bool!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected bool.", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }

                    if(std::any_cast<std::string>(code[i+1].data[0]) == "word"){
                        if (std::any_cast<std::string>(code[i+1].data[1]) == "true"){
                            arg2 = true;
                        }else if(std::any_cast<std::string>(code[i+1].data[1]) == "false"){
                            arg2 = false;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected true or false!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected true or false.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }
                    }else if(std::any_cast<std::string>(code[i+1].data[0]) == "int"){
                        if (std::any_cast<std::string>(code[i+1].data[1]) == "1"){
                            arg2 = true;
                        }else if(std::any_cast<std::string>(code[i+1].data[1]) == "0"){
                            arg2 = false;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected 1 or 0!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected 1 or 0.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected bool!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected bool.", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }
                    bool result = arg1 && arg2;
                    if(result){
                        code[i-1].data[0] = (std::string)"int";
                        code[i-1].data[1] = (std::string)"1";
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        i -= 2;
                    }else{
                        code[i-1].data[0] = (std::string)"int";
                        code[i-1].data[1] = (std::string)"0";
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        i -= 2;
                    }
                }else{
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected bool before and after end!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected bool before and after and.", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }
            }else if(std::any_cast<std::string>(code[i].data[1]) == "or"){
                if (isIndexInBounds(code, i-1) && isIndexInBounds(code, i+1)){
                    bool arg1;
                    bool arg2;
                    if(std::any_cast<std::string>(code[i-1].data[0]) == "word"){
                        if (std::any_cast<std::string>(code[i-1].data[1]) == "true"){
                            arg1 = true;
                        }else if(std::any_cast<std::string>(code[i-1].data[1]) == "false"){
                            arg1 = false;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected true or false!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected true or false.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }
                    }else if(std::any_cast<std::string>(code[i-1].data[0]) == "int"){
                        if (std::any_cast<std::string>(code[i-1].data[1]) == "1"){
                            arg1 = true;
                        }else if(std::any_cast<std::string>(code[i-1].data[1]) == "0"){
                            arg1 = false;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected 1 or 0!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected 1 or 0.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected bool!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected bool.", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }

                    if(std::any_cast<std::string>(code[i+1].data[0]) == "word"){
                        if (std::any_cast<std::string>(code[i+1].data[1]) == "true"){
                            arg2 = true;
                        }else if(std::any_cast<std::string>(code[i+1].data[1]) == "false"){
                            arg2 = false;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected true or false!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected true or false.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }
                    }else if(std::any_cast<std::string>(code[i+1].data[0]) == "int"){
                        if (std::any_cast<std::string>(code[i+1].data[1]) == "1"){
                            arg2 = true;
                        }else if(std::any_cast<std::string>(code[i+1].data[1]) == "0"){
                            arg2 = false;
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected 1 or 0!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected 1 or 0.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected bool!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected bool.", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }
                    bool result = arg1 || arg2;
                    if(result){
                        code[i-1].data[0] = (std::string)"int";
                        code[i-1].data[1] = (std::string)"1";
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        i -= 2;
                    }else{
                        code[i-1].data[0] = (std::string)"int";
                        code[i-1].data[1] = (std::string)"0";
                        removeAtIndex(code, i);
                        removeAtIndex(code, i);
                        i -= 2;
                    }
                }else{
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected bool before and after or!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected bool before and after or.", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }
            }
        }
        i++;
    }




    //code.at(0).print();
    //std::cout << std::any_cast<std::string>(code.at(0).data.at(1));
    if (code.size() == 0){
        Token result;
        result.data.push_back((std::string)"str");
        result.data.push_back((std::string)"None");
        code.push_back(result);
    }
    return code.at(0);
}

//void exec(std::vector<Token> code){
//
//}


std::vector<Token> CTokens(std::string code){
    code += " ";
    std::vector<Token> ret;
    int length = code.length();
    int debug_line_count = 1;
    int debug_index_count = 1;
    //ret.reserve(1000);

    int i = 0;
    int last_space = 0;
    bool num = false;
    bool str = false;
    bool IS = false;//inside string
    int LSC = 0;//last string cut
    Token token = Token();
    while (i < length){
        if (code[i] == ';' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)";");
            //token.data = ';';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '+' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"+");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '+';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '-' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"-");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '-';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '*' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"*");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '*';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '/' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"/");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '/';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }
        else if (code[i] == '%' && !IS)
        {
            str = false;
            if (num)
            {
                token.data.push_back((std::string) "int");
                // token.type = "int";
                num = false;
            }
            else
            {
                token.data.push_back((std::string) "word");
                // token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i - last_space));
            token.data.push_back((std::string) "line: " + std::to_string(debug_line_count) + ":" + std::to_string(debug_index_count));
            // token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string) "keyword");
            // token.type = "keyword";
            token.data.push_back((std::string) "%");
            token.data.push_back((std::string) "line: " + std::to_string(debug_line_count) + ":" + std::to_string(debug_index_count));
            // token.data = '%';
            ret.push_back(token);
            token.data.clear();
            last_space = i + 1;
        }else if (code[i] == '^' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"^");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '^';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '"'){
            str = false;
            if (!IS){
                if (num){
                    token.data.push_back((std::string)"int");
                    //token.type = "int";
                    num = false;
                }else{
                    token.data.push_back((std::string)"word");
                    //token.type = "word";
                }
                token.data.push_back(code.substr(last_space, i-last_space));
                token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
                //token.data = code.substr(last_space, i-last_space);
                ret.push_back(token);
                token.data.clear();
                IS = true;
                LSC = i+1;
            }else{
                token.data.push_back((std::string)"str");
                //token.type = "str";
                token.data.push_back(code.substr(LSC, i-LSC));
                token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
                //token.data = code.substr(LSC, i-LSC);
                ret.push_back(token);
                //std::cout << "data: " << std::any_cast<std::string>(token.data[1]) << "type: " << std::any_cast<std::string>(token.data[0]) << "\n";
                token.data.clear();
                IS = false;
                LSC = 0;
            }
            //token.data = '"';
            //token.type = "keyword";
            last_space = i+1;
        }else if (code[i] == '(' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"(");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '(';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == ')' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)")");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = ')';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '{' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"{");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '{';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '}' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"}");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '}';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '<' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"<");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '<';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '>' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)">");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '>';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == ',' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)",");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = ',';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '.' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)".");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '.';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '=' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"=");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '=';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '&' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"&");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '&';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '!' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;    
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"!");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '!';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        }else if (code[i] == '|' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            token.data.push_back((std::string)"keyword");
            //token.type = "keyword";
            token.data.push_back((std::string)"|");
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = '|';
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
        //}else if (code[i] == ';' && !IS){
        //    num = false;
        //    str = false;
        //    token.data = code.substr(last_space, i-last_space);
        //    token.type = "word";
        //    ret.push_back(token);
        //    token.data = ';';
        //    token.type = "keyword";
        //    ret.push_back(token);
        //    last_space = i+1;
        }else if (code[i] == ' ' && !IS){
            str = false;
            if (num){
                token.data.push_back((std::string)"int");
                //token.type = "int";
                num = false;
            }else{
                token.data.push_back((std::string)"word");
                //token.type = "word";
            }
            token.data.push_back(code.substr(last_space, i-last_space));
            token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
            //token.data = code.substr(last_space, i-last_space);
            ret.push_back(token);
            token.data.clear();
            last_space = i+1;
          
        }else if(isdigit(code[i]) && !IS){
            //printf("NUM: %i, i: %i\n", num, i);
            //str = false;
            if (!num && !str){
                token.data.push_back((std::string)"word");
                //token.type = "word";
                token.data.push_back(code.substr(last_space, i-last_space));
                token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
                //token.data = code.substr(last_space, i-last_space);
                //token.print();
                //std::cout << code.substr(last_space, i-last_space) << "\n";
                ret.push_back(token);
                token.data.clear();
                last_space = i;
                num = true;
            }
        }else if (code[i] == '\n' && !IS){
            debug_line_count++;
            debug_index_count = 1;
        }else if (code[i] == '\r' && !IS){
        }else if(isalpha(code[i]) && !IS){
            if (!str){
                if (num){
                    token.data.push_back((std::string)"int");
                    //token.type = "int";
                    num = false;
                }else{
                    token.data.push_back((std::string)"word");
                    //token.type = "word";
                }
                token.data.push_back(code.substr(last_space, i-last_space));
                token.data.push_back((std::string)"line: "+std::to_string(debug_line_count)+":"+std::to_string(debug_index_count));
                //token.data = code.substr(last_space, i-last_space);
                ret.push_back(token);
                token.data.clear();
                last_space = i;
                str = true;
            }
          
        }else{
            //std::cout << "Syntax error at line " << debug_line_count << ", at charachter " << debug_index_count<< "\n";
            //printf("char: '%c', int: %d\n", code[i], code[i]);
            //std::cout << "char '" << code[i] << "'\n";
        }
        i++;
        debug_index_count++;
    }

    i = 0;
    while (i < ret.size()){
        if (std::any_cast<std::string>(ret.at(i).data.at(1)) == "" || std::any_cast<std::string>(ret.at(i).data.at(1))[0] == '\r' || std::any_cast<std::string>(ret.at(i).data.at(1))[0] == '\n'){
        //if (ret.at(i).data == "" || ret.at(i).data[0] == '\r'){
            if(std::any_cast<std::string>(ret.at(i).data.at(0)) != "str"){
                ret.erase(ret.begin()+i);
                i--;
            }
        }
        i++;
    }

    return ret;
}

void interpret(std::vector<Token> code, Venv* venv, std::string path){
    int i = 0;
    int x = 0;
    int last_cut = 0;
    //int 
    while(i<code.size()){
        //std::cout << "--data: " << std::any_cast<std::string>(code[i].data.at(1)) << ", type: " << std::any_cast<std::string>(code[i].data.at(0)) << "\n";
        if (std::any_cast<std::string>(code[i].data[0]) == "word"){
            if (std::any_cast<std::string>(code[i].data[1]) == "var"){
                if (isIndexInBounds(code, i+1)){
                    if (std::any_cast<std::string>(code[i+1].data[0]) == "word"){
                        Var Nvar;
                        Nvar.name = std::any_cast<std::string>(code[i+1].data[1]);
                        Token InitVarToken;
                        InitVarToken.data.push_back((std::string)"str");
                        InitVarToken.data.push_back((std::string)"None");
                        Nvar.data = InitVarToken;
                        venv->vars.push_back(Nvar);
                        //new and might be broken.... PLEASE MYSELF TEST THIS SHIT....
                        i++;
                        last_cut = i+1;
                        //std::cout << Nvar.name << '\n';
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected var name after var, got non word type!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected var name after var, got non word type.", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }
                }else{
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected var name after var!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected var name after var.", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }
            }else if(std::any_cast<std::string>(code[i].data[1]) == "print"){
                std::string finalmsg = "";
                std::string message = "";
                Token result;
                std::vector<Token> subvector;
                x = i;
                int startindex = i;
                int end1;
                bool fin;
                bool end = false;
                while (!end){
                    if (x==code.size()){
                        i = x;
                        end = true;
                    }
                    if (std::any_cast<std::string>(code[x].data[1]) == ";" && std::any_cast<std::string>(code[x].data[0]) == "keyword"){
                        message += "\033[0m";
                        finalmsg += message;
                        message = "";
                        i = x;
                        end = true;
                    }
                    if (std::any_cast<std::string>(code[x].data[1]) == "<" && std::any_cast<std::string>(code[x].data[0]) == "keyword" && isIndexInBounds(code, x+2)){
                        if (std::any_cast<std::string>(code[x+1].data[1]) == "<" && std::any_cast<std::string>(code[x+1].data[0]) == "keyword"){
                            message += "\033[0m";
                            finalmsg += message;
                            message = "";
                            //if (std::any_cast<std::string>(code[x+2].data[0]) == "str"){
                            end1 = x+2;
                            fin = false;
                            while (!fin){
                                if (std::any_cast<std::string>(code[end1].data[1]) == ";" && std::any_cast<std::string>(code[end1].data[0]) == "keyword"){
                                    end1 -= 1;
                                    fin = true;
                                }
                                if (!fin && std::any_cast<std::string>(code[end1].data[1]) == "<" && std::any_cast<std::string>(code[end1].data[0]) == "keyword" && isIndexInBounds(code, end1+1)){
                                    if (std::any_cast<std::string>(code[end1+1].data[1]) == "<" && std::any_cast<std::string>(code[end1+1].data[0]) == "keyword"){
                                        end1 -= 1;
                                        fin = true;
                                    }
                                }
                                if (!fin && std::any_cast<std::string>(code[end1].data[1]) == "c" && std::any_cast<std::string>(code[end1].data[0]) == "word" && isIndexInBounds(code, end1+1)){
                                    if (std::any_cast<std::string>(code[end1+1].data[1]) == "<" && std::any_cast<std::string>(code[end1+1].data[0]) == "keyword"){
                                        end1 -= 1;
                                        fin = true;
                                    }
                                }
                                if (!fin){
                                    end1++;
                                }
                            }
                            //std::cout << "start: " << x+2 << " end: " << end1 << "\n";
                            
                            if (end1<x+2){
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected string!", PRINT_WHITE, PRINT_ERROR);
                                //print("ERROR: Expected str", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }
                            subvector = getSubvector(code, x+2, end1);
                            //for (Token element : subvector) {
                            //    std::cout << "data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                            //}
                            result = eval(subvector, venv, path);
                            if (std::any_cast<std::string>(result.data.at(0)) == "str"){
                                message += std::any_cast<std::string>(result.data.at(1));
                            }else if(std::any_cast<std::string>(result.data.at(0)) == "int"){
                                message += std::any_cast<std::string>(result.data.at(1));
                            }else{
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected str!", PRINT_WHITE, PRINT_ERROR);
                                //print("ERROR: Expected str", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }
                            
                        }
                    }else if(std::any_cast<std::string>(code[x].data[1]) == "c" && std::any_cast<std::string>(code[x].data[0]) == "word" && isIndexInBounds(code, x+2)){
                        if (std::any_cast<std::string>(code[x+1].data[1]) == "<" && std::any_cast<std::string>(code[x+1].data[0]) == "keyword"){
                            end1 = x+2;
                            fin = false;
                            while (!fin){
                                if (std::any_cast<std::string>(code[end1].data[1]) == ";" && std::any_cast<std::string>(code[end1].data[0]) == "keyword"){
                                    end1 -= 1;
                                    fin = true;
                                }
                                if (!fin && std::any_cast<std::string>(code[end1].data[1]) == "<" && std::any_cast<std::string>(code[end1].data[0]) == "keyword" && isIndexInBounds(code, end1+1)){
                                    if (std::any_cast<std::string>(code[end1+1].data[1]) == "<" && std::any_cast<std::string>(code[end1+1].data[0]) == "keyword"){
                                        end1 -= 1;
                                        fin = true;
                                    }
                                }
                                if (!fin && std::any_cast<std::string>(code[end1].data[1]) == "c" && std::any_cast<std::string>(code[end1].data[0]) == "word" && isIndexInBounds(code, end1+1)){
                                    if (std::any_cast<std::string>(code[end1+1].data[1]) == "<" && std::any_cast<std::string>(code[end1+1].data[0]) == "keyword"){
                                        end1 -= 1;
                                        fin = true;
                                    }
                                }
                                if (!fin){
                                    end1++;
                                }
                            }
                            //std::cout << "start: " << x+2 << " end: " << end1 << "\n";
                            
                            if (end1<x+2){
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected string!", PRINT_WHITE, PRINT_ERROR);
                                //print("ERROR: Expected str", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }
                            subvector = getSubvector(code, x+2, end1);
                            //for (Token element : subvector) {
                            //    std::cout << "data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                            //}
                            result = eval(subvector, venv, path);
                            if (std::any_cast<std::string>(result.data.at(0)) == "int"){
                                if (std::stof(std::any_cast<std::string>(result.data[1]))>=0 && std::stof(std::any_cast<std::string>(result.data[1]))<=7){
                                    message = "\033[" + std::to_string(30+std::stoi(std::any_cast<std::string>(result.data[1]))) + "m" + message;
                                }else{
                                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Colour should be within the range 0 and 7!", PRINT_WHITE, PRINT_ERROR);
                                    //print("ERROR: colour should be within the range 0 and 7.", PRINT_WHITE, PRINT_ERROR);
                                    freedlibs();
                                    exit(-1);
                                }
                                //message += std::any_cast<std::string>(result.data.at(1));
                            }else{
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected integer!", PRINT_WHITE, PRINT_ERROR);
                                //print("ERROR: Expected int", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }


                            //if (std::any_cast<std::string>(code[x+2].data[0]) == "int"){
                            //    if (std::stof(std::any_cast<std::string>(code[x+2].data[1]))>=0 && std::stof(std::any_cast<std::string>(code[x+2].data[1]))<=7){
                            //        message += "\033[" + std::to_string(10+std::stof(std::any_cast<std::string>(code[x+2].data[1]))) + "m";
                            //    }else{
                            //        print("ERROR: colour should be within the range 0 and 7.", PRINT_WHITE, PRINT_ERROR);
                            //        exit(-1);
                            //    }
                            //}else{
                            //    print("ERROR: Expected str", PRINT_WHITE, PRINT_ERROR);
                            //    exit(-1);
                            //}
                            
                        }
                    }
                    x++;
                }
                //std::cout << "x: " << startindex << " i: " << i << "\n";
                while (i>=startindex){
                    //std::cout << std::any_cast<std::string>(code[i].data[1]) << " " << i << "\n";
                    removeAtIndex(code, i);
                    i--;
                }
                last_cut = i+1;
                
                std::cout << finalmsg;
            }else if (std::any_cast<std::string>(code[i].data[1]) == "println"){
                std::string finalmsg = "";
                std::string message = "";
                Token result;
                std::vector<Token> subvector;
                x = i;
                int startindex = i;
                int end1;
                bool fin;
                bool end = false;
                while (!end){
                    if (x==code.size()){
                        i = x;
                        end = true;
                    }
                    if (std::any_cast<std::string>(code[x].data[1]) == ";" && std::any_cast<std::string>(code[x].data[0]) == "keyword"){
                        message += "\033[0m";
                        finalmsg += message;
                        message = "";
                        i = x;
                        end = true;
                    }
                    if (std::any_cast<std::string>(code[x].data[1]) == "<" && std::any_cast<std::string>(code[x].data[0]) == "keyword" && isIndexInBounds(code, x+2)){
                        if (std::any_cast<std::string>(code[x+1].data[1]) == "<" && std::any_cast<std::string>(code[x+1].data[0]) == "keyword"){
                            message += "\033[0m";
                            finalmsg += message;
                            message = "";
                            //if (std::any_cast<std::string>(code[x+2].data[0]) == "str"){
                            end1 = x+2;
                            fin = false;
                            while (!fin){
                                if (std::any_cast<std::string>(code[end1].data[1]) == ";" && std::any_cast<std::string>(code[end1].data[0]) == "keyword"){
                                    end1 -= 1;
                                    fin = true;
                                }
                                if (!fin && std::any_cast<std::string>(code[end1].data[1]) == "<" && std::any_cast<std::string>(code[end1].data[0]) == "keyword" && isIndexInBounds(code, end1+1)){
                                    if (std::any_cast<std::string>(code[end1+1].data[1]) == "<" && std::any_cast<std::string>(code[end1+1].data[0]) == "keyword"){
                                        end1 -= 1;
                                        fin = true;
                                    }
                                }
                                if (!fin && std::any_cast<std::string>(code[end1].data[1]) == "c" && std::any_cast<std::string>(code[end1].data[0]) == "word" && isIndexInBounds(code, end1+1)){
                                    if (std::any_cast<std::string>(code[end1+1].data[1]) == "<" && std::any_cast<std::string>(code[end1+1].data[0]) == "keyword"){
                                        end1 -= 1;
                                        fin = true;
                                    }
                                }
                                if (!fin){
                                    end1++;
                                }
                            }
                            //std::cout << "start: " << x+2 << " end: " << end1 << "\n";
                            if (end1<x+2){
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected string!", PRINT_WHITE, PRINT_ERROR);
                                //print("ERROR: Expected str", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }
                            subvector = getSubvector(code, x+2, end1);
                            //for (Token element : subvector) {
                            //    std::cout << "data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                            //}
                            result = eval(subvector, venv, path);
                            if (std::any_cast<std::string>(result.data.at(0)) == "str"){
                                message += std::any_cast<std::string>(result.data.at(1));
                            }else if(std::any_cast<std::string>(result.data.at(0)) == "int"){
                                message += std::any_cast<std::string>(result.data.at(1));
                            }else{
                                std::cout << std::any_cast<std::string>(result.data.at(1)) << "\n";
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected string!", PRINT_WHITE, PRINT_ERROR);
                                //print("ERROR: Expected str", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }
                            
                        }
                    }else if(std::any_cast<std::string>(code[x].data[1]) == "c" && std::any_cast<std::string>(code[x].data[0]) == "word" && isIndexInBounds(code, x+2)){
                        if (std::any_cast<std::string>(code[x+1].data[1]) == "<" && std::any_cast<std::string>(code[x+1].data[0]) == "keyword"){
                            end1 = x+2;
                            fin = false;
                            while (!fin){
                                if (std::any_cast<std::string>(code[end1].data[1]) == ";" && std::any_cast<std::string>(code[end1].data[0]) == "keyword"){
                                    end1 -= 1;
                                    fin = true;
                                }
                                if (!fin && std::any_cast<std::string>(code[end1].data[1]) == "<" && std::any_cast<std::string>(code[end1].data[0]) == "keyword" && isIndexInBounds(code, end1+1)){
                                    if (std::any_cast<std::string>(code[end1+1].data[1]) == "<" && std::any_cast<std::string>(code[end1+1].data[0]) == "keyword"){
                                        end1 -= 1;
                                        fin = true;
                                    }
                                }
                                if (!fin && std::any_cast<std::string>(code[end1].data[1]) == "c" && std::any_cast<std::string>(code[end1].data[0]) == "word" && isIndexInBounds(code, end1+1)){
                                    if (std::any_cast<std::string>(code[end1+1].data[1]) == "<" && std::any_cast<std::string>(code[end1+1].data[0]) == "keyword"){
                                        end1 -= 1;
                                        fin = true;
                                    }
                                }
                                if (!fin){
                                    end1++;
                                }
                            }
                            //std::cout << "start: " << x+2 << " end: " << end1 << "\n";
                            if (end1<x+2){
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected string!", PRINT_WHITE, PRINT_ERROR);
                                //print("ERROR: Expected str", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }
                            subvector = getSubvector(code, x+2, end1);
                            //for (Token element : subvector) {
                            //    std::cout << "data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                            //}
                            result = eval(subvector, venv, path);
                            if (std::any_cast<std::string>(result.data.at(0)) == "int"){
                                if (std::stof(std::any_cast<std::string>(result.data[1]))>=0 && std::stof(std::any_cast<std::string>(result.data[1]))<=7){
                                    message = "\033[" + std::to_string(30+std::stoi(std::any_cast<std::string>(result.data[1]))) + "m" + message;
                                }else{
                                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected colour should be within the range 0 and 7!", PRINT_WHITE, PRINT_ERROR);
                                    //print("ERROR: colour should be within the range 0 and 7.", PRINT_WHITE, PRINT_ERROR);
                                    freedlibs();
                                    exit(-1);
                                }
                                //message += std::any_cast<std::string>(result.data.at(1));
                            }else{
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected int!", PRINT_WHITE, PRINT_ERROR);
                                //print("ERROR: Expected int", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }


                            //if (std::any_cast<std::string>(code[x+2].data[0]) == "int"){
                            //    if (std::stof(std::any_cast<std::string>(code[x+2].data[1]))>=0 && std::stof(std::any_cast<std::string>(code[x+2].data[1]))<=7){
                            //        message += "\033[" + std::to_string(10+std::stof(std::any_cast<std::string>(code[x+2].data[1]))) + "m";
                            //    }else{
                            //        print("ERROR: colour should be within the range 0 and 7.", PRINT_WHITE, PRINT_ERROR);
                            //        exit(-1);
                            //    }
                            //}else{
                            //    print("ERROR: Expected str", PRINT_WHITE, PRINT_ERROR);
                            //    exit(-1);
                            //}
                            
                        }
                    }
                    x++;
                }
                
                //std::cout << "x: " << startindex << " i: " << i << "\n";
                while (i>=startindex){
                    //std::cout << std::any_cast<std::string>(code[i].data[1]) << " " << i << "\n";
                    removeAtIndex(code, i);
                    i--;
                }
                last_cut = i+1;

                std::cout << finalmsg << "\n";
            }else if(std::any_cast<std::string>(code[i].data[1]) == "error"){
                std::string finalmsg = "";
                std::string message = "";
                Token result;
                std::vector<Token> subvector;
                x = i;
                int startindex = i;
                int end1;
                bool fin;
                bool end = false;
                while (!end){
                    if (x==code.size()){
                        i = x;
                        end = true;
                    }
                    if (std::any_cast<std::string>(code[x].data[1]) == ";" && std::any_cast<std::string>(code[x].data[0]) == "keyword"){
                        message += "\033[0m";
                        finalmsg += message;
                        message = "";
                        i = x;
                        end = true;
                    }
                    if (std::any_cast<std::string>(code[x].data[1]) == "<" && std::any_cast<std::string>(code[x].data[0]) == "keyword" && isIndexInBounds(code, x+2)){
                        if (std::any_cast<std::string>(code[x+1].data[1]) == "<" && std::any_cast<std::string>(code[x+1].data[0]) == "keyword"){
                            message += "\033[31m";
                            finalmsg += message;
                            message = "";
                            //if (std::any_cast<std::string>(code[x+2].data[0]) == "str"){
                            end1 = x+2;
                            fin = false;
                            while (!fin){
                                if (std::any_cast<std::string>(code[end1].data[1]) == ";" && std::any_cast<std::string>(code[end1].data[0]) == "keyword"){
                                    end1 -= 1;
                                    fin = true;
                                }
                                if (!fin && std::any_cast<std::string>(code[end1].data[1]) == "<" && std::any_cast<std::string>(code[end1].data[0]) == "keyword" && isIndexInBounds(code, end1+1)){
                                    if (std::any_cast<std::string>(code[end1+1].data[1]) == "<" && std::any_cast<std::string>(code[end1+1].data[0]) == "keyword"){
                                        end1 -= 1;
                                        fin = true;
                                    }
                                }
                                if (!fin && std::any_cast<std::string>(code[end1].data[1]) == "c" && std::any_cast<std::string>(code[end1].data[0]) == "word" && isIndexInBounds(code, end1+1)){
                                    if (std::any_cast<std::string>(code[end1+1].data[1]) == "<" && std::any_cast<std::string>(code[end1+1].data[0]) == "keyword"){
                                        end1 -= 1;
                                        fin = true;
                                    }
                                }
                                if (!fin){
                                    end1++;
                                }
                            }
                            //std::cout << "start: " << x+2 << " end: " << end1 << "\n";
                            
                            if (end1<x+2){
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected string!", PRINT_WHITE, PRINT_ERROR);
                                //print("ERROR: Expected str", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }
                            subvector = getSubvector(code, x+2, end1);
                            //for (Token element : subvector) {
                            //    std::cout << "data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                            //}
                            result = eval(subvector, venv, path);
                            if (std::any_cast<std::string>(result.data.at(0)) == "str"){
                                message += std::any_cast<std::string>(result.data.at(1));
                            }else if(std::any_cast<std::string>(result.data.at(0)) == "int"){
                                message += std::any_cast<std::string>(result.data.at(1));
                            }else{
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected str!", PRINT_WHITE, PRINT_ERROR);
                                //print("ERROR: Expected str", PRINT_WHITE, PRINT_ERROR);
                                freedlibs();
                                exit(-1);
                            }
                            
                        }
                    }else if(std::any_cast<std::string>(code[x].data[1]) == "c" && std::any_cast<std::string>(code[x].data[0]) == "word" && isIndexInBounds(code, x+2)){
                        if (std::any_cast<std::string>(code[x+1].data[1]) == "<" && std::any_cast<std::string>(code[x+1].data[0]) == "keyword"){
                            print("Calling error doesn't support color! ", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }
                    }
                    x++;
                }
                //std::cout << "x: " << startindex << " i: " << i << "\n";
                while (i>=startindex){
                    //std::cout << std::any_cast<std::string>(code[i].data[1]) << " " << i << "\n";
                    removeAtIndex(code, i);
                    i--;
                }
                last_cut = i+1;
                
                std::cout << finalmsg << "\n";
            }else if(std::any_cast<std::string>(code[i].data[1]) == "func"){
                if (isIndexInBounds(code, i+1) && std::any_cast<std::string>(code[i].data[0]) == "word"){
                    int startindex = i;
                    index indexdata = cutCH('(', ')', i+2, code);
                    //Venv functionvenv;
                    Var FunctionVar;
                    if (!indexdata.error){
                        std::vector<Token> subvector;
                        std::vector<std::vector<Token>> argvector;
                        if (indexdata.index1-1!=indexdata.index2){
                            subvector = getSubvector(code, indexdata.index1, indexdata.index2);
                            //for (Token element : subvector) {
                            //    std::cout << "data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                            //}
                            int x = 0;
                            int last = 0;
                            Token extraToken;
                            extraToken.data.push_back((std::string)"keyword");
                            extraToken.data.push_back((std::string)";");
                            while(x!=subvector.size()){
                                if (x+1==subvector.size()){
                                    argvector.push_back(getSubvector(subvector, last, x));
                                }
                                if (std::any_cast<std::string>(subvector[x].data[0]) == "keyword" && std::any_cast<std::string>(subvector[x].data[1]) == ","){
                                    argvector.push_back(getSubvector(subvector, last, x-1));
                                    last = x+1;
                                }

                                

                                x++;
                            }

                            //for (std::vector<Token> elements: argvector){
                            //    std::cout << "start\n";
                            //    for (Token element : elements) {
                            //        std::cout << "--data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                            //    }
                            //    std::cout << "end\n";
                            //}

                        }

                        //std::cout << indexdata.index2+2 << "\n";
                        index FunctionCodeIndexData = cutCH('{', '}', indexdata.index2+2, code);

                        std::vector<Token> FunctionCode;
                        //std::cout << "start: " << FunctionCodeIndexData.index1 << " end: " << FunctionCodeIndexData.index2 << " error: " << FunctionCodeIndexData.error << "\n"; 
                        if (!FunctionCodeIndexData.error){
                            if (FunctionCodeIndexData.index1-1!=FunctionCodeIndexData.index2){
                                FunctionCode = getSubvector(code, FunctionCodeIndexData.index1, FunctionCodeIndexData.index2);
                            }
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected '}' close!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected '}' close.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }

                        std::string FunctionName = std::any_cast<std::string>(code[i+1].data[1]);

                        if (venv->varin(FunctionName)){
                            removeAtIndex(venv->vars, venv->varindex(FunctionName));
                        }


                        FunctionVar.name = FunctionName;
                        FunctionVar.data.data.push_back((std::string)"func");
                        FunctionVar.data.data.push_back(FunctionName);
                        FunctionVar.data.data.push_back(FunctionCode);
                        FunctionVar.data.data.push_back(argvector);

                        venv->vars.push_back(FunctionVar);


                        i = FunctionCodeIndexData.index2+1;
                        last_cut = i+1;

                        while (i>=startindex){
                            
                            //std::cout << std::any_cast<std::string>(code[i].data[1]) << " " << i << "\n";
                            removeAtIndex(code, i);
                            i--;
                        }
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected ')' close!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected ')' close.", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }
                }else{
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected function name!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected function name", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }
            }else if(std::any_cast<std::string>(code[i].data[1]) == "return"){
                int x = i;
                while (!(std::any_cast<std::string>(code[x].data[0]) == "keyword" && std::any_cast<std::string>(code[x].data[1]) == ";")){
                    x++;
                    if (x==code.size()){
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected ; after return!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected ; after return", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }
                }
                Token result;

                if (i==x-1){
                    result.data.push_back((std::string)"str");
                    result.data.push_back((std::string)"None");
                }else{
                    result = eval(getSubvector(code, i+1, x-1), venv, path);
                }

                std::vector<Var*> venvvars = getvars(venv);
                venvvars[varinlistindex(venvvars, "Function_result_return_value")]->data = result;

                i = x+1;
                last_cut = i+1;

                //venv->vars[0].data = result;//return standard var                
            }else if(std::any_cast<std::string>(code[i].data[1]) == "EXECDLIBFUNC"){
                if(isIndexInBounds(code, i+1) && isIndexInBounds(code, i+2)){
                    //int ik = i;
                    //while (std::any_cast<std::string>(code[ik].data[1])!=";"){
                    //    std::cout << std::any_cast<std::string>(code[ik].data[1]) << " ";
                    //    ik++;
                    //}
                    //std::cout << "\n";
                    Token test;
                    test.data.push_back((std::string)"word");
                    test.data.push_back((std::string)"dlib");

                    //std::cout << std::any_cast<std::string>(eval({test}, venv, path).data[1]) << "\n";
                    //std::cout << std::any_cast<std::string>(venv->vars[venv->varindex(std::any_cast<std::string>(code[i+1].data[1]))].data.data[0]) << "\n";

                    Token dlllibToken = eval({code[i+1]}, venv, path);
                    //std::cout << "--data: " << std::any_cast<std::string>(dlllibToken.data.at(1)) << ", type: " << std::any_cast<std::string>(dlllibToken.data.at(0)) << "\n";
                    int dlib_id = std::stoi(std::any_cast<std::string>(dlllibToken.data.at(1)));
                    //std::cout << "sdf\n";
                    dlib DLIB = dlibs.at(dlib_id); //std::stoi(std::any_cast<std::string>(dlllibToken.data.at(1)))
                    Token dllfuncnameToken = eval({code[i+2]}, venv, path);
                    //std::cout << "--data: " << std::any_cast<std::string>(dllfuncnameToken.data.at(1)) << ", type: " << std::any_cast<std::string>(dllfuncnameToken.data.at(0)) << "\n";
                    x = i+3;
                    std::vector<transferableToken> resultinputvector;
                    bool end = false;
                    Token tempToken;
                    transferableToken TFToken;
                    
                    while(!end){
                        if(x==code.size()){
                            end = true;
                            break;
                        }
                        if(std::any_cast<std::string>(code[x].data[0]) == "keyword" && std::any_cast<std::string>(code[x].data[1]) == ";"){
                            end = true;
                            break;
                        }
                        tempToken = eval({code[x]}, venv, path);
                        TFToken.data = tempToken.data;
                        //TFToken.type = std::any_cast<std::string>(tempToken.data[0]);
                        //TFToken.data = std::any_cast<std::string>(tempToken.data[1]);
                        resultinputvector.push_back(TFToken);
                        x++;
                    }

                    //for (transferableToken element : resultinputvector) {
                    //    std::cout << "--data: " << std::any_cast<std::string>(element.data) << ", type: " << std::any_cast<std::string>(element.type) << "\n";
                    //}
                    




    

                    if(std::any_cast<std::string>(dllfuncnameToken.data.at(0)) == "str"){
                        std::string dlibfunctionname = std::any_cast<std::string>(dllfuncnameToken.data.at(1));
                        #if defined(_WIN32)
                            dlibfunc dlibfunction = (dlibfunc)GetProcAddress(DLIB.hDll, dlibfunctionname.c_str());
                            if (!dlibfunction) {
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Could not locate: " + dlibfunctionname + "!", PRINT_WHITE, PRINT_ERROR);
                                //std::cerr << "Could not locate function: " << dlibfunctionname << std::endl;
                                FreeLibrary(DLIB.hDll);
                                exit(-1);
                            }
                        #elif defined(__linux__)
                            dlibfunc dlibfunction = (dlibfunc)dlsym(DLIB.handle, dlibfunctionname.c_str());
                            const char* error = dlerror();
                            if (error) {
                                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Could not locate: " + error + "!", PRINT_WHITE, PRINT_ERROR);
                                //std::cerr << "Could not locate the function: " << error << std::endl;
                                dlclose(DLIB.handle);
                                exit(-1);
                            }
                        #endif
                        transferableToken resultTFToken = dlibfunction(resultinputvector, spnglobaldata);
                        Token Tokenresult;
                        //std::cout << "size: " << resultTFToken.data.size() << "\n";
                        Tokenresult.data.reserve(resultTFToken.data.size());
                        Tokenresult.data = resultTFToken.data;
                        //std::cout << "lkskhgdfdf\n";
                        
                        //Tokenresult.data.push_back(resultTFToken.type);
                        //Tokenresult.data.push_back(resultTFToken.data);
                        std::vector<Var*> venvvars = getvars(venv);
                        venvvars[varinlistindex(venvvars, "ExecFunctionReturnVar")]->data = Tokenresult;
                        //venv->vars.at(2).data = Tokenresult;
                        //std::cout << "sjdfslkds\n";
                    }

                    
                    // NEW FOR last_cut
                    i = x+1;
                    last_cut = i+1;
                    // END FOR last_cut

                }
            }else if(std::any_cast<std::string>(code[i].data[1]) == "if"){
                if(isIndexInBounds(code, i+1)){
                    if(std::any_cast<std::string>(code[i+1].data[0]) == "keyword" && std::any_cast<std::string>(code[i+1].data[1]) == "("){
                        index indexdata = cutCH('(', ')', i+1, code);
                        if(indexdata.index1<=indexdata.index2){
                            //std::cout << indexdata.index1 << " " << indexdata.index2 << "\n";
                            Token resultToken = eval(getSubvector(code, indexdata.index1, indexdata.index2), venv, path);
                            
                            bool BoolResult;
                            if(std::any_cast<std::string>(resultToken.data[0]) == "word"){
                                if(std::any_cast<std::string>(resultToken.data[1]) == "true"){
                                    BoolResult = true;
                                }else if(std::any_cast<std::string>(resultToken.data[1]) == "false"){
                                    BoolResult = false;
                                }else{
                                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected true or false!", PRINT_WHITE, PRINT_ERROR);
                                    //print("ERROR: Expected true or false.", PRINT_WHITE, PRINT_ERROR);
                                    freedlibs();
                                    exit(-1);
                                }
                            }else if(std::any_cast<std::string>(resultToken.data[0]) == "int"){
                                if(std::any_cast<std::string>(resultToken.data[1]) == "1"){
                                    BoolResult = true;
                                }else if(std::any_cast<std::string>(resultToken.data[1]) == "0"){
                                    BoolResult = false;
                                }else{
                                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected 1 or 0!", PRINT_WHITE, PRINT_ERROR);
                                    //print("ERROR: Expected 1 or 0.", PRINT_WHITE, PRINT_ERROR);
                                    freedlibs();
                                    exit(-1);
                                }
                            }
                            index ifindexdata = cutCH('{', '}', indexdata.index1+2, code);
                            if(BoolResult){
                                if(ifindexdata.index1<=ifindexdata.index2){
                                    std::vector<Token> ifcode = getSubvector(code, ifindexdata.index1, ifindexdata.index2);
                                    //for (Token element : ifcode) {
                                    //    std::cout << "--data: " << std::any_cast<std::string>(element.data[1]) << ", type: " << std::any_cast<std::string>(element.data[0]) << "\n";
                                    //}
                                    interpret(ifcode, venv, path);
                                }
                            }
                            i = ifindexdata.index2+1;
                            last_cut = i+1;

                            //std::cout << "--data: " << std::any_cast<std::string>(result.data[1]) << ", type: " << std::any_cast<std::string>(result.data[0]) << "\n";
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected true or false!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected true or false, got nothing.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }

                    }
                }
            }else if(std::any_cast<std::string>(code[i].data[1]) == "while"){
                if(isIndexInBounds(code, i+1)){
                    if(std::any_cast<std::string>(code[i+1].data[0]) == "keyword" && std::any_cast<std::string>(code[i+1].data[1]) == "("){
                        index indexdata = cutCH('(', ')', i+1, code);
                        if(indexdata.index1<=indexdata.index2){
                            //std::cout << indexdata.index1 << " " << indexdata.index2 << "\n";
                            index ifindexdata = cutCH('{', '}', indexdata.index1+2, code);
                            std::vector<Token> ifcode;
                            if(ifindexdata.index1<=ifindexdata.index2){
                                ifcode = getSubvector(code, ifindexdata.index1, ifindexdata.index2);
                            }else{
                                ifcode = {};
                            }
                            bool result = true;
                            bool BoolResult;
                            while (result){
                                Token resultToken = eval(getSubvector(code, indexdata.index1, indexdata.index2), venv, path);
                                
                                if(std::any_cast<std::string>(resultToken.data[0]) == "word"){
                                    if(std::any_cast<std::string>(resultToken.data[1]) == "true"){
                                        BoolResult = true;
                                    }else if(std::any_cast<std::string>(resultToken.data[1]) == "false"){
                                        BoolResult = false;
                                    }else{
                                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected true or false!", PRINT_WHITE, PRINT_ERROR);
                                        //print("ERROR: Expected true or false.", PRINT_WHITE, PRINT_ERROR);
                                        freedlibs();
                                        exit(-1);
                                    }
                                }else if(std::any_cast<std::string>(resultToken.data[0]) == "int"){
                                    if(std::any_cast<std::string>(resultToken.data[1]) == "1"){
                                        BoolResult = true;
                                    }else if(std::any_cast<std::string>(resultToken.data[1]) == "0"){
                                        BoolResult = false;
                                    }else{
                                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected 1 or 0!", PRINT_WHITE, PRINT_ERROR);
                                        //print("ERROR: Expected 1 or 0.", PRINT_WHITE, PRINT_ERROR);
                                        freedlibs();
                                        exit(-1);
                                    }
                                }
                                //index ifindexdata = cutCH('{', '}', indexdata.index1+2, code);
                                if(BoolResult){
                                    //if(ifindexdata.index1<=ifindexdata.index2){
                                    //    std::vector<Token> ifcode = getSubvector(code, ifindexdata.index1, ifindexdata.index2);
                                        //for (Token element : ifcode) {
                                        //    std::cout << "--data: " << std::any_cast<std::string>(element.data[1]) << ", type: " << std::any_cast<std::string>(element.data[0]) << "\n";
                                        //}
                                    interpret(ifcode, venv, path);
                                    //}
                                }else{
                                    result = false;
                                }
                                //i = ifindexdata.index2+2;
                            }
                            i = ifindexdata.index2+1;
                            last_cut = i+1;

                            //std::cout << "--data: " << std::any_cast<std::string>(result.data[1]) << ", type: " << std::any_cast<std::string>(result.data[0]) << "\n";
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected true or false, got nothing!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected true or false, got nothing.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }

                    }
                }
            }else if(std::any_cast<std::string>(code[i].data[1]) == "import"){
                bool end = false;
                int x = i+1;
                while(!end){
                    if(x==code.size()){
                        end = true;
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected ';'!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected \";\"", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }
                    if(std::any_cast<std::string>(code[x].data[0]) == "keyword" && std::any_cast<std::string>(code[x].data[1]) == ";"){
                        end = true;
                        continue;
                    }

                    x++;
                }
                //std::cout << i << " " << x << "\n";
                if(i+1==x){
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected file name!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected file name.", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }else{
                    Token spnfilename = eval(getSubvector(code, i+1, x-1), venv, path);
                    if(std::any_cast<std::string>(spnfilename.data[0]) == "str"){
                        //std::cout << std::any_cast<std::string>(spnfilename.data[1]) << "\n";
                        std::ifstream importfile(std::any_cast<std::string>(spnfilename.data[1])); // Open the file 
                        std::stringstream buffer; 
                        std::string fileContent;
                        if (importfile.is_open()) { 
                            buffer << importfile.rdbuf(); // Read the file into the buffer 
                            fileContent = buffer.str(); // Convert buffer to string 
                            //std::cout << "File content:\n" << fileContent << std::endl; inputFile.close(); // Close the file 
                        } else { 
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Unable to open file \"" + std::any_cast<std::string>(spnfilename.data[1]) + "!", PRINT_WHITE, PRINT_ERROR);
                            //std::cerr << "Unable to open file \"" << std::any_cast<std::string>(spnfilename.data[1]) << "\"" << std::endl; 
                            freedlibs();
                            exit(-1);
                        }


                        //set path to imported file's path

                        
                        std::string folderPath;
                        std::string filename = std::any_cast<std::string>(spnfilename.data[1]);
                        
                        
                        #if defined(_WIN32)
                            //size_t lastSlash = ((std::string)std::any_cast<std::string>(spnfilename.data[1])).find_last_of("\\/");
                            //folderPath = ((std::string)std::any_cast<std::string>(spnfilename.data[1])).substr(0, lastSlash);
                            //std::cout << folderPath << "\n";
                            
                            folderPath = dirname((char*)filename.c_str());
                            //std::cout << folderPath << "\n";
                            if (!SetCurrentDirectoryA(folderPath.c_str())){
                                print("CRITICAL WARNING at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Unablr to set proper path, file paths are going to fail!", PRINT_WHITE, PRINT_ERROR);
                                //std::cerr << "CRITICAL WARNING: Unable to set proper path, file paths are going to be incorrect!" << std::endl;
                                //exit(-1);
                            }
                        #elif defined(__linux__)
                            folderPath = dirname((char*)filename.c_str());
                            if (chdir(folderPath.c_str()) == 1){
                                print("CRITICAL WARNING at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Unablr to set proper path, file paths are going to fail!", PRINT_WHITE, PRINT_ERROR);
                                //std::cerr << "CRITICAL WARNING: Unable to set proper path, file paths are going to be incorrect!" << std::endl; 
                                //return 1;
                            }
                        #endif


                        interpret(CTokens(fileContent), venv, path);

                        //set the path back to normal
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected string type!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected str type.", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }
                }
                i = x;
                last_cut = i+1;
            }else if(std::any_cast<std::string>(code[i].data[1]) == "class"){
                if (isIndexInBounds(code, i+1) && std::any_cast<std::string>(code[i+1].data[0]) == "word"){
                    index indexdata = cutCH('{', '}', i+2, code);
                    Var classvar; 
                    Token classvarToken;
                    std::string classname = std::any_cast<std::string>(code[i+1].data[1]);
                    if(indexdata.index2+1!=indexdata.index1){
                        std::vector<Token> classcodevector = getSubvector(code, indexdata.index1, indexdata.index2);
                        classvarToken.data.push_back((std::string)"class");
                        classvarToken.data.push_back(classcodevector);//code
                        classvarToken.data.push_back(classname);
                    }else{
                        Token classvarToken;
                        classvarToken.data.push_back((std::string)"class");
                        classvarToken.data.push_back({});//code
                        classvarToken.data.push_back(classname);
                    }
                    classvar.name = classname;
                    classvar.data = classvarToken;
                    venv->vars.push_back(classvar);
                    i = indexdata.index2+1;
                    last_cut = i+1;
                }else{
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected class name!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected class name.", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }
            }else if(std::any_cast<std::string>(code[i].data[1]) == "operator"){
                if (isIndexInBounds(code, i+1) && std::any_cast<std::string>(code[i].data[0]) == "word"){
                    int startindex = i;
                    index indexdata = cutCH('(', ')', i+2, code);
                    //Venv functionvenv;
                    //Var FunctionVar;
                    Token operatorToken;
                    if (!indexdata.error){
                        std::vector<Token> subvector;
                        std::vector<std::vector<Token>> argvector;
                        if (indexdata.index1-1!=indexdata.index2){
                            subvector = getSubvector(code, indexdata.index1, indexdata.index2);
                            //for (Token element : subvector) {
                            //    std::cout << "data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                            //}
                            int x = 0;
                            int last = 0;
                            //Token extraToken;
                            //extraToken.data.push_back((std::string)"keyword");
                            //extraToken.data.push_back((std::string)";");
                            
                            while(x!=subvector.size()){
                                if (x+1==subvector.size()){
                                    argvector.push_back(getSubvector(subvector, last, x));
                                }
                                if (std::any_cast<std::string>(subvector[x].data[0]) == "keyword" && std::any_cast<std::string>(subvector[x].data[1]) == ","){
                                    argvector.push_back(getSubvector(subvector, last, x-1));
                                    last = x+1;
                                }

                                

                                x++;
                            }

                            

                        }

                        //std::cout << indexdata.index2+2 << "\n";
                        index FunctionCodeIndexData = cutCH('{', '}', indexdata.index2+2, code);

                        std::vector<Token> operatorCode;
                        //std::cout << "start: " << FunctionCodeIndexData.index1 << " end: " << FunctionCodeIndexData.index2 << " error: " << FunctionCodeIndexData.error << "\n"; 
                        if (!FunctionCodeIndexData.error){
                            if (FunctionCodeIndexData.index1-1!=FunctionCodeIndexData.index2){
                                operatorCode = getSubvector(code, FunctionCodeIndexData.index1, FunctionCodeIndexData.index2);
                            }
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected '}' close!", PRINT_WHITE, PRINT_ERROR);
                            //print("ERROR: Expected '}' close.", PRINT_WHITE, PRINT_ERROR);
                            freedlibs();
                            exit(-1);
                        }

                        std::string operatorName = std::any_cast<std::string>(code[i+1].data[1]);

                        //if (venv->varin(FunctionName)){
                        //    removeAtIndex(venv->vars, venv->varindex(FunctionName));
                        //}

                        //for (std::vector<Token> elements: argvector){
                        //    std::cout << "--start\n";
                        //    for (Token element : elements) {
                        //        std::cout << "--data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << "\n";
                        //    }
                        //    std::cout << "--end\n";
                        //}


                        operatorToken.data.push_back(operatorCode);
                        operatorToken.data.push_back(argvector);
                        //int hj = 0;
                        //int hk = 0;
                        //while (hj!=argvector.size()){
                        //    hk = 0;
                        //    while (hk!=argvector[hj].size()){
                        //        std::cout << std::any_cast<std::string>(argvector[hj][hk].data[0]) << "\n";
                        //        hk++;
                        //    }
//
//
                        //    hj++;
                        //}
                        //FunctionVar.name = FunctionName;
                        //FunctionVar.data.data.push_back((std::string)"func");
                        //FunctionVar.data.data.push_back(FunctionName);
                        //FunctionVar.data.data.push_back(FunctionCode);
                        //FunctionVar.data.data.push_back(argvector);

                        //venv->vars.push_back(FunctionVar);
                        if(operatorName == "INIT"){
                            //std::cout << "sdfs\n";
                            venv->operatorINIT = operatorToken;
                            //std::cout << venv << "\n";
                        }else if(operatorName == "STR"){
                            venv->operatorSTR = operatorToken;
                        }else if(operatorName == "PLUS"){
                            venv->operatorPLUS = operatorToken;
                        }else if(operatorName == "MINUS"){
                            venv->operatorMINUS = operatorToken;
                        }else if(operatorName == "MULT"){
                            venv->operatorMULT = operatorToken;
                        }else if(operatorName == "DIV"){
                            venv->operatorDIV = operatorToken;
                        }else if(operatorName == "POWER"){
                            venv->operatorPOWER = operatorToken;
                        }else if(operatorName == "AND"){
                            venv->operatorAND = operatorToken;
                        }else if(operatorName == "OR"){
                            venv->operatorOR = operatorToken;
                        }else if(operatorName == "NOT"){
                            venv->operatorNOT = operatorToken;
                        }

                        i = FunctionCodeIndexData.index2+1;

                        while (i>=startindex){
                            //std::cout << std::any_cast<std::string>(code[i].data[1]) << " " << i << "\n";
                            removeAtIndex(code, i);
                            i--;
                        }
                        last_cut = i+1;
                    }else{
                        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected ')' close!", PRINT_WHITE, PRINT_ERROR);
                        //print("ERROR: Expected ')' close.", PRINT_WHITE, PRINT_ERROR);
                        freedlibs();
                        exit(-1);
                    }
                }else{
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected operator type!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected operator type.", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }


            }
            //else if(venv->varin(std::any_cast<std::string>(code[i].data[1])) && isIndexInBounds(code, i+1)){
            //    int varindex = venv->varindex(std::any_cast<std::string>(code[i].data[1]));
            //    if (std::any_cast<std::string>(venv->vars[varindex].data.data[0]) != "object"){
            //        i++;
            //        continue;
            //    }
            //    int objectindex = std::any_cast<int>(venv->vars[varindex].data.data[1]);
            //    if(std::any_cast<std::string>(code[i+1].data[0]) == "keyword" && std::any_cast<std::string>(code[i+1].data[1]) == "."){
            //        int x = i+2;
            //        bool finished = false;
//
            //        while(!finished){
            //            if (x==code.size()){
            //                finished = true;
            //                print("ERROR: missing ';'.", PRINT_WHITE, PRINT_ERROR);
            //                exit(-1);
            //            }
            //            if (std::any_cast<std::string>(code[x].data[0]) == "keyword" && std::any_cast<std::string>(code[x].data[1]) == ";"){
            //                finished = true;
            //                break;
            //            }
            //            x++;
            //        }
//
            //        if(i+2!=x){
            //            std::vector<Token> objectcodevector = getSubvector(code, i+2, x);
            //            for (Token t: objectcodevector){
            //                std::cout << "type: " << std::any_cast<std::string>(t.data[0]) << " data: " << std::any_cast<std::string>(t.data[1]) << "\n";
            //            }
//
            //            Venv* objectVenv = &venv->objects[objectindex];
            //            std::cout << "pointer 2: " << objectVenv << "\n";
//
            //            interpret(objectcodevector, objectVenv, path);
            //            std::cout << "skdjfs\n";
//
            //            int y = 0;
            //            Venv MVenv = *objectVenv;
//
            //            while (y!=MVenv.vars.size()){
            //                if (MVenv.vars.at(y).data.data.size() == 2 && std::any_cast<std::string>(MVenv.vars.at(y).data.data.at(0)) != "class" && std::any_cast<std::string>(MVenv.vars.at(y).data.data.at(0)) != "object"){
            //                    std::cout << "name: \"" << MVenv.vars.at(y).name << "\" data: " << std::any_cast<std::string>(MVenv.vars.at(y).data.data.at(1)) << " type: " << std::any_cast<std::string>(MVenv.vars.at(y).data.data.at(0)) << "\n"; 
            //                }else if(MVenv.vars.at(y).data.data.size() == 3 || std::any_cast<std::string>(MVenv.vars.at(y).data.data.at(0)) == "class" || std::any_cast<std::string>(MVenv.vars.at(y).data.data.at(0)) == "object"){
            //                    std::cout << "name: \"" << MVenv.vars.at(y).name << "\" type: " << std::any_cast<std::string>(MVenv.vars.at(y).data.data.at(0)) << "\n"; 
            //                }
            //                y++;
            //            }
            //        }else{
            //            print("ERROR: Expected word.", PRINT_WHITE, PRINT_ERROR);
            //            exit(-1);
            //        }
//
            //        i = x;
            //    }
            //}
        }else if(std::any_cast<std::string>(code[i].data[0]) == "keyword"){
            if (std::any_cast<std::string>(code[i].data[1]) == "="){
                int startindex = i;
                if (isIndexInBounds(code, i-1)){
                    if (std::any_cast<std::string>(code.at(i-1).data.at(0)) == "word"){
                        std::string varN = std::any_cast<std::string>(code.at(i-1).data.at(1));
                        std::vector<Var*> venvvars = getvars(venv);
                        if (varinlist(venvvars, varN)){
                            Venv* varvenv;
                            if (venv->varin(varN)){
                                varvenv = venv;
                            }else if(venv->parent!=NULL){
                                bool end = false;
                                varvenv = venv->parent;
                                while (!end){
                                    if (varvenv->varin(varN)){
                                        end = true;
                                    }else{
                                        varvenv = varvenv->parent;
                                    }
                                }
                            }
                            bool finished = false;
                            int x = i+1;
                            int end = 0;
                            while (!finished){
                                if (x == code.size()){
                                    finished = true;
                                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected ';'!", PRINT_WHITE, PRINT_ERROR);
                                    //print("ERROR: Expected \";\"", PRINT_WHITE, PRINT_ERROR);
                                    freedlibs();
                                    exit(-1);
                                }
                                if (std::any_cast<std::string>(code[x].data[0]) == "keyword"){
                                    if (std::any_cast<std::string>(code[x].data[1]) == ";"){
                                        end = x-1;
                                        finished = true;
                                    }
                                }
                                
                                x++;
                            }
                            //std::cout << end << "\n";
                            //std::cout << "sdfsfdsdfsas p;k;lkm;pm'pol'p;l,';l,'p[hjl'pjlk'hgpljk'hpjlk'hj[plk'hjpkl'hjdfaksdgfkahsgdkfjas gkdjga skdjh gaksjdh fkasjdjf\n";
                            std::vector<Token> resultvector = getSubvector(code, i+1, end);
                            Token result = eval(resultvector, venv, path);
                            //std::cout << std::any_cast<std::string>(result.data[0]) << "\n";
                            
                            varvenv->vars.at(varvenv->varindex(varN)).data = result;
                            //std::cout << "var name: " << varN << "\n";
                            //std::cout << std::any_cast<std::string>(varvenv->vars.at(varvenv->varindex(varN)).data.data[1]) << "\n";
                            i = end;
                            last_cut = i+1;
                            //while(i>=startindex){
                            //    //std::cout << std::any_cast<std::string>(code[i].data[1]) << " " << i << "\n";
                            //    removeAtIndex(code, i);
                            //    i--;
                            //}
                            
                        }else{
                            print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Var \"" + varN + "\" is not defined!", PRINT_WHITE, PRINT_ERROR);
                            //std::cout << "var \"" << varN << "\" is not defined.\n";
                            freedlibs();
                            exit(-1);
                        }
                    }
                    //}else if(std::any_cast<std::string>(code.at(i-1).data.at(0)) == "keyword"){
                    //    if (!(isIndexInBounds(code, i-2) && std::any_cast<std::string>(code.at(i-2).data.at(0)) == "word")){
                    //        print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected var name before '='!", PRINT_WHITE, PRINT_ERROR);
                    //        //print("ERROR: Expected var name before '='.", PRINT_WHITE, PRINT_ERROR);
                    //        freedlibs();
                    //        exit(-1);
                    //    }
                    //    int operation;
                    //    if(std::any_cast<std::string>(code.at(i-1).data.at(1)) == "+"){
                    //        operation = 0;
                    //    }else if(std::any_cast<std::string>(code.at(i-1).data.at(1)) == "-"){
                    //        operation = 1;
                    //    }else if(std::any_cast<std::string>(code.at(i-1).data.at(1)) == "*"){
                    //        operation = 2;
                    //    }else if(std::any_cast<std::string>(code.at(i-1).data.at(1)) == "/"){
                    //        operation = 3;
                    //    }
//
                    //    std::string varN = std::any_cast<std::string>(code.at(i-2).data.at(1));
                    //    std::vector<Var*> venvvars = getvars(venv);
                    //    if (varinlist(venvvars, varN)){
                    //        Venv* varvenv;
                    //        if (venv->varin(varN)){
                    //            varvenv = venv;
                    //        }else if(venv->parent!=NULL){
                    //            bool end = false;
                    //            varvenv = venv->parent;
                    //            while (!end){
                    //                if (varvenv->varin(varN)){
                    //                    end = true;
                    //                }else{
                    //                    varvenv = varvenv->parent;
                    //                }
                    //            }
                    //        }
                    //        bool finished = false;
                    //        int x = i+1;
                    //        int end = 0;
                    //        while (!finished){
                    //            if (x == code.size()){
                    //                finished = true;
                    //                print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected ';'!", PRINT_WHITE, PRINT_ERROR);
                    //                //print("ERROR: Expected \";\"", PRINT_WHITE, PRINT_ERROR);
                    //                freedlibs();
                    //                exit(-1);
                    //            }
                    //            if (std::any_cast<std::string>(code[x].data[0]) == "keyword"){
                    //                if (std::any_cast<std::string>(code[x].data[1]) == ";"){
                    //                    end = x-1;
                    //                    finished = true;
                    //                }
                    //            }
                    //            
                    //            x++;
                    //        }
                    //        //std::cout << end << "\n";
                    //        //std::cout << "sdfsfdsdfsas p;k;lkm;pm'pol'p;l,';l,'p[hjl'pjlk'hgpljk'hpjlk'hj[plk'hjpkl'hjdfaksdgfkahsgdkfjas gkdjga skdjh gaksjdh fkasjdjf\n";
                    //        std::vector<Token> resultvector = getSubvector(code, i+1, end);
                    //        Token result = eval(resultvector, venv, path);
                    //        //std::cout << std::any_cast<std::string>(result.data[0]) << "\n";
                    //        if (operation == 0){
                    //            if (std::any_cast<std::string>(varvenv->vars.at(varvenv->varindex(varN)).data.data.at(0)) == "int"){
                    //                
                    //            }
                    //            //varvenv->vars.at(varvenv->varindex(varN)).data += result;
                    //        }
                    //        
                    //        //std::cout << "var name: " << varN << "\n";
                    //        //std::cout << std::any_cast<std::string>(varvenv->vars.at(varvenv->varindex(varN)).data.data[1]) << "\n";
                    //        i = end;
                    //        last_cut = i+1;
                    //        //while(i>=startindex){
                    //        //    //std::cout << std::any_cast<std::string>(code[i].data[1]) << " " << i << "\n";
                    //        //    removeAtIndex(code, i);
                    //        //    i--;
                    //        //}
//
//
//
//
                    //}
                }else{
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected var name before '='!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected var name before '='.", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }
            }else if(std::any_cast<std::string>(code[i].data[1]) == "("){
                index indexdata = cutCH('(', ')', i, code);
                //std::cout << indexdata.index2;

                i = indexdata.index2+1;
            }else if(std::any_cast<std::string>(code[i].data[1]) == ";"){
                if(i-last_cut>0){
                    //std::cout << "cuts: " << last_cut << " " << i-1 << "\n";
                    std::vector<Token> result = getSubvector(code, last_cut, i-1);
                    eval(result, venv, path);
                    last_cut = i+1;
                }else{
                    last_cut = i+1;

                }
            }else if(std::any_cast<std::string>(code[i].data[1]) == "."){
                //std::cout << "xsdsfd\n";
                if (!isIndexInBounds(code, i-1)){
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected object before '.'!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected object before \".\".", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }

                if (!isIndexInBounds(code, i+1)){
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected code after '.'!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected code after \".\".", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }

                int x = i-1;
                bool end = false;
                while (!end){
                    if (x == -1){
                        end == true;
                        x = 0;
                        break;
                    }

                    if (std::any_cast<std::string>(code[x].data[1]) == ";" && std::any_cast<std::string>(code[x].data[0]) == "keyword"){
                        end = true;
                        x++;
                        break;
                    }

                    x--;
                }

                Token objectToken = eval(getSubvector(code, x, i-1), venv, path);
                if(std::any_cast<std::string>(objectToken.data[0]) != "object"){
                    print("ERROR at " + std::any_cast<std::string>(code.at(i).data.at(2)) + ": Expected object before '.'!", PRINT_WHITE, PRINT_ERROR);
                    //print("ERROR: Expected object before \".\".", PRINT_WHITE, PRINT_ERROR);
                    freedlibs();
                    exit(-1);
                }
                int objectvenvindex = std::any_cast<int>(objectToken.data[1]);
                Venv* objectvenv = &venv->objects[objectvenvindex];

                //std::cout << "venv pointer: " << objectvenv << "\n";

                x = i+1;
                end = false;
                std::vector<Token> objectcode;

                while (!end){
                    if (x == code.size()){
                        end == true;
                        x--;
                        break;
                    }

                    if (std::any_cast<std::string>(code[x].data[1]) == ";" && std::any_cast<std::string>(code[x].data[0]) == "keyword"){
                        end = true;
                        break;
                    }

                    x++;
                }

                objectcode = getSubvector(code, i+1, x);

                interpret(objectcode, objectvenv, path);
                //std::cout << "object venv vars:\n";
                //for (Var t: objectvenv->vars){
                //    if (t.data.data.size() == 2 && std::any_cast<std::string>(t.data.data.at(0)) != "class"){
                //        std::cout << "name: \"" << t.name << "\" data: " << std::any_cast<std::string>(t.data.data.at(1)) << " type: " << std::any_cast<std::string>(t.data.data.at(0)) << "\n"; 
                //    }else if(t.data.data.size() == 3 || std::any_cast<std::string>(t.data.data.at(0)) == "class"){
                //        std::cout << "name: \"" << t.name << "\" type: " << std::any_cast<std::string>(t.data.data.at(0)) << "\n"; 
                //    }
                //}
//
                //std::cout << "\n\n\n";

                i = x;
                last_cut = i+1;
                
            }
        }
        i++;
    }
}


int main(int argc, char *argv[]){
    bool timer = true;
    //Venv venv;
    //Function func;
    //func.name = "panos";
    //venv.functions.push_back(func);
    //std::vector<Token> list;
    //Token t;
    //t.type = "word";
    //t.data = "panos";
    //list.push_back(t);
    //t.type = "keyword";
    //t.data = "(";
    //list.push_back(t);
    //t.data = "+";
    //list.push_back(t);
    //t.data = "panos";
    //list.push_back(t);
    //t.data = ")";
    //list.push_back(t);
    //Token res = eval(list, venv);
    //std::cout << res.data << " " << res.type << "\n";
    

    if (argc < 2){
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    




    std::ifstream inputFile(argv[1]); // Open the file 
    std::stringstream buffer; 
    std::string fileContent;
    if (inputFile.is_open()) { 
        buffer << inputFile.rdbuf(); // Read the file into the buffer 
        fileContent = buffer.str(); // Convert buffer to string 
        //std::cout << "File content:\n" << fileContent << std::endl; inputFile.close(); // Close the file 
    } else { 
        std::cerr << "Unable to open file" << std::endl; 
        return 1;
    }

    std::string folderPath;

    #if defined(_WIN32)
        size_t lastSlash = ((std::string)argv[1]).find_last_of("\\/");
        folderPath = ((std::string)argv[1]).substr(0, lastSlash);
        if (!SetCurrentDirectoryA(folderPath.c_str())){
            std::cerr << "CRITICAL WARNING: Unable to set proper path, file paths are going to fail!" << std::endl; 
            //return 1;
        }
    #elif defined(__linux__)
        folderPath = dirname(argv[1]);
        if (chdir(folderPath.c_str()) == 1){
            std::cerr << "CRITICAL WARNING: Unable to set proper path, file paths are going to fail!" << std::endl; 
            //return 1;
        }
    #endif

    
    auto start = std::chrono::high_resolution_clock::now();


    std::vector<Token> code = CTokens(fileContent);//"code;more code1324;extra;09;021"
    int i = 0;
    //for (Token element : code) {
    //    std::cout << "data: " << std::any_cast<std::string>(element.data.at(1)) << ", type: " << std::any_cast<std::string>(element.data.at(0)) << ", index: " << i << "\n";
    //    //printf("char: %c, int: %d\n", element.data.at(0), element.data.at(0)); 
    //    i++;
    //}
    //std::cout << "\n";
    Venv MVenv;
    MVenv.parent = NULL;
    Var MVenvResultVar;
    MVenvResultVar.name = "Function_result_return_value";
    MVenvResultVar.data.data.push_back((std::string)"str");
    MVenvResultVar.data.data.push_back((std::string)"None");
    MVenv.vars.push_back(MVenvResultVar);

    Var MVenvOSVar;
    MVenvOSVar.name = "CURRENT_RUNNING_OS";
    MVenvOSVar.data.data.push_back((std::string)"str");

    #if defined(_WIN32)
    //    #include <windows.h>
        MVenvOSVar.data.data.push_back((std::string)"WINDOWS");
    #elif defined(__linux__)
    //    #include <dlfcn.h>
        MVenvOSVar.data.data.push_back((std::string)"LINUX");
    #else
        print("WARNING: UNKNOWN OS, NO DLIB SUPPORT!");
        MVenvOSVar.data.data.push_back((std::string)"UNKNOWN");
    #endif

    MVenv.vars.push_back(MVenvOSVar);

    
    Var MVenvExecFunctionReturnVar;
    MVenvExecFunctionReturnVar.name = "ExecFunctionReturnVar";
    MVenvExecFunctionReturnVar.data.data.push_back((std::string)"str");
    MVenvExecFunctionReturnVar.data.data.push_back((std::string)"None");

    MVenv.vars.push_back(MVenvExecFunctionReturnVar);

    Var MVenvSPNVersion;
    MVenvSPNVersion.name = "SPNVERSION";
    MVenvSPNVersion.data.data.push_back((std::string)"int");
    MVenvSPNVersion.data.data.push_back((std::string)"2");

    MVenv.vars.push_back(MVenvSPNVersion);

    spnglobaldata.spnversion = 2;
    spnglobaldata.evalpointer = (void*)eval;
    spnglobaldata.evalpointer = (void*)interpret;
    spnglobaldata.Mvenv = (void*)&MVenv;
    //spnglobaldata.venv = (void*)&MVenv;


    interpret(code, &MVenv, folderPath);

    //i = 0;
    //while (i!=MVenv.vars.size()){
    //    if (MVenv.vars.at(i).data.data.size() == 2 && std::any_cast<std::string>(MVenv.vars.at(i).data.data.at(0)) != "class" && std::any_cast<std::string>(MVenv.vars.at(i).data.data.at(0)) != "object"){
    //        std::cout << "name: \"" << MVenv.vars.at(i).name << "\" data: " << std::any_cast<std::string>(MVenv.vars.at(i).data.data.at(1)) << " type: " << std::any_cast<std::string>(MVenv.vars.at(i).data.data.at(0)) << "\n"; 
    //    }else if(MVenv.vars.at(i).data.data.size() == 3 || std::any_cast<std::string>(MVenv.vars.at(i).data.data.at(0)) == "class" || std::any_cast<std::string>(MVenv.vars.at(i).data.data.at(0)) == "object"){
    //        std::cout << "name: \"" << MVenv.vars.at(i).name << "\" type: " << std::any_cast<std::string>(MVenv.vars.at(i).data.data.at(0)) << "\n"; 
    //    }
    //    i++;
    //}
    

    i = 0;

    freedlibs();

    
    if (timer){
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "\ntime: " << std::fixed << std::setprecision(6) << ((std::chrono::duration<double>)(end-start)).count() << "s\n";
    }
    


    //std::vector<Token> tokens;
    //Token token;
    //token.data = "0.1";
    //token.type = "int";
    //tokens.push_back(token);
    //token.data = "+";
    //token.type = "keyword";
    //tokens.push_back(token);
    //token.data = "0.2";
    //token.type = "int";
    //tokens.push_back(token);


    //std::cout << eval(code, Venv()).data << "\n";
    //std::cout << "kjld\n";
    exit(0);
}
