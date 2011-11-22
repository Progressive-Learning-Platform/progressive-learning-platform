/*
    Copyright 2011 the PLP authors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.tab.h"
#include "log.h"
#include "symbol.h"
#include "parse_tree.h"
#include "code_gen.h"
#include "line.h"

extern int yy_scan_string(const char*);
extern int yyparse(void);

int LOG_LEVEL = 2;
int STOP_ERROR = 1;
int NO_COMPILE = 0;
int ANNOTATE_SOURCE = 1;


symbol_table *sym = NULL;   /* scoped symbol tables */
symbol *constants = NULL;   /* constants */
symbol *labels    = NULL;   /* labels */
node *parse_tree_head = NULL;
char *program = NULL;

extern int num_lines;
extern char** source_lines;
extern int* visited;

void build_jni_lines(const char* str) {
	int start = 0;
	int curr = 0;
	char *line = NULL;	
	int done = 0;

	while (!done) {
			while (str[curr] != '\n' && str[curr] != '\0') {
				curr++;
				if (str[curr] == '\0') done = 1;
			}
			curr++;

			line = malloc(sizeof(char*)*(curr-start + 1));
			memcpy(line, str+start, curr-start); 
			line[curr-start] = '\0';

			num_lines++;
			source_lines = realloc(source_lines, sizeof(char*)*num_lines);
			source_lines[num_lines-1] = line;	
			start = curr;
	}
	visited = calloc(num_lines, sizeof(int));
}

JNIEXPORT jstring JNICALL Java_plptool_interfaces_PLPCC_nativeCompile
	(JNIEnv *env, jobject jobj, jint log_level, jstring input) {
	printf("PLPCC JNI: nativeCompile called.\n");
	jboolean iscopy;
	const char *str = (*env)->GetStringUTFChars(env, input, &iscopy);
	printf("%s\n", str);

	LOG_LEVEL = log_level;
	build_jni_lines(str);

	/* create an empty symbol table */
	sym = new_symbol_table(NULL);

	yy_scan_string(str);
	yyparse();
	handle(parse_tree_head);

	return (*env)->NewStringUTF(env, program);
}

JNIEXPORT jstring JNICALL Java_plptool_interfaces_PLPCC_getVersion
	(JNIEnv *env, jobject jobj) {

	printf("PLPCC JNI: Version 3.4\n");
	const char *str = "3.4";

	return (*env)->NewStringUTF(env, str);
}
