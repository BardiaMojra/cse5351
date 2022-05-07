#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
/* #define MSG "This sentence has a length of 64 characters, xxxxxxxxxxxxxxxx." */
#define MSG_LEN 64

int main(int argc, char *argv[])
{
  int rank, size;
  MPI_Request request = MPI_REQUEST_NULL;
  int status, gstatus;

  MPI_Init(&argc, &argv);
  char msg[MSG_LEN];
  int i, loop;
  /* Starts MPI */
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /** All processors send message to all processors with nonblocking send;
   * note that there are 2-second pauses between 2 consecutive send
   * operations */


  char *send_buff;
  char *recv_buff;


  send_buff = malloc(size * sizeof(char) * MSG_LEN);
  recv_buff = malloc(size * sizeof(char) * MSG_LEN);

  for (int i=0; i<size; i++)
  {
    sprintf(msg, "message from (%d) to (%d). This message is %d char long.\n\0",\
    rank, i, MSG_LEN);
    printf("%s", msg);
    strcpy(msg, send_buff[i * sizeof(char) * MSG_LEN]);
    printf("recv_buff[i]: %s\n", recv_buff[i * sizeof(char) * MSG_LEN]);
    sprintf(msg, "                                                       \n\0",\
    rank, i, MSG_LEN);
    printf("%s", msg);
    strcpy(msg, recv_buff[i * sizeof(char) * MSG_LEN]);

    sleep(2);
    MPI_Wait(&request, MPI_STATUS_IGNORE);
    MPI_Iallreduce(send_buff, recv_buff, 1, MPI_CHAR, MPI_SUM, MPI_COMM_WORLD, &request);
  }

  MPI_Wait(&request, MPI_STATUS_IGNORE);

  while (rank == 0) {
    loop += 1;
    if (request != 0) {
      printf("all_to_all returned %d\n",status);
      fflush(stdout);
      break;
    }
  }

  /* print messages */
  for (int i=0; i<size; i++)
    printf("[Proc %d/%d] msg=\"%s\" \n",rank,size,send_buff[i * sizeof(char) * MSG_LEN]);

  if(rank == 0) // just for a cleaner read on terminal, i.e. 100 samples
    printf("[[Proc %d/%d]], loop= %d\n",rank,size,loop);

  free(recv_buff);
  free(send_buff);

  MPI_Finalize();
  return 0;
}
