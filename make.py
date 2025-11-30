import os
import sys


if sys.platform == "win32":
  os.system("g++ ./src/main.cpp -o main.exe")
elif sys.platform == "linux":
  os.system("g++ ./src/main.cpp -o main.o")
else:
    print("OS NOT SUPPORTED")
