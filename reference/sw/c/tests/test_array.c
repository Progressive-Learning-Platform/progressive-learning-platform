/* tests the use of arrays */

int main(void) {
	int a[4];
	int b[3] = {0,5,10};
	int i;
	
	for (i=0; i<3; i++) {
		a[i+1] = b[i];
	}

	return 0;
}
