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

symbol_table *sym = NULL;   /* scoped symbol tables */
symbol *constants = NULL;   /* constants */
symbol *labels    = NULL;   /* labels */
node *parse_tree_head = NULL;
char *program = NULL;

JNIEXPORT jstring JNICALL Java_plptool_interfaces_PLPCC_nativeCompile
	(JNIEnv *env, jobject jobj, jstring input) {
	printf("PLPCC JNI: nativeCompile called.\n");
	jboolean iscopy;

	const char *str = (*env)->GetStringUTFChars(env, input, &iscopy);

	yy_scan_string(str);
	yyparse();
	handle(parse_tree_head);

	return (*env)->NewStringUTF(env, program);
}

JNIEXPORT jint JNICALL Java_plptool_interfaces_PLPCC_testLink
	(JNIEnv *env, jobject jobj) {

	printf("PLPCC JNI: Test link.\n");

	return NULL;
}
