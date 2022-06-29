#include <mpi.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* config */
#define ROOT 0
#define BLOCK_FIRST 3 /* first prime */
#define BLOCK_STEP 2  /* skip odds */
#define MIN(a, b) ((a) < (b)? (a): (b))
#define BLOCK_LOW(id, p, n) ((id) * (n) / (p) / BLOCK_STEP)
#define BLOCK_HIGH(id, p, n) (BLOCK_LOW((id) + 1, p, n) - 1)
#define BLOCK_SIZE(id, p, n) (BLOCK_LOW((id) + 1, p, n) - BLOCK_LOW((id), p, n))
#define BLOCK_OWNER(index, p, n) (((p) * ((index) + 1) - 1) / (n))
#define BLOCK_VALUE_TO_INDEX(val, id, p, n) (val - BLOCK_FIRST) / BLOCK_STEP - BLOCK_LOW(id, p, n - 1)


/* local declarations */
int* create_gBuff(int rank, int size, int* arrLen);
void print_buff(int rank, int size, int* buff, int arrLen);
void print_lBuffs(int rank, int size, int* buff, int arrLen);
void print_arrLen(int rank, int size, int arrLen);


int main(int argc, char *argv[])
{




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

int* create_gBuff(int rank, int size, int* arrLen) {
  printf("[%2d/%2d]: <=> :[rank/size]\n", rank, size);
  printf("[%2d/%2d]: Enter the array length: \n", rank, size);
  scanf("%d", arrLen);
  int* gBuff = (int*) malloc(sizeof(int)*((*arrLen)*size));
  assert(gBuff != NULL);
  memset(gBuff, 0, sizeof(int)*((*arrLen)*size));
  print_buff(rank, size, gBuff, *arrLen);
  return gBuff;
}

void print_arrLen(int rank, int size, int arrLen) {
  printf("[%2d/%2d]: int arr len: %d\n", rank, size, arrLen); fflush(stdout);
  return;
}
