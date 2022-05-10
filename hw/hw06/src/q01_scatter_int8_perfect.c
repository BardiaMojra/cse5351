#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
#define ROOT 0
#define ARR_LEN 8

int main(int argc, char *argv[])
{
  /* init */
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Request request;
  int arr[ARR_LEN];
  int sbuf[size*ARR_LEN*sizeof(int)];
  int rbuf[ARR_LEN*sizeof(int)];
  int sbuf_size = size*ARR_LEN*sizeof(int);
  int *rptr = &rbuf[0];
  int *sptr = &sbuf[0];;
  memset(sbuf,0,sizeof(sbuf));
  memset(rbuf,0,sizeof(rbuf));
  double t1, t2;

  if (rank == ROOT)
  {
    t1 = MPI_Wtime();
    printf("rank: %2d \n", rank);
    /* load buffers */
    for (int i=0; i<size; i++) {
      /* construct msg array */
      printf(" msg[(%2d/%2d)>%2d]: ", rank, size, i);
      for (int j=0; j<ARR_LEN; j++) {
        arr[j] = ((i*ARR_LEN)+j);
        printf("%3d ", arr[j]);
      }
      printf("\n");
      /* load msg array to send buffer */
      memcpy(&sbuf[i*ARR_LEN], arr, sizeof(sbuf));
      printf("sbuf[(%2d/%2d)>%2d]: ", rank, size, i);
      for (int j=0; j<ARR_LEN; j++) {
        printf("%3d ", sbuf[(i*ARR_LEN)+j]);
      } printf("\n");
      printf("\n");
      printf("rbuf[(%2d/%2d)<%2d]: ", rank, size, i);
      for (int j=0; j<ARR_LEN; j++) {
        printf("%3d ", rbuf[(i*ARR_LEN)+j]);
      }  printf("\n");
      fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank !=ROOT)
  {
    printf("rank: %2d \n", rank);
    /* print buffers */
    for (int i=0; i<size; i++) {
    printf("sbuf[(%2d/%2d)>%2d]: ", i, size, rank);
    for (int j=0; j<ARR_LEN; j++) {
      printf("%3d ", sbuf[(rank*ARR_LEN)+j]);
      }
      printf("\n");
    } printf("\n"); fflush(stdout);

    for (int i=0; i<size; i++) {
      printf("rbuf[(%2d/%2d)<%2d]: ", i, size, rank);
      for (int j=0; j<ARR_LEN; j++) {
        printf("%3d ", rbuf[(i*ARR_LEN)+j]);
      }
      printf("\n");
    } printf("\n"); fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Scatter(&sbuf, ARR_LEN, MPI_INT, &rbuf, ARR_LEN, MPI_INT, ROOT, MPI_COMM_WORLD);
  /* MPI_Gather(rbuf, ARR_LEN, MPI_INT, sbuf, ARR_LEN, MPI_INT, ROOT, MPI_COMM_WORLD); */

  MPI_Barrier(MPI_COMM_WORLD);

  for (int a=0; a<size; a++) {
    if ((rank == ROOT) && (a == rank)) {
      t2 = MPI_Wtime();
      printf( "Elapsed time:     %f\n", t2-t1);

      printf("rank: %2d \n", rank);

      /* print buffers */
      for (int i=0; i<size; i++) {
        printf("sbuf[(%2d/%2d)<%2d]: ", i, size, rank);
        for (int j=0; j<ARR_LEN; j++) {
          printf("%3d ", sbuf[(i*ARR_LEN)+j]);        }
        printf("\n");
      } printf("\n"); fflush(stdout);

      for (int i=0; i<size; i++) {
        printf("rbuf[(%2d/%2d)>%2d]: ", i, size, rank);
        for (int j=0; j<ARR_LEN; j++) {
          printf("%3d ", rbuf[(i*ARR_LEN)+j]);        }
        printf("\n");
      } printf("\n"); fflush(stdout);
    }

    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    if ((a!=ROOT) && (a==rank) )
    {
      /* print buffers */
      printf("rank: %2d \n", rank);
      for (int i=0; i<size; i++) {
        printf("sbuf[(%2d/%2d)>%2d]: ", rank, size, ROOT);
        for (int j=0; j<ARR_LEN; j++) {
          printf("%3d ", sbuf[(rank*ARR_LEN)+j]);
        }
        printf("\n");
      } printf("\n"); fflush(stdout);

      for (int i=0; i<size; i++) {
        printf("rbuf[(%2d/%2d)<%2d]: ", rank, size, ROOT);
        for (int j=0; j<ARR_LEN; j++) {
          printf("%3d ", rbuf[(i*ARR_LEN)+j]);
        }
        printf("\n");
      } printf("\n"); fflush(stdout);
    }

    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  fflush(stdout);
  MPI_Finalize();
  return 0;
}
