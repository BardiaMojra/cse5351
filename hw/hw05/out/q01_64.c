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
  MPI_Init(&argc, &argv);
  /* Starts MPI */
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  double t1, t2;

  if(rank == 0) {
    t1 = MPI_Wtime();
    strcpy(msg, MSG);
    for(i=0; i<size; i++) {
      MPI_Isend(msg, MSG_LEN, MPI_CHAR, i, 0, MPI_COMM_WORLD, &request);
    }
  }
  /* Receive the msg with nonblocking receive */
  MPI_Irecv(msg, MSG_LEN, MPI_CHAR, 0, 0, MPI_COMM_WORLD,&request);

  /* Show the msg and number of loops */
  MPI_Ibarrier(MPI_COMM_WORLD, &request);
  if(rank == 0) {
    t2 = MPI_Wtime();
    printf("[Proc %d/%d] time elapsed: %f\n",rank,size,t2-t1);
    fflush(stdout);
  }
  MPI_Finalize();
  return 0;
}
