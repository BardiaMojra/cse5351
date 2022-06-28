#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
#define ROOT 0

void print_arrBuff(int rank, int size, int* arr_ptr, MPI_Comm comm);

int main(int argc, char *argv[])
{
  /* init */
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Request request;
  int* arr_ptr;

  if size != 2 || size != 4 || size != 8 || size != 16
  {
    printf("\n\nUnsupported number of processors: %d\n", size);
    exit(0);
  }

  int arr_len;
  if (rank == ROOT)
  {
    printf("[%2d/%2d]: <--> [rank/size]\n", rank, size);
    printf("[%2d/%2d]: Enter an integer array length: \n", rank, size);
    scanf("%d", &arr_len);
    printf("[%2d/%2d]: int array length = %d\n", rank, size, arr_len);

  }

  // NBUG
  arr_len = rank;
  printf("[%2d/%2d]: int arr size: %d\n", rank, size, arr_len); fflush(stdout);
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(arr_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);

  // NBUG
  printf("[%2d/%2d]: int arr size: %d\n", rank, size, arr_len); fflush(stdout);
  MPI_Barrier(MPI_COMM_WORLD);


  arr_ptr = (int*) malloc(sizeof(int) * arr_len);
  memset(arr_ptr, 7, sizeof(int) * arr_len); // <---- init all buffers with 7
  print_arr_all(rank, size, arr_ptr, arr_len, MPI_COMM_WORLD);



  if(rank == ROOT){
    for (int i=0; i<arr_len; i++){
      arr_ptr[i] = i+10;}
  }

  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Scatter(arr_ptr, 1, MPI_INT, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  print_arr_all(rank, size, arr_ptr, arr_len, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);

  exit(0);

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
  if (rank == ROOT)
  {
    t2 = MPI_Wtime();
    printf( "Elapsed time:     %f\n", t2-t1);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  fflush(stdout);
  MPI_Finalize();
  return 0;
}


// private routines
void print_arr_all(rank, size, arr_ptr, arr_len, comm)
{
  /* print arr_ptr */
  for (int i=0; i<size; i++) {
    MPI_Barrier(comm);
    if(i == rank)
    {
      printf("[%2d/%2d]: ", rank, size);
      for (int j=0; j<arr_len; j++) {
      printf("%3d ", arr_ptr[j]);
      } printf("\n"); fflush(stdout);
    }
    MPI_Barrier(comm);
  }
  MPI_Barrier(comm);
  return 0;
}
