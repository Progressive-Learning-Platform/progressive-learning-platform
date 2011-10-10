#include <stdio.h>
#include <stdlib.h>

/* we're in hacky mode, so assume everything is sane */

int main(int argc, char *argv[]) {
	FILE* inFile = fopen(argv[1], "r");
	FILE* outFile = fopen(argv[2], "w");
	
	char *line = (char*)malloc(256);
	size_t len = 255;
	int ret = -1;
	int i=0;

	/* skip the top four lines */
	ret = getline(&line, &len, inFile);
	ret = getline(&line, &len, inFile);
	ret = getline(&line, &len, inFile);
	ret = getline(&line, &len, inFile);
	
	/* pixels come in 3's */
	for (i=0; i<640*480; i+=4) {
		int j=0;
		unsigned int word = 0;
		for (j=0; j<4; j++) {
			unsigned char r,g,b,pixel=0;
			/* red */
			ret = getline(&line, &len, inFile);
			r = atoi(line);
		
			/* green */
			ret = getline(&line, &len, inFile);
			g = atoi(line);
		
			/* blue */
			ret = getline(&line, &len, inFile);
			b = atoi(line);

			pixel |= (r & 0xe0);
			pixel |= (g & 0xe0) >> 3;
			pixel |= (b & 0xc0) >> 6;
			word  |= pixel << 8*j;
		}
		fprintf(outFile,".word 0x%x\n",word);
	}

	fclose(inFile);
	fclose(outFile);
	return 0;	
}
