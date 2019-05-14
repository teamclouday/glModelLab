# Model Lab 
This program is created to practice loading 3D models, as well as experimenting different light techniques  

------

**Dependencies**  
1. GLEW  
2. SDL2  
3. GLM  
4. ImGui  
5. Assimp  
6. SOIL

------

How to compile:  

*On Windows Machine:*  
```
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```  
If exception occurs because of sh.exe on Windows, then change to  
```
cmake .. -G "MinGW Makefiles" -DCMAKE_SH=CMAKE_SH-NOTFOUND
```  

*On Linux(or Unix) Machine:*
```
mkdir build && cd build
cmake ..
make
```
If cmake fails because some packages are not found, be sure to install SDL2, glew, assimp, SOIL on the machine before cmake  

------

Don't forget to create a Models folder and put in models before run

------

Most libs in externel folder are for **64-bit Windows** systems, except `imgui` and `glm`  
The static lib for glew in external folder is self-compiled by myself, which will be system specific, and may be different on dfferent machines  