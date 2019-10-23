#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>

#define MASTER_NODE_RANK 0



void do_timer(int nNode, char *strEvent, bool bStart, bool bDisplayDuration, double *dStartTime, double *dEndTime, double *dTimeDuration);

void freeMem(int filelen, char ** lines);

char ** createArray(FILE * logFile, char ** lines, int * filelength);

void printArray(char ** lines, int filelength);


int main(int argc, char *argv[]) {

  double dTimeDuration=0, dStartTime=0, dEndTime=0;

  int numtasks, taskid, source, dest, nTagArrayOffset, nTagArrayData;
  char ** lines;
  FILE * logFile = fopen("log.csv", "r"); // Open our logfile in read mode. 
  int * filelength = malloc(sizeof(int)); // Allocate some memory for our file size

  // This function creates an array filled with the lines, and also will set the filelength pointer to the length of the file.
  

MPI_Status status;

MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

if (taskid == MASTER_NODE_RANK){          //Master Node logic.
	lines = createArray(logFile, lines, filelength); // We send an empty memory address, so function can fill it in.
	printf("There are : %d Elements in the array \n", * filelength);
	printf("I have %d Workers ready to work \n" , numtasks);



 	//the first step will be to chop the array into pieces each array can handle.

	int workperNode = (*filelength / numtasks );
	int workLeft = (*filelength % numtasks );
		printf("Each node will recieve %d elements to search \n" , workperNode);
		printf("With %d left over \n" , workLeft);

/*
int i=0;
	for (i = 1; i < numtasks; i++) 
{
MPI_Send(lines[i], sizeof(greeting), MPI_CHAR,i, 1, MPI_COMM_WORLD, &status);
printf("%s\n", greeting);
}
*/

//Call this after everything is done.
//freeMem(* filelength,lines); // Deallocate the memory

}

if (taskid > MASTER_NODE_RANK){           // Slave nodes

}


MPI_Finalize();


//printArray(lines, * filelength);


return 0;
} // End main function



void do_timer(int nNode, char *strEvent, bool bStart, bool bDisplayDuration, double *dStartTime, double *dEndTime, double *dTimeDuration)
{
    if (bStart == true)
    {
        // reset duration and end time
        *dTimeDuration = 0;
        *dEndTime = 0;
        
        // start timer
        *dStartTime = MPI_Wtime();
    }
    else
    {
        // stop timer and compute duration
        *dEndTime = MPI_Wtime();
        *dTimeDuration = *dEndTime - *dStartTime;
        
        // if showing duration, display time it took for event to compute
        if (bDisplayDuration == true)
        {
            // display time duration
            char strBuf[256];
            memset(strBuf, 0, 256);
            sprintf(strBuf, "Time duration for %s from node %d", strEvent, nNode);
            printf("%-30s=%0.6lf (sec)\n", strBuf, *dTimeDuration);
        }
    }
}

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


