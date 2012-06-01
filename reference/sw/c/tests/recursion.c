/* fibonacci sequence */

/* tests: 
	global variables
	return statements
	function calls
	recursion
*/

int depth = 10;
int result = 0;

int fib(int fn1, int fn2) {
	int fn0 = fn1 + fn2;
	depth--;
	if (depth == 0)
		return fn0;
	else
		return fib(fn0, fn1);
}

void main(void) {
	result = fib(1,0);
}	
