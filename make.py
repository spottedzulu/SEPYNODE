import os
import sys

if sys.platform == "win32":
    os.system("g++ -shared -o ./src/std/stdlib.dll ./src/std/stdlib.cpp")
    os.system("g++ ./src/main.cpp -o main.exe")

elif sys.platform == "linux":
    os.system("g++ -fPIC -shared -o ./src/std/stdlib.so ./src/std/stdlib.cpp")
    os.system("g++ ./src/main.cpp -o main") 

elif sys.platform == "darwin":
    #os.system("g++ -dynamiclib -o ./std/stdlib.dylib ./std/stdlib.cpp")
    os.system("g++ ./src/main.cpp -o main")# to be honest I don't think I added dlib support so it will probably crash lol

else:
    print("OS NOT SUPPORTED")
