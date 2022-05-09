#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
/* #define MSG "This sentence has a length of 64 characters, xxxxxxxxxxxxxxxx." */
#define MSG_LEN 64
#define ROOT 0

int main(int argc, char *argv[])
{
  int rank, size;

  /* init */
  MPI_Init(&argc, &argv);
  char msg[MSG_LEN];
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  char sbuf[size * MSG_LEN];
  char rbuf[size * MSG_LEN];
  memset(sbuf,0,sizeof(sbuf));
  memset(rbuf,0,sizeof(rbuf));
  double t1, t2;

  if (rank == ROOT) {
    t1 = MPI_Wtime();
  }

  /* load send buffer */
  for (int i=0; i<size; i++) {
    sprintf(msg,"[%d --> %d]: This message is about %d char long, xxxxxxxxxx.",\
      rank, i, MSG_LEN);
    char *sb_ptr =  sbuf+(i*MSG_LEN);
    strncpy(sb_ptr, msg, MSG_LEN-1);
    sbuf[(i*MSG_LEN)-1] = '\0';
    printf("[%d/%d] sbuf[%d]: %s\n", rank, size, i, sb_ptr);

    char *rb_ptr =  rbuf+(i*MSG_LEN);
    rbuf[(i*MSG_LEN)-1] = '\0';
    printf("[%d/%d] rbuf[%d]: %s\n", rank,size, i, rb_ptr);
    fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Igather(sbuf, MSG_LEN, MPI_CHAR, rbuf, MSG_LEN, MPI_CHAR, ROOT,\
    MPI_COMM_WORLD, &request);

  MPI_Wait(&request, &status);
  while (rank == ROOT) {
    if (request != 0) {
      t2 = MPI_Wtime();
      printf( "Elapsed time:     %f\n", t2-t1);
      printf("Root rbuf:\n");
      for (int i=0; i<size; i++) {
        char *rb_ptr =  rbuf+(i*MSG_LEN);
        printf("[%d/%d] rbuf[%d]: %s\n", rank, size, i, rb_ptr);
        fflush(stdout);
      }
      break;
    }
  }
  MPI_Finalize();
  return 0;
}
