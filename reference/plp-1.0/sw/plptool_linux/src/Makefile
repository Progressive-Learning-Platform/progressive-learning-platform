CXX = g++
CC = gcc

# CPPFLAGS <= preprocessor flags (the include file  locations)
CPPFLAGS =  -I. 
# CFLAGS <= options for c compiler
CFLAGS = -O2 -Wall -g -m32

# LDFLAGS <= linker options (the libraries, and library locations)
LDFLAGS = -m32

# Here we list all of the executables.  No command is needed
all : plptool

# Now list the target:dependencies, and commands to build targets
main.o : main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c main.c

cserial.o : cserial.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c cserial.c

clog.o : clog.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c clog.c

plptool : main.o cserial.o clog.o
	$(CC) -o ../plptool $(LDFLAGS) main.o cserial.o clog.o

clean :
	rm main.o cserial.o clog.o

