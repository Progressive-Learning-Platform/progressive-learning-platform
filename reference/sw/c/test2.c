/* program to write something to the leds */
void main(void) {
	int a = 0;
	void *leds = 0xf0200000;
	while (a < 10) {
		*leds = a;
		a++;
	}
}
