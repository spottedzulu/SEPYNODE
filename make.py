import os
import sys


if sys.platform == "win32":
    os.system("g++ -shared -o ./std/stdlib.dll ./std/stdlib.cpp")
elif sys.platform == "linux":
    os.system("g++ -fPIC -shared -o ./std/stdlib.so ./std/stdlib.cpp")
elif sys.platform == "darwin":
    os.system("g++ -dynamiclib -o ./std/stdlib.dylib ./std/stdlib.cpp")
else:
    print("OS NOT SUPPORTED")
os.system("g++ main.cpp -o main.o")
