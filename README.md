# mycompiler
This is the final project of Computational theory at Zhejiang University.

## Description
This is a C-like compiler built by flex, bison and llvm.
The C-like language is a weaker version of C language.

We have implemented the following features in this C-like language:
* Arithmetic calculations
* Logical calculations
* If-Else statements
* Loops
* Function Call

## Steps to compile and run the program
flex, bison, clang and llvm are used to compile this project. Please ensure that the needed tools are installed.

### Windows OS
Run the script we provided and it will generate an executable named ``proj.exe``.
```
start.bat
```
Before running the program ``proj.exe``, please ensure that the current directory contains a file named ``testdata.c``. The program ``proj.exe`` will read and compile the content in ``testdata.c``.
```
start.bat
```