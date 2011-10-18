/* fibonacci sequence */

int depth = 10;

int fib(int fn1, int fn2) {
	int fn0 = fn1 + fn2;
	if (depth == 0)
		return fn0;
	else
		return fib(fn0, fn1);
}

void main(void) {
	fib(0,1);
}	
