#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER_NODE_RANK 0

char ** createArray(FILE * logFile, char ** lines, int * filelength) { // Function to get the number of lines, and put them into an array.

  char buff[300]; // Create a new buffer
  // This will create pointer to a pointer
  int filelen = 0, linelen, maxlinelen = 0;

  while (1) {
    if (feof(logFile)) { //If the end has been reached, we break out of the loop.
      break;
    } else {
      filelen++;
      fgets(buff, 300, (FILE * ) logFile); // Consumes until 255 chars or end of line.
      linelen = strlen(buff);
      if (linelen > maxlinelen) {
        maxlinelen = linelen;
      }
    }
  }

  lines = malloc(sizeof(char * ) * filelen); // Allocate the entire length of the file in memory.
  fseek(logFile, 0, SEEK_SET); // Move file pointer back to beginning of file.

  int i;
  for (i = 0; i < filelen; i++) {
    lines[i] = malloc(sizeof(char * ) * maxlinelen); //Allocate each line with the maximum line size, to ensure no data gets removed.
    fgets(lines[i], 300, (FILE * ) logFile); //Read from that array that was just created.
  }

  fclose(logFile);

  * filelength = filelen;
  return lines;
}

void freeMem(int filelen, char ** lines) { // Function to free up the memory. Only call when all is done.
  int i = 0;
  for (i = 0; i < filelen; i++) {
    free(lines[i]);
  }
  free(lines);

}

void printArray(char ** lines, int filelength) {
  int i = 0;
  for (i = 0; i < filelength; i++) {
    printf("%s\n", lines[i]);
  }

}

int main(int argc, char *argv[]) {
  int numtasks, taskid, source, dest, nTagArrayOffset, nTagArrayData;
  char ** lines;
  FILE * logFile = fopen("log.csv", "r"); // Open our logfile in read mode. 
  int * filelength = malloc(sizeof(int)); // Allocate some memory for our file size

  // This function creates an array filled with the lines, and also will set the filelength pointer to the length of the file.
  
MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
if (taskid == MASTER_NODE_RANK){          //Master Node logic.
	lines = createArray(logFile, lines, filelength);
	printf("Length: %d", * filelength);
}

MPI_Finalize();


printArray(lines, * filelength);

printf("%d WORLD RANK",taskid);

  freeMem(* filelength,lines);
}
