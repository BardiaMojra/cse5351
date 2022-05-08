#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
/* #define MSG "This sentence has a length of 64 characters, xxxxxxxxxxxxxxxx." */
#define MSG_LEN 128

int main(int argc, char *argv[])
{
  int rank, size;
  MPI_Request request;
  MPI_Status status;

  /* init */
  MPI_Init(&argc, &argv);
  char msg[MSG_LEN];
  int loop = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  char sendbuf[size * MSG_LEN];
  char recvbuf[size * MSG_LEN];
  memset(sendbuf,0,sizeof(sendbuf));
  memset(recvbuf,0,sizeof(recvbuf));
  double t1, t2;

  if (rank == 0) {
    t1 = MPI_Wtime();
  }

  /* load send buffer */
  for (int i=0; i<size; i++) {
    sprintf(msg, "message from (%d) to (%d). This message is %d char long\
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx.",\
    rank, i, MSG_LEN);
    // printf(msg);
    char *ptr =  sendbuf+(i*MSG_LEN);
    strncpy(ptr, msg, MSG_LEN-1);
    sendbuf[(i*MSG_LEN)-1] = '\0';
    printf("(%d) sendbuf[%d]: %s\n", rank, i, ptr);
  }

  /* just call the nonblocking version and wait on it */
  MPI_Ialltoall(sendbuf, MSG_LEN, MPI_CHAR, recvbuf, MSG_LEN,\
    MPI_CHAR, MPI_COMM_WORLD, &request);

  MPI_Wait(&request, &status);
  while (rank == 0) {
    loop += 1;
    if (request != 0) {
      t2 = MPI_Wtime();
      printf( "Elapsed time is %f\n", t2 - t1 );
      fflush(stdout);
      break;
    }
  }
  MPI_Finalize();
  return 0;
}
