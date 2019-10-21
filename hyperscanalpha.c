
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {

	FILE *f = fopen("log.csv", "r");
	char buff[255];
	char **lines;
	int filelen = 0, linelen, maxlinelen = 0;

	//fseek(f, 0, SEEK_END);
	//filelen = ftell(f);
	//fseek(f, 0, SEEK_SET);

	while (1) {
		if (feof(f))
			break;			
		filelen++;
		fgets(buff, 255, (FILE*)f); // Consumes until 255 chars or end of line.
		linelen = strlen(buff);
		if (linelen > maxlinelen)
			maxlinelen = linelen;	
	}

	lines = malloc(sizeof(char *) * filelen);
	fseek(f, 0, SEEK_SET); // Move file pointer back to beginning of file.
	int i;
	for (i = 0; i < filelen; i++) {
		lines[i] = malloc(sizeof(char *) * maxlinelen);
		fgets(lines[i], 255, (FILE*)f);
		printf("%s\n", lines[i]);
	}
	
	printf("Length: %d\n", filelen);
	printf("Max Line Length: %d\n", maxlinelen);

	//fgets(buff, 255, (FILE*)f);
	//printf("%s\n", buff);

	fclose(f);

	for (i = 0; i < filelen; i++) {
		free(lines[i]);
	}
	free(lines);

	return 0;
}
