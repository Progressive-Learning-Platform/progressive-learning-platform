#include "parser.tab.h"
#include "log.h"

int main() {
	LOG_LEVEL=1;
	log("nothing to see here...");

	yyparse();
	return 0;
}
