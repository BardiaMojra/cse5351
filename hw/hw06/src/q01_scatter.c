#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
#define ROOT 0

void print_buff(int rank, int size, int** arr_ptr, int arr_len);

int main(int argc, char *argv[])
{
  /* init */
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Request request;
  int arr_len;
  int* arr_ptr; // global pointer
  double t1, t2;

  if (rank == ROOT) {
    if ((size != 2) && (size != 4) && (size != 8) && (size != 16)) {
      printf("\n\nUnsupported number of processors: %d\n\n", size);
      exit(13);
    }
  }

  /* NBUG */
  arr_len = rank;
  printf("[%2d/%2d]: init int arr size: %d\n", rank, size, arr_len); fflush(stdout);
  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == ROOT) {
    printf("[%2d/%2d]: <=> :[rank/size]\n", rank, size);
    printf("[%2d/%2d]: Enter an integer array length: \n", rank, size);
    scanf("%d", &arr_len);
    printf("[%2d/%2d]: int array length = %d\n", rank, size, arr_len);

    arr_ptr = (int*) malloc(sizeof(int) * arr_len);
    memset(arr_ptr, 7, arr_len); // <---- init all buffers with 7

    printf("[%2d/%2d]: initial array buffer values \n", rank, size);
    print_buff(rank, size, &arr_ptr, arr_len);

  }
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Bcast(&arr_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);

  /* NBUG */
  printf("[%2d/%2d]: int arr len: %d\n", rank, size, arr_len); fflush(stdout);
  MPI_Barrier(MPI_COMM_WORLD);




  if(rank == ROOT){
    for (int i=0; i<arr_len; i++){
      arr_ptr[i] = i+10;
    }
    printf("[%2d/%2d]: before scatter arr buff values \n", rank, size);
    print_buff(rank, size, &arr_ptr, arr_len);
    t1 = MPI_Wtime();
  }

  MPI_Barrier(MPI_COMM_WORLD);
  printf("[%2d/%2d]: int arr len: %d\n", rank, size, arr_len); fflush(stdout);
  MPI_Barrier(MPI_COMM_WORLD);

  int *rec = NULL;
	MPI_Scatter(arr_ptr, 1, MPI_INT, rec, 1, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  print_buff(rank, size, &arr_ptr, arr_len);
  MPI_Barrier(MPI_COMM_WORLD);


  if (rank == ROOT)
  {
    t2 = MPI_Wtime();
    printf( "Elapsed time:     %f\n", t2-t1);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  fflush(stdout);
  free(arr_ptr);
  MPI_Finalize();
  return 0;
}


/* private routines */
void print_buff(int rank, int size, int** arr_ptr, int arr_len) {
  /* print arr_ptr */
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == ROOT) {
    printf("[%2d/%2d]: ", rank, size);
    for (int j=0; j<arr_len; j++) {
    printf("%3d ", arr_ptr[j]);
    } printf("\n"); fflush(stdout);
  } MPI_Barrier(MPI_COMM_WORLD);
  return;
}
