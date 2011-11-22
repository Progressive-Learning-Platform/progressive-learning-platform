/* tests the use of arrays */

int d[3];
int e[] = {0,5,-1};

int main(void) {
	int a[4];
	int b[] = {0,5,10};
	int i;
	int c[4][2];
	
	for (i=0; i<3; i++) {
		a[i+1] = b[i];
	}

	return 0;
}
