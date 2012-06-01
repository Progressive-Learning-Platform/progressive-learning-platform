int result0 = 0;
int result1 = 0;
int result2 = 0;

struct a {
	int b;
	int c;
	char d;
};

int main(void) {
	struct e;
	e->b = 1;
	e->c = 2;
	e->d = 'a';
	result0 = e->b;
	result1 = e->c;
	result2 = e->d;
	return 0;
}
