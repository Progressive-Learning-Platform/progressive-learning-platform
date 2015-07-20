int result0 = 0;
int result1 = 0;
int result2 = 0;
int result3 = 0;
int result4 = 0;

struct a
{
	int x;
	char y;
};

union b
{
	int a;
	char b;
};

int main(void) {
	struct a d;
	union b e;

	d.x = 42;
	d.y = 'f';

	e.a = 0x706968;
	result2 = e->a;
	e.b = 'e';
	result3 = e->b;
	result4 = e->a;
	

	result0 = d->x;
	result1 = d->y;
	return 0;
}
