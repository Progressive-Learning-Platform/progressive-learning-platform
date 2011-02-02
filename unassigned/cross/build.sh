flex -s lexer.lex
bison -d parser.y
gcc main.c parser.tab.c handlers.c -lfl
