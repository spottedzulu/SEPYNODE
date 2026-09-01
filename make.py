import os
import sys

if sys.platform == "win32":
    os.system("g++ -shared -o ./src/stdlib/stdlib.dll ./src/stdlib/stdlib.cpp")
    os.system("g++ ./src/main.cpp -o main.exe")

elif sys.platform == "linux":
    os.system("g++ -fPIC -shared -o ./src/stdlib/stdlib.so ./src/stdlib/stdlib.cpp")
    os.system("g++ ./src/main.cpp -o main") 

elif sys.platform == "darwin":
    os.system("g++ -dynamiclib -o ./src/stdlib/stdlib.dylib ./src/stdlib/stdlib.cpp")
    os.system("g++ ./src/main.cpp -o main")# to be honest I don't think I added dlib support so it will probably crash lol

else:
    print("OS NOT SUPPORTED")
