# Model Lab 
This program is created to practice loading 3D models, as well as experimenting different light techniques  

------

**Dependencies**  
1. GLEW  
2. SDL2  
3. GLM  
4. ImGui  

------

How to compile:  
```
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
```  
If exception occurs because of sh.exe on Windows, then change to  
```
cmake .. -G "MinGW Makefiles" -DCMAKE_SH=CMAKE_SH-NOTFOUND
```  
After that  
```
mingw32-make
```

------

The libs in externel folder are all for **64-bit Windows** systems  
The static lib for glew is self-compiled by myself, which will be system specific, and may cause errors