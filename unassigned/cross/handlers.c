#include <stdio.h>
#include <stdlib.h>
#include "plp.h"

void plp_handle_rtype(char *a, char *b, char *c, char *d) {
	log_s(LOG_INST,"R-TYPE HANDLER:",a);
	sprintf(buf,"%s %s, %s, %s",a,b,c,d);
	output(buf);
}

void plp_handle_r2li(char *a, char *b, char *c) {
	log_s(LOG_INST,"R2LI HANDLER:",a);
	sprintf(buf,"%s %s, %s",a,b,c);
	output(buf);
}

void plp_handle_itype(char *a, char *b, char *c, char *d) {
	log_s(LOG_INST,"I-TYPE HANDLER:",a);
	sprintf(buf,"%s %s, %s, %s",a,b,c,d);
	output(buf);
}

void plp_handle_lwsw(char *a, char *b, char *c) {
	log_s(LOG_INST,"LWSW HANDLER:",a);
	sprintf(buf,"%s %s, %s",a,b,c);
	output(buf);
}

void plp_handle_branch(char *a, char *b, char *c, char *d) {
	log_s(LOG_INST,"BRANCH HANDLER:",a);
	sprintf(buf,"%s %s, %s, %s",a,b,c,d);
	output(buf);
}

void plp_handle_directive(char *a) {
	log_s(LOG_JUNK,"DROPPING DIRECTIVE:",a);
}

void plp_handle_label(char *a) {
	log_s(LOG_INST,"LABEL HANDLER:",a);
	sprintf(buf,"%s\n",a);
	output(buf);
}

void plp_handle_jump(char *a, char *b) {
	log_s(LOG_INST,"JUMP HANDLER:",a);
	sprintf(buf,"%s %s\n",a,b);
	output(buf);
}

