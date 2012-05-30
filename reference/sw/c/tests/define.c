#define P
#define REPLACE result = 1;

#ifdef P
int result = 0;
#endif

#ifndef F
int main(void) {
	REPLACE
	return 0;
}
#endif
