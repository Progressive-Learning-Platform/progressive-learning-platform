int result0 = 0;
int result1 = 0;

struct a {
	int x;
	char y;
};

int main(void) {
	struct a d;

	d.x = 42;
	d.y = 'f';

	result0 = d->x;
	result1 = d->y;
	return 0;
}
