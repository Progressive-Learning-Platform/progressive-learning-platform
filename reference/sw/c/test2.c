/* program to write something to the leds */
void main(void a, int leds) {
	int a = 0;
	void *leds = 0xf0200000;
	while (1) {
		*leds = a;
		a++;
	}
}
