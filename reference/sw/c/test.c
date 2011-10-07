typedef struct fritz_t {
	int x;
	int y;
} fritz;

int main(void) {
	enum {
		david,
		fritz
	} stuff;

	int *g;

	auto register int a;
	const int b = 5;
	char *q = "boo";
	volatile int c = a + b;
	int d,e;
	a = 22;

	return 0;
		
}
