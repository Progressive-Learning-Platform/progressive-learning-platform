#include <stdio.h>
#include <stdlib.h>
#include "plp.h"
#include "parser.tab.h"
#include "lex.yy.c"

/* for getopts */
#include <ctype.h>
#include <unistd.h>

void log_i(int level, char *str, int opt) {
	if (level <= glevel)
		printf("[i] %s %d\n",str, opt);	
}

void log_s(int level, char *str, char *str2) {
	if (level <= glevel)
		printf("[i] %s %s\n",str, str2);
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

void output(char *s) {
        log_s(LOG_ALL,"WRITING:",s);
        if (oFile != NULL)
                fprintf(oFile,"%s\n",s);
        else {
                printf("[e] output file not open!\n");
                exit(1);
        }
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
