#include <stdio.h>
#include <stdlib.h>
#include "plp.h"
#include <stdarg.h>
#include "parser.tab.h"
#include "lex.yy.c"

/* for getopts */
#include <ctype.h>
#include <unistd.h>

void log_i(int level, char *str, int opt) {
	if (level <= glevel)
		printf("[i] %d: %s %d\n",yyget_lineno(), str, opt);	
}

void log_s(int level, char *str, char *str2) {
	if (level <= glevel)
		printf("[i] %d: %s %s\n",yyget_lineno(), str, str2);
}

void handle_opts(int argc, char* argv[]) {
	char *dvalue = NULL;
	char *ivalue = NULL;
	char *ovalue = NULL;
	int c;

	opterr = 0;
	
	while ((c = getopt(argc, argv, "d:i:o:")) != -1)
		switch (c) {
			case 'd':
				dvalue = optarg;
				break;
			case 'i':
				ivalue = optarg;
				break;
			case 'o':
				ovalue = optarg;
				break;
			case '?':
				if (optopt == 'd' || 
				    optopt == 'i' || 
				    optopt == 'o'    )
					printf("[e] option -%c requires and argument.\n", optopt);
				else if (isprint (optopt))
					printf("[e] unknown option '-%c'.\n", optopt);
				else
					printf("[e] unkown option character '\\x%x'.\n",optopt);
				exit(1);
			default:
				exit(1);
		}
	
	if (dvalue != NULL) {
		glevel = atoi(dvalue);
		log_i(LOG_INFO,"setting debug value:", glevel);
	}
	if (ivalue != NULL) {
		inFile = ivalue;
		log_s(LOG_INFO,"setting input file:",inFile);
	} else {
		printf("[e] missing input file\n");
		exit(1);
	}
	if (ovalue != NULL) {
		outFile = ovalue;
		log_s(LOG_INFO,"setting output file:",outFile);
	} else {
		printf("[e] missing output file\n");
		exit(1);
	}
}

void output(char *prod) {
	fprintf(oFile,"%s\n",prod);
}

void craft(int n_args, ...) {
	va_list ap;
	int i;
	char prod[1024];

	va_start(ap, n_args);
	for (i=0; i < n_args; i++) {
		char *word = va_arg(ap, char*);
		if (i==0) /* opcode */
			sprintf(prod,"%s ",word);
		else if (i+1 == n_args) /* last word */
			sprintf(prod,"%s%s",prod,word);
		else /* some word in the middle */
			sprintf(prod,"%s%s,",prod,word);
	}
	va_end(ap);
	log_s(LOG_INST,"crafted instruction:",prod);
	output(prod);
}

int main(int argc, char *argv[]) {
	

	handle_opts(argc, argv);

	/* open files */
	log_s(LOG_ALL,"opening file:",inFile);
	iFile = fopen(inFile,"r");
	if (iFile == NULL) {
		printf("[e] cannot open input file\n");
		exit(1);
	}
	log_s(LOG_ALL,"opening file:",outFile);
	oFile = fopen(outFile,"w");
	if (oFile == NULL) {
		printf("[e] cannot open output file\n");
		exit(1);
	}
	yyset_in(iFile);


	/* hit it! */	
	yyparse();

	/* clean up */
	log_s(LOG_ALL,"closing file:",inFile);
	fclose(iFile);
	log_s(LOG_ALL,"closing file:",outFile);
	fclose(oFile);

	return 0;
}
