#ifndef CSERIAL
#define CSERIAL

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

int openDevice(char* device);
char readChar(int fd);
void writeChar(int fd, char c);

#endif
