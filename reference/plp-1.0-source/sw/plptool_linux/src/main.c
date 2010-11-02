/* David Fritz

3.4.2010

Tool to program the PLP board via RS232

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "clog.h"
#include "cserial.h"

#define BOARD_ID "plp0"

void getHeaderInfo(int *org, char* binaryName, char* s) {
	FILE *fp;
	char cmd[256];
	sprintf(cmd,"tar --to-command cat -xf %s header.mcasm",s);
	char temp[256];	

	fp = popen(cmd, "r");
	if (fp == NULL)
		log_error("Could not open file!");
	
	// get the org and binary name
	fgets(temp, 255, fp); //org
	*org = atoi(temp);
	fgets(binaryName, 255, fp); //binary name
	
	//trim the newline
	binaryName[strlen(binaryName)-1] = '\0';
	
	fclose(fp);
}

void extractBinary(char* filename, char* archive) {
	char cmd[256];
	sprintf(cmd,"tar -xf %s %s", archive, filename);
	system(cmd);
}

void removeBinary(char* b) {
	char cmd[256];
	sprintf(cmd, "rm %s", b);
	system(cmd);
}

int identify(int fd) {
	char ctemp[5];
	char temp[255];	
	writeChar(fd,'i');
	ctemp[0] = readChar(fd);
	ctemp[1] = readChar(fd);
	ctemp[2] = readChar(fd);
	ctemp[3] = readChar(fd);
	ctemp[4] = '\0';
	sprintf(temp,"got identity: %s",ctemp);
	log_info(temp);
	return strcmp(ctemp,BOARD_ID);
}

void write_org(int fd, int org) {
	writeChar(fd,'o');
	writeChar(fd,(char)((org>>24)	& 0x000000ff));
	writeChar(fd,(char)((org>>16)	& 0x000000ff));
	writeChar(fd,(char)((org>>8)	& 0x000000ff));
	writeChar(fd,(char)((org) 	& 0x000000ff));
}

void write_data(int fd, char* data) {
	writeChar(fd,'d');
	writeChar(fd,data[0]);
	writeChar(fd,data[1]);
	writeChar(fd,data[2]);
	writeChar(fd,data[3]);
}

void write_jump(int fd) {
	writeChar(fd,'j');
}

int main(int argc, char *argv[]) {
	char temp[256];
	char ctemp[5];
	int fd; //the serial device
	int org;
	char binaryName[256];
	FILE* binary;

	//check arguments
	if (argc != 3)
		log_error("Usage: plptool <executable> <serial port>");

	log_info("plptool starting up");
	
	//get the header info	
	getHeaderInfo(&org, binaryName, argv[1]);
	sprintf(temp,"Using binary executable %s", binaryName);
	log_info(temp);

	//open the file
	extractBinary(binaryName,argv[1]);
	binary = fopen(binaryName,"r");
	if (binary == NULL)
		log_error("Could not open extracted binary!");

	//open the serial port
	fd = openDevice(argv[2]);	
	
	//now everything is open and ready to rock, let's start by identifying the board
	log_info("requesting board identity");
	if (identify(fd) != 0)
		log_error("got invalid identity!");

	//we have the right board, let's write out our data
	write_org(fd, org);

	//read 4 bytes, and write it out
	while (!feof(binary)) {
		fread(ctemp,1,4,binary);
		write_data(fd,ctemp);
	}

	//data is loaded, prompt the user
	log_info("program loaded, press any key to start program");
	getc(stdin);

	write_jump(fd);

	close(fd);
	fclose(binary);

	//remove the temporary binary file
	removeBinary(binaryName);

	return 0;
}	
