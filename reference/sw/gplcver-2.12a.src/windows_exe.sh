#!/bin/bash

# Create GPL Cver Windows Executable and support DLL
# Author: Brian Gordon
# Requires: make, gcc

# Create object files using Cygwin make file
cd src
make -f makefile.cygwin all

# Create DLL and executable using object files
cd ../objs
make -f makefile.dll dll
make -f makefile.dll exe

# Move resulting binaries to bin folder
mv cver.exe    ../bin
mv libcver.dll ../bin