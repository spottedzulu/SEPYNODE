import os
import sys

if sys.platform == "win32":
    os.system("g++ -shared -o ./std/stdlib.dll ./std/stdlib.cpp")
    os.system("g++ .src/main.cpp -o main.exe")

elif sys.platform == "linux":
    os.system("g++ -fPIC -shared -o ./std/stdlib.so ./std/stdlib.cpp")
    os.system("g++ .src/main.cpp -o main") 

elif sys.platform == "darwin":
    os.system("g++ -dynamiclib -o ./std/stdlib.dylib ./std/stdlib.cpp")
    os.system("g++ .src/main.cpp -o main")

else:
    print("OS NOT SUPPORTED")
