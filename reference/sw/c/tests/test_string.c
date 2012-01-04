/* test for string literals */

int main(void) {
	char *a = "hello world";
	char *c = "wira is a jerk\n";
	char *b = a;
	a = c;
	return 0;
}
