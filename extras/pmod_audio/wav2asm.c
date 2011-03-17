/* 
* fritz
* 2011.3.11
* extracts the PCM data of a PCM encapsulated WAV file and outputs a 
* PLPTool compatible .asm file.
*
* usage: wav2asm <infile> <outfile>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct wav_header {
	char ckID[4];
	uint32_t cksize;
	char WAVEID[4];
};

struct chunk_fmt {
	char ckID[4];
	uint32_t cksize;
	uint16_t wFormatTag;
	uint16_t nChannels;
	uint32_t nSamplesPerSec;	
	uint32_t nAvgBytesPerSec;
	uint16_t nBlockAlign;
	uint16_t wBitsPerSample;
};

struct chunk_data {
	char ckID[4];
	uint32_t cksize;
};

int main(int argc, char *argv[]) {
	FILE* infile = NULL;
	FILE* outfile = NULL;
	char riff[4] = {'R','I','F','F'};
	char fmt [4] = {'f','m','t',' '};
	char data[4] = {'d','a','t','a'};
	struct wav_header* wh = malloc(sizeof(struct wav_header));
	struct chunk_fmt* cf  = malloc(sizeof(struct chunk_fmt));
	struct chunk_data* cd = malloc(sizeof(struct chunk_data));
	int ret = 0;
	unsigned char *d = NULL;
	int i;

	printf("[i] wav2asm - fritz\n");
	printf("[i] infile  : %s\n",argv[1]);
	printf("[i] outfile : %s\n",argv[2]);
	
	infile = fopen(argv[1], "r");
	outfile = fopen(argv[2], "w");
	if (infile == NULL) {
		printf("[e] couldn't open %s\n",argv[1]);
		exit(-1);
	}
	if (outfile == NULL) {
		printf("[e] couldn't open %s\n",argv[2]);
		exit(-1);
	}

	/* get the header */
	fread(wh, sizeof(struct wav_header), 1, infile);
	ret = memcmp(riff,wh->ckID,4);
	if (ret == 0) {
		printf("[i] found RIFF header. congrats.\n");
		printf("[i] chunk size is %d\n",wh->cksize);
	} else {
		printf("[e] invalid header!\n");
		exit(-1);
	}

	/* get the format chunk */
	fread(cf, sizeof(struct chunk_fmt), 1, infile);
	ret = memcmp(fmt, cf->ckID,4);
	if (ret == 0) {
		printf("[i] found format chunk\n");
		if (cf->wFormatTag != 0x0001) {
			printf("[e] not pcm encoded data!\n");
			exit(-1);
		}
		printf("[i] channels           : %d\n",cf->nChannels);
		printf("[i] samples per second : %d\n",cf->nSamplesPerSec);
		printf("[i] bits per sample    : %d\n",cf->wBitsPerSample);
	} else {
		printf("[e] invalid format chunk\n");
		exit(-1);
	} 
			
	/* find the data chunk */
	fread(cd, sizeof(struct chunk_data), 1, infile);
	ret = memcmp(data, cd->ckID,4);
	while (ret != 0 && !feof(infile)) {
		printf("[i] found chunk : %c%c%c%c.\n",cd->ckID[0],cd->ckID[1],cd->ckID[2],cd->ckID[3]);
		fseek(infile, cd->cksize, SEEK_CUR);
		fread(cd, sizeof(struct chunk_data), 1, infile);
		ret = memcmp(data, cd->ckID,4);
	}

	if (ret == 0) {
		printf("[i] found data chunk\n");
		printf("[i] data chunk size is %d\n", cd->cksize);
	} else {
		printf("[e] did not find data chunk!\n");
		exit(-1);
	}
	
	printf("[i] loading data chunk\n");
	d = malloc(cd->cksize);
	fread(d, cd->cksize, 1, infile);

	printf("[i] outputting asm data\n");
	fprintf(outfile, "audio_data:\n");
	for (i=0; i<cd->cksize; i=i+2)
		fprintf(outfile, ".word 0x%02x%02x%02x%02x\n", d[i],0,d[i+1],0);
	
	printf("[i] done.\n");

	fclose(infile);
	fclose(outfile);
	
	return 0;	
}
