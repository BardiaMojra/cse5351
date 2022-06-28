#include <mpi.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
#define ROOT 0

int* create_gBuff(int rank, int size, int* arrLen);
void print_buff(int rank, int size, int* buff, int arrLen);
void print_arrLen(int rank, int size, int arrLen);

int main(int argc, char *argv[])
{
  /* init */
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Request request;
  int arrLen = 0;
  arrLen = rank;
  double t1, t2;

  if (rank == ROOT) {
    if ((size != 2) && (size != 4) && (size != 8) && (size != 16)) {
      printf("\n\nUnsupported number of processors: %d\n\n", size);
      exit(13);
    }
  }


  int* gBuff = NULL;
  if (rank == ROOT) {
    gBuff = create_gBuff(rank, size, &arrLen);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&arrLen, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
  print_arrLen(rank, size, arrLen);

  int* lBuff = NULL;
  lBuff = (int*) malloc(sizeof(int) * arrLen);
  assert(lBuff != NULL);
  memset(lBuff, 0, arrLen*sizeof(int));
  print_buff(rank, size, lBuff, arrLen);




  if(rank == ROOT){
    for (int i=0; i<arrLen*size; i++){
      gBuff[i] = i+1;
    }
    printf("[%2d/%2d]: before scatter arr buff values \n", rank, size);
    print_buff(rank, size, gBuff, arrLen*size);
    t1 = MPI_Wtime();
  }
  MPI_Barrier(MPI_COMM_WORLD);

  print_buff(rank, size, lBuff, arrLen);



	MPI_Scatter(gBuff, arrLen, MPI_INT, lBuff, arrLen, MPI_INT, ROOT, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  print_buff(rank, size, lBuff, arrLen);
  MPI_Barrier(MPI_COMM_WORLD);
  print_buff(rank, size, lBuff, arrLen);
  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == ROOT)
  {
    t2 = MPI_Wtime();
    printf( "Elapsed time:     %f\n", t2-t1);
    free(gBuff);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  fflush(stdout);


  free(lBuff);
  MPI_Finalize();
  return 0;
}


/* private routines */
void print_buff(int rank, int size, int* buff, int arrLen) {
  if(rank == ROOT) {
    printf("[%2d/%2d]: ", rank, size);
    for (int j=0; j<arrLen; j++) {
    printf("%3d ", buff[j]);
    } printf("\n"); fflush(stdout);
  }
  return;
}


int* create_gBuff(int rank, int size, int* arrLen) {
  printf("[%2d/%2d]: <=> :[rank/size]\n", rank, size);
  printf("[%2d/%2d]: Enter the array length: \n", rank, size);
  scanf("%d", arrLen);
  printf("[%2d/%2d]: int array length = %d\n", rank, size, *arrLen);
  int* gBuff = (int*) malloc(sizeof(int)*((*arrLen)*size));
  assert(gBuff != NULL);
  memset(gBuff, 0, size*sizeof(*gBuff));
  print_buff(rank, size, gBuff, *arrLen);
  return gBuff;
}


void print_arrLen(int rank, int size, int arrLen) {
  printf("[%2d/%2d]: int arr len: %d\n", rank, size, arrLen); fflush(stdout);
  MPI_Barrier(MPI_COMM_WORLD);
}
