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

## Steps to compile the C-like compiler
flex, bison, clang and llvm are used to compile this project. Please ensure that the needed tools are installed.

### Windows OS
Run the script we provided and it will generate an executable named ``proj.exe``.
```
start.bat
```


### Linux OS
Run the Makefile we provided and it will generate an executable named ``proj``.
```
make
```


## Steps to run the C-like compiler
Before running the compiler ``proj.exe``, please ensure that the current directory contains a file named ``testdata.c``. The program ``proj.exe`` will read and compile the content in ``testdata.c``. 
```
proj.exe    //windows
./proj      //linux
```
the compiler ``proj.exe`` will generate an intermediate file named ``output.ll``. If you want to further optimize this intermediate code file, you can enter the following command:
```
opt -S -O3 output.ll
```
Now we need to compile and link the intermediate code files to generate executable files, which we can do through the following instructions:
```
clang output.ll -o output
```
Next, simply run the program by running output.exe/output.


