/* Simple PSHV Encoder by Rinnegatamante */
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 536870912

unsigned long audiosize;
unsigned long videosize;

void bufferedWrite(FILE* in, FILE* out, unsigned long size){
	char* buffer = (char*)malloc(BUFFER_SIZE);
	int i = 0;
	unsigned long len;
	while (i < size){
		len = ((i + BUFFER_SIZE) > size) ? (size - i) : BUFFER_SIZE;
		fread(buffer, 1, len, in);
		fwrite(buffer, 1, len, out);
		i += len;
	}
	free(buffer);
}

int main(int argc,char** argv){
	if (argc < 4) {
		fprintf(stderr, "%s <framerate> <audiofile> <videofile> [<outfile>]\n", argv[0]);
		return -1;
	}

	// Getting framerate from argv
	float framerate = atof(argv[1]);
	
	// Opening output file
	FILE* output = stdout;
	if (argc > 4) {
		output = fopen(argv[4], "wb");
		if (output == NULL) {
			fprintf(stderr, "Error opening \"%s\": %s\n", argv[4], strerror(errno));
			return -1;
		}
	}

	fwrite("PSHV",1,4,output);
	fwrite(&framerate, sizeof(float), 1, output);
	
	// Getting audio size
	fprintf(stderr, "Writing audio track...\n");
	FILE* input_audio = fopen(argv[2], "rb");
	if (input_audio == NULL) {
		fprintf(stderr, "Error opening \"%s\": %s\n", argv[2], strerror(errno));
		return -1;
	}
	unsigned long read_start = 0;
	fseek(input_audio, 0, SEEK_END);
	unsigned long size = (unsigned long)ftell(input_audio);
	audiosize = size - read_start;
	fwrite(&audiosize, sizeof(unsigned long), 1, output);
	fprintf(stderr, "Audio size: %lu bytes\n", audiosize);
	
	// Writing audio data
	fseek(input_audio, read_start, SEEK_SET);
	bufferedWrite(input_audio, output, audiosize);
	fclose(input_audio);
	
	// Getting video size
	fprintf(stderr, "Opening video track...\n");
	FILE* input_video = fopen(argv[3], "rb");
	if (input_video == NULL) {
		fprintf(stderr, "Error opening \"%s\": %s\n", argv[3], strerror(errno));
		return -1;
	}
	read_start = 0;
	fseek(input_video, 0, SEEK_END);
	size = (unsigned long)ftell(input_video);
	videosize = size - read_start;
	fprintf(stderr, "Video size: %lu bytes\n", videosize);
	
	// Writing video data
	fseek(input_video, read_start, SEEK_SET);
	bufferedWrite(input_video, output, videosize);
	fclose(input_video);
	
	// Flushing output
	fclose(output);
	fprintf(stderr, "Done!\n", size);
	
	return 0;
	
}