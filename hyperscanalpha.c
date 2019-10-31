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

int scanarray(char ** array, int length){
//logic goes here, just a test for now
int i;
printf("Scanning array now... ");
char * searcher;

for (i=0; i<length;i++){
searcher = strstr(array[i], "allalal"); 
if (searcher!= NULL){return 1;}

}
return 0;

}


int main(int argc, char *argv[]) {

  double dTimeDuration=0, dStartTime=0, dEndTime=0;

  int numtasks, taskid, source, dest, nTagArrayOffset, nTagArrayData, offset =0;
  char ** lines;
  //MUST BE NAMED LOG.CSV for now
  FILE * logFile = fopen("log.csv", "r"); // Open our logfile in read mode. 
  int * filelength = malloc(sizeof(int)); // Allocate some memory for our file size

  // This function creates an array filled with the lines, and also will set the filelength pointer to the length of the file.
  

MPI_Status status;

MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
int tag1,tag2, workperNode, workLeft;
tag2 = 1;
tag1 = 2;



if (taskid == MASTER_NODE_RANK){          //Master Node logic.
  printf("Array has been initialized...");
  lines = createArray(logFile, lines, filelength); // We send an empty memory address, so function can fill it in.
  printf("There are : %d Elements in the array \n", * filelength);
  printf("I have %d Workers ready to work \n" , numtasks);

  //the first step will be to chop the array into pieces each array can handle.
  //Debugging
  printf("lines mem 1: %p \n", &(lines[0][0]));
  printf("lines mem 2: %p \n", &(lines[1][0]));
  printf("lines mem 3: %p \n", &(lines[2][0]));

   

   workperNode = (*filelength / numtasks );
   workLeft = (*filelength % numtasks );
    printf("Each node will recieve %d elements to search \n" , workperNode);
    printf("With %d left over \n" , workLeft);
   int i,start,end;

offset = 0;
  for (dest=1; dest<numtasks; dest++) {
    MPI_Send(&workperNode, 1, MPI_INT, dest, 3, MPI_COMM_WORLD);
    MPI_Send(&offset, 1, MPI_CHAR, dest, tag1, MPI_COMM_WORLD);
    end = workperNode;

    for (i=offset;i<end;i++){
    MPI_Send(&(lines[i][0]), workperNode+1, MPI_CHAR, dest, tag2, MPI_COMM_WORLD);
    //printf("Sent %d elements to task %d offset= %d\n",workperNode,dest,offset);
   }
     offset = offset + workperNode;
    end = end + workperNode;
 }
    


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
  source = MASTER_NODE_RANK;
   //This will send the buffer size, since the master created it.
    MPI_Recv(&workperNode, 1, MPI_INT, source, 3, MPI_COMM_WORLD, &status);
//MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
  char * buf;
  buf = malloc(workperNode+1 * sizeof(char));
  int i;

 char * temparr[workperNode];
for (i=0;i<workperNode;i++){
  MPI_Recv(buf, workperNode+1, MPI_CHAR, source, tag2, MPI_COMM_WORLD, &status);
  temparr[i] = buf;
}

printf("%s",temparr[1]); // now we have a nice array of pointers, where we can now create func

printf("%d",scanarray(temparr,workperNode));

}
//printArray(lines, * filelength);

MPI_Finalize();

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


