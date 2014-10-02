/* test for string literals */

int result = 0;

int main(void) {
	char *a = "hello world";
	char *c = "wira is a jerk\n";
	char *b = a;
	a = c;
	result = a[3];
	return 0;
}
