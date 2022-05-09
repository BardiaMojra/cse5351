#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
#define ROOT 0
#define ARR_LEN 32

int main(int argc, char *argv[])
{
  /* init */
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int arr[ARR_LEN];
  int sbuf[size * ARR_LEN];
  int rbuf[size * ARR_LEN];
  memset(sbuf,0,sizeof(sbuf));
  memset(rbuf,0,sizeof(rbuf));
  double t1, t2;

  if (rank == ROOT) {
    t1 = MPI_Wtime();
  }

  /* load buffers */
  for (int a=0; a<size; a++) {
    for (int i=0; i<size; i++) {
      if (a==rank) {
        /* construct msg array */
        printf("    msg[%2d > %2d]: ", rank, i);
        for (int j=0; j<ARR_LEN; j++) {
          arr[j] = ((i*ARR_LEN)+j);
          printf("%3d ", arr[j]);
        }
        printf("\n");
        /* load msg array to send buffer */
        memcpy(sbuf, arr, ARR_LEN*sizeof(int));
        printf("[%2d/%2d] sbuf[%2d]: ", rank, size, i);
        for (int j=0; j<ARR_LEN; j++) {
          printf("%3d ", sbuf[(i*ARR_LEN)+j]);
        }
        printf("\n");
        printf("[%2d/%2d] rbuf[%2d]: ", rank, size, i);
        for (int j=0; j<ARR_LEN; j++) {
          printf("%3d ", rbuf[(i*ARR_LEN)+j]);
        }
        printf("\n");
      }
    }
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Scatter(sbuf, ARR_LEN, MPI_INT, rbuf, ARR_LEN, MPI_INT, ROOT,\
    MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);

  for (int a=0; a<size; a++) {
    if ((rank == ROOT) && (a == rank)) {
      t2 = MPI_Wtime();
      printf( "Elapsed time:     %f\n", t2-t1);
    }
    if (a == rank)
    {
      printf( "receive buffers: [%d] \n", rank);
      for (int i=0; i<size; i++) {
        printf("[%2d/%2d] rbuf[%2d]: ", rank, size, i);
        for (int j=0; j<ARR_LEN; j++) {
          printf("%3d ", rbuf[(i*ARR_LEN)+j]);
        }
        printf("\n");
      }
      printf( "send buffers: [%d] \n", rank);
      /* print send buffers */
      for (int i=0; i<size; i++) {
        printf("[%2d/%2d] sbuf[%2d]: ", rank, size, i);
        for (int j=0; j<ARR_LEN; j++) {
          printf("%3d ", sbuf[(i*ARR_LEN)+j]);
        }
        printf("\n");
      }
    }
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);
  }
  fflush(stdout);
  MPI_Finalize();
  return 0;
}
