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
  int rbuf[size*ARR_LEN];
  int *rbuf_ptr;
  rbuf_ptr = &rbuf;
  int sbuf[ARR_LEN];
  memset(rbuf,0,sizeof(rbuf));
  memset(sbuf,0,sizeof(sbuf));
  double t1, t2;

  if (rank == ROOT)
  {
    t1 = MPI_Wtime();
    printf("rank: %2d \n", rank);
    /* print rbuf (cleared) */
    for (int i=0; i<size; i++) {
      printf("rbuf[(%2d/%2d)<%2d]: ", rank, size, i);
      for (int j=0; j<ARR_LEN; j++) {
        printf("%3d ", rbuf[(i*ARR_LEN)+j]);
      } printf("\n"); fflush(stdout);
    } printf("\n");fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  /* load send buffers */
  for (int a=0; a<size; a++)
  {
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank==a) {
      printf("rank: %2d \n", rank);
      for (int i=0; i<size; i++)
      {
        /* construct msg arr */
        printf(" msg[(%2d/%2d)>%2d]: ", rank, size, ROOT);
        for (int j=0; j<ARR_LEN; j++) {
          arr[j] = ((a*ARR_LEN)+j);
          printf("%3d ", arr[j]);
        }  printf("\n"); fflush(stdout);
        /* load arr to sbuf */
        memcpy(sbuf, arr, sizeof(sbuf));
        /* print sbuf */
        printf("sbuf[(%2d/%2d)>%2d]: ", rank, size, ROOT);
        for (int j=0; j<ARR_LEN; j++) {
          printf("%3d ", sbuf[j]);
        }  printf("\n"); fflush(stdout);
      }
    } // end if(i==rank)
  } printf("\n");fflush(stdout);

  MPI_Gather(rbuf_ptr, ARR_LEN, MPI_INT, sbuf, ARR_LEN, MPI_INT, ROOT, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);

  for (int a=0; a<size; a++) {
    MPI_Barrier(MPI_COMM_WORLD);

    if ((rank == ROOT) && (a == rank)) {
      t2 = MPI_Wtime();
      printf( "Elapsed time:     %f\n", t2-t1);

      printf("rank: %2d \n", rank);

      /* print sbuf */
      printf("sbuf[(%2d/%2d)>%2d]: ", rank, size, ROOT);
      for (int j=0; j<ARR_LEN; j++) {
        printf("%3d ", sbuf[j]);
      } printf("\n"); fflush(stdout);

      /* print rbuf */
      for (int i=0; i<size; i++) {
        printf("rbuf[(%2d/%2d)<%2d]: ", ROOT, size, i);
        for (int j=0; j<ARR_LEN; j++) {
          printf("%3d ", rbuf[(i*ARR_LEN)+j]);
        } printf("\n");
      } printf("\n"); fflush(stdout);
    } /*end of if root*/

    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    if ((a!=ROOT) && (a==rank) ) {
      /* print buffers */
      printf("rank: %2d \n", rank);

      /* print sbuf */
      printf("sbuf[(%2d/%2d)>%2d]: ", rank, size, ROOT);
      for (int j=0; j<ARR_LEN; j++) {
        printf("%3d ", sbuf[j]);
      } printf("\n"); fflush(stdout);

      /* print rbuf */
      for (int i=0; i<size; i++) {
        printf("rbuf[(%2d/%2d)<%2d]: ", rank, size, ROOT);
        for (int j=0; j<ARR_LEN; j++) {
          printf("%3d ", rbuf[(i*ARR_LEN)+j]);
        } printf("\n");
      } printf("\n"); fflush(stdout);
    } fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  fflush(stdout);
  MPI_Finalize();
  return 0;
}
