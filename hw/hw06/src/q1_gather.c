/**
 * @file q1_gather.c
 * @author Bardia Mojra
 * @brief compile with "mpicc q1g.c -o q1g"
 * @version 0.1
 * @date 2022-07-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <mpi.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
#define ROOT 0

void getInput_arrLen(int rank, int size, int* arrLen);
void print_buff(int rank, int size, int* buff, int arrLen);
void print_lBuffs(int rank, int size, int* buff, int arrLen);
void print_arrLen(int rank, int size, int arrLen);
int* create_n_load_lBuffs(int rank, int len);

int main(int argc, char *argv[])
{
  /* init */
  int rank, size;
  int arrLen;
  double t1, t2;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  if ((size != 2) && (size != 4) && (size != 8) && (size != 16)) {
    if (rank == ROOT)
      printf("\n\nUnsupported number of processors: %d\n\n", size);
  }

  int* gBuff = NULL;
  if (rank == ROOT) {
    getInput_arrLen(rank, size, &arrLen);
    gBuff = (int*) calloc(arrLen*size, sizeof(int));
    assert(gBuff != NULL); memset(gBuff, 0, arrLen*size*sizeof(int));
    printf("[%2d/%2d]: global-address buffer: \n", rank, size);
    print_buff(rank, size, gBuff, arrLen*size);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&arrLen, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  int* lBuff = NULL;
  lBuff = create_n_load_lBuffs(rank, arrLen);

  if(rank == ROOT){
    printf("[%2d/%2d]: global buffer before MPI_Gather()... \n", rank, size);
    print_buff(rank, size, gBuff, arrLen*size);
    printf("[%2d/%2d]: local buffers before MPI_Gather()... \n", rank, size);
  }
  print_lBuffs(rank, size, lBuff, arrLen);

  if(rank == ROOT){
    t1 = MPI_Wtime();
  }

  MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(lBuff, arrLen, MPI_INT, gBuff, arrLen, MPI_INT, ROOT, MPI_COMM_WORLD);

  if (rank == ROOT){
    t2 = MPI_Wtime();
    printf( "Elapsed time:     %f\n", t2-t1);
    printf("[%2d/%2d]: global buffer after MPI_Gather()... \n", rank, size);
    print_buff(rank, size, gBuff, arrLen*size);
    free(gBuff);
    printf("[%2d/%2d]: local buffers after MPI_Gather()... \n", rank, size);
  }
  print_lBuffs(rank, size, lBuff, arrLen);
  free(lBuff);
  MPI_Finalize();
  return 0;
}

/* private routines */
void print_buff(int rank, int size, int* buff, int arrLen) {
  printf("[%2d/%2d]: ", rank, size);
  for (int j=0; j<arrLen; j++) {
    printf("%3d ", buff[j]);
  } printf("\n"); fflush(stdout);
  return;
}

void print_lBuffs(int rank, int size, int* buff, int arrLen) {
  for ( int i=0; i<size; i++) {
    if (i==rank) {
      print_buff(rank, size, buff, arrLen);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return;
}

void getInput_arrLen(int rank, int size, int* arrLen) {
  printf("[%2d/%2d]: <=> :[rank/size]\n", rank, size);
  printf("[%2d/%2d]: Enter the array length: \n", rank, size);
  scanf("%d", arrLen);
  return;
}

void print_arrLen(int rank, int size, int arrLen) {
  printf("[%2d/%2d]: int arr len: %d\n", rank, size, arrLen); fflush(stdout);
  return;
}

int* create_n_load_lBuffs(int rank, int len) {
  int* lBuff = NULL;
  lBuff = (int*) calloc(len, sizeof(int));
  assert(lBuff != NULL); memset(lBuff, 0, len*sizeof(int));
  for (int i=0; i<len; i++) {
    lBuff[i] = (rank*len) + i;
  }
  return lBuff;
}
