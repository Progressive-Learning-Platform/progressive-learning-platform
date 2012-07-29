int result0 = 0;
int result1 = 0;
int result2 = 0;
int result3 = 0;

struct a {
	int x;
	char y;
};

typedef struct b_t {
	struct b_t *next;
	int z;
} b;

int main(void) {
	struct a d;
	b e;

	d.x = 42;
	d.y = 'f';

	e.next = 0xdeadbeef;
	e.z = d.x+1;

	result0 = d->x;
	result1 = d->y;
	result2 = e->next;
	result3 = e->z;
	return 0;
}
