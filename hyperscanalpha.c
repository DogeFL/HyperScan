#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>

#define MASTER_NODE_RANK 0

void do_timer (int nNode, char *strEvent, bool bStart, bool bDisplayDuration,
         double *dStartTime, double *dEndTime, double *dTimeDuration);
///////////////////////// Free it all ////////////////////////////////////////
void freeMem (int filelen, char **lines);

/////////////// Function that will Create a 2d array from a file ////////////
char **createArray (FILE * logFile, char **lines, int *filelength);

///////////////// Function that will print a 2d array ///////////////////////
void printArray (char **lines, int filelength);

///////////////Function to check if attack going on ////////////////////////
char * scanarray (char **array, int length);

  char ** lines;

int
main (int argc, char *argv[])
{

  double dTimeDuration = 0, dStartTime = 0, dEndTime = 0;

  int numtasks, taskid, source, dest, nTagArrayOffset, nTagArrayData, offset = 0;

  //MUST BE NAMED LOG.CSV for now
  FILE *logFile = fopen ("log.csv", "r"); // Open our logfile in read mode. 
  int *filelength = malloc (sizeof (int));  // Allocate some memory for our file size

  // This function creates an array filled with the lines, and also will set the filelength pointer to the length of the file.
  //lines = createArray (logFile, lines, filelength); // We send an empty memory address, so function can fill it in.


  MPI_Status status;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank (MPI_COMM_WORLD, &taskid);
  int tag1, tag2, workperNode, workLeft;
  tag2 = 1;
  tag1 = 2;
  
      lines = createArray (logFile, lines, filelength); 

////////////////////Master Node logic/////////////////

  if (taskid == MASTER_NODE_RANK)
    {
       //lines = createArray (logFile, lines, filelength);
int l=0;
      printf ("Array has been initialized...");
      printf ("There are : %d Elements in the array \n", *filelength);
      printf ("I have %d Workers ready to work \n", numtasks);
 
      workperNode = (*filelength / numtasks);
      workLeft = (*filelength % numtasks);
      printf ("Each node will recieve %d elements to search \n", workperNode);
      printf ("With %d left over \n", workLeft);
      
      int i, start;

      offset = workperNode;

///////////////////SLAVE WORK///////////////////

      for (dest = 1; dest < numtasks; dest++)
  {
    printf ("offset : %d end: %d \n", offset, (workperNode+offset));

        MPI_Send (&workperNode, 1, MPI_INT, dest, 3, MPI_COMM_WORLD);
        MPI_Send (&offset, 1, MPI_INT, dest, 4, MPI_COMM_WORLD);
        MPI_Send(&(lines[offset][0]), workperNode, MPI_CHAR, dest, tag2,
                 MPI_COMM_WORLD);
        printf("Sent Process: %d array \n",dest);      
     offset = offset + workperNode ;
  }

//Call this below when done writing program... 
//freeMem(* filelength,lines); // Deallocate the memory
 

      //////////////////MASTER WORK///////////////////
      printf("MASTER NODE SCANNING:\n");

      offset = 0;
      for (i = 0; i < workperNode; i++)
  {
    if (scanarray (&lines[i], sizeof (lines[i])) != "not found")
      {
        printf ("%s \n", scanarray (&lines[i], sizeof (lines[i])));
      }

}
    }



///////////////////////////  SLAVE NODE LOGIC  //////////////////////////////////

  if (taskid > MASTER_NODE_RANK)
    {
      source = MASTER_NODE_RANK;
//This will send the buffer size, since the master created it.
      //Recieve the data from the master.
      MPI_Recv (&workperNode, 1, MPI_INT, source, 3, MPI_COMM_WORLD, &status);
      MPI_Recv (&offset, 1, MPI_INT, source, 4, MPI_COMM_WORLD, &status);
           printf ("PROCESS %d :offset : %d end: %d \n",taskid, offset, (workperNode+offset));
      MPI_Recv (&(lines[offset][0]), workperNode , MPI_CHAR, source, tag2,MPI_COMM_WORLD, &status);
           printf("PROCESS %d recieved the array........\n", taskid);

int i;
printf("PROCESS %d Scanning in progress.....\n", taskid);
 for (i = offset; i < (workperNode + offset); i++)
  {
    if (scanarray (&lines[i], sizeof (lines[i])) != "not found")
      {
        printf ("%s \n", scanarray (&lines[i], sizeof (lines[i])));
      }
  }

    }
  MPI_Finalize ();
}       // End main function



void
do_timer (int nNode, char *strEvent, bool bStart, bool bDisplayDuration,
    double *dStartTime, double *dEndTime, double *dTimeDuration)
{
  if (bStart == true)
    {
      // reset duration and end time
      *dTimeDuration = 0;
      *dEndTime = 0;

      // start timer
      *dStartTime = MPI_Wtime ();
    }
  else
    {
      // stop timer and compute duration
      *dEndTime = MPI_Wtime ();
      *dTimeDuration = *dEndTime - *dStartTime;

      // if showing duration, display time it took for event to compute
      if (bDisplayDuration == true)
  {
    // display time duration
    char strBuf[256];
    memset (strBuf, 0, 256);
    sprintf (strBuf, "Time duration for %s from node %d", strEvent,
       nNode);
    printf ("%-30s=%0.6lf (sec)\n", strBuf, *dTimeDuration);
  }
    }
}

char **
createArray (FILE * logFile, char **lines, int *filelength)
{       // Function to get the number of lines, and put them into an array.

  char buff[300];   // Create a new buffer
  // This will create pointer to a pointer
  int filelen = 0, linelen, maxlinelen = 0;

  while (1)
    {
      if (feof (logFile))
  {     //If the end has been reached, we break out of the loop.
    break;
  }
      else
  {
    filelen++;
    fgets (buff, 300, (FILE *) logFile);  // Consumes until 255 chars or end of line.
    linelen = strlen (buff);
    if (linelen > maxlinelen)
      {
        maxlinelen = linelen;
      }
  }
    }

  lines = malloc (sizeof (char *) * filelen); // Allocate the entire length of the file in memory.
  fseek (logFile, 0, SEEK_SET); // Move file pointer back to beginning of file.

  int i;
  for (i = 0; i < filelen; i++)
    {
      lines[i] = malloc (sizeof (char *) * maxlinelen); //Allocate each line with the maximum line size, to ensure no data gets removed.
      fgets (lines[i], 300, (FILE *) logFile);  //Read from that array that was just created.
    }

  fclose (logFile);

  *filelength = filelen;
  return lines;
}

void
freeMem (int filelen, char **lines)
{       // Function to free up the memory. Only call when all is done.
  int i = 0;
  for (i = 0; i < filelen; i++)
    {
      free (lines[i]);
    }
  free (lines);

}

void
printArray (char **lines, int filelength)
{
  int i = 0;
  for (i = 0; i < filelength; i++)
    {
      printf ("%s\n", lines[i]);
    }

}


char * scanarray (char **array, int length)
{
  int i;
  char *searcher;   // this is what searches for the strings
  searcher = strstr (array[0], "1.1.1.1.1");
  if (searcher != NULL)
    {       // This means something is found....
      return array[0];
    }
  {
    return "not found";
  }

}

