/* tests the use of arrays */

int result0 = 0;
int result1 = 0;
int result2 = 0;

int d[3];
int e[] = {0,5,-1};

int main(void) {
	int a[4];
	int b[] = {1,2,3};
	int i;
	int c[4][2];
	
	for (i=0; i<3; i++) {
		a[i+1] = b[i] + e[i];
	}

	result0 = a[1];
	result1 = a[2];
	result2 = a[3];

	return 0;
}
