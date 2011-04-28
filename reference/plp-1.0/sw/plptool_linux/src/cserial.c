#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include "cserial.h"
#include "clog.h"

int openDevice(char* device) {
	struct termios newtio, oldtio;
	int fd = open(device, O_RDWR | O_NOCTTY);
	if (fd < 0) 
		log_error("opening device");
	fcntl(fd, F_SETFL, 0);

	tcgetattr(fd,&oldtio);
	bzero(&newtio,sizeof(newtio));

	newtio.c_cflag = B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0; //ICANON;

	tcflush(fd,TCIOFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

	return fd;
}

char readChar(int fd) {
	char ret;
	while (read(fd,&ret,1) != 1);
	return ret;
}

void writeChar(int fd, char c) {
	write(fd,&c,1);
	tcdrain(fd);
}

