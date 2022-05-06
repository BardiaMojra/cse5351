#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* config */
#define MSG "This sentence has a length of 64 characters, xxxxxxxxxxxxxxxx."
#define MSG_LEN 64

int main(int argc, char *argv[])
{
  int rank, size;
  char msg[MSG_LEN];
  int i;
  MPI_Request request;
  int flag;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  /* Starts MPI */
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /** All processors send message to all processors with nonblocking send;
   * note that there are 2-second pauses between 2 consecutive send
   * operations */



  char **buff_array;

  buff_array = malloc(size * sizeof(char*));
  for (int i=0; i<size; i++)
  {
    buff_array[i] = malloc((MSG_LEN+1) * sizeof(char));
  }

  MPI_Alltoall(&MSG, 1, MPI_INT, buff_array, 1, MPI_INT, MPI_COMM_WORLD);


  /* Loop to check whether receive is completed */
  for (i=0; ;i++)
  {
    MPI_Test(&request, &flag, &status);
    if(flag != 0)
    /* receive completed */
    break;
  }
  /* Show the msg and number of loops */
  MPI_Ibarrier(MPI_COMM_WORLD, &request);
  if(rank == size-1) // just for a cleaner read on terminal, i.e. 100 samples
  {
    printf("[Proc %d/%d] msg=\"%s\", \nloop= %d\n",rank,size,msg,i);
  }


  for (int i=0; i<size; i++)
  {
    printf("[Proc %d/%d] msg=\"%s\", \nloop= %d\n",rank,size,msg,i);

  }

  for (int i = 0; i < size; i++)
  {
    free(buff_array[i]);
  }
  free(buff_array);



  char *test = (char*) malloc(12*sizeof(char));
  strcpy(test, "testingonly");


https://mpitutorial.com/tutorials/mpi-broadcast-and-collective-communication/




  if(rank == 0)
  {
    strcpy(msg, MSG);
    for(i=0; i<size; i++)
    {
      sleep(2);
      MPI_Isend(msg, MSG_LEN, MPI_CHAR, i, 0, MPI_COMM_WORLD, &request);
    }
  }
  /* Receive the msg with nonblocking receive */
  MPI_Irecv(msg, MSG_LEN, MPI_CHAR, 0, 0, MPI_COMM_WORLD,&request);
  /* Loop to check whether receive is completed */
  for (i=0; ;i++)
  {
    MPI_Test(&request, &flag, &status);
    if(flag != 0)
    /* receive completed */
    break;
  }
  /* Show the msg and number of loops */
  // MPI_Ibarrier(MPI_COMM_WORLD, &request);




  MPI_Finalize();
  return 0;
}
