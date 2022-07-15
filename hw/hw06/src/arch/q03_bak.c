/** sieve1.c finds prime numbers using a parallel/MPI version
 *  of Eratosthenes Sieve.
 * Based on implementation by Quinn
 * Modified by Ryan Holt to correctly handle '-np 1', Fall 2007
 * Modified by Nathan Dykhuis to handle larger ranges, Fall 2009
 * Modified by Bardia Mojra to gather local arrays and print out all primes found
 */

#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

/* config */
#define ROOT 0


char* create_gBuff(int rank, int size, long int* n);
void prt_buff(int rank, int size, char* buff, long int n);
void prt_lBuffs(int rank, int size, char* buff, long int n);
void prt_var(int rank, int size, long int* var, char** lab);
void prt_primes(int rank, int size, char* gBuff, long int n, int arrLen);
int get_arrLen(int size, long int n);
int get_len(int rank, int size, long int n);

int main (int argc, char ** argv) {
  int i;
  long int n;
  int index;
  int len;
  int prime;
  int cnt;
  int gCnt;
  int first;
  long int hi_val;
  long int lo_val;
  int rank;
  int size;
  char* marked;
  double time;
  char* lab = NULL;
  int arrLen;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  char* gBuff = NULL;
  int* gArrLens = NULL;
  if (rank == ROOT){
    if ((2+(n-1/size)) < (int) sqrt((double) n)) {
      if (rank == ROOT) {
        printf("[%2d/%2d]: Too many processors...\n", rank, size);
      }
      MPI_Finalize();
      exit(1);
    }
    gBuff = (char*) create_gBuff(rank, size, &n);
    gArrLens = (int*) calloc(size, sizeof(int));
  }


  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD); //todo remove line
  lab = "n"; prt_var(rank, size, (long int*)&n, &lab); //todo remove line




  MPI_Barrier(MPI_COMM_WORLD);
  time = -MPI_Wtime();


  lo_val = (2 +((long int)(rank)   * (n-1)) ) / (long int)size;
  hi_val = (1 +((long int)(rank+1) * (n-1)) ) / (long int)size;
  len    = hi_val - lo_val + 1;
  arrLen = get_arrLen(size, n);

  MPI_Barrier(MPI_COMM_WORLD); //todo remove line
  lab = "len"; prt_var(rank, size, (long int*)&len, &lab); //todo remove line

  MPI_Barrier(MPI_COMM_WORLD); //todo remove line
  lab = "arrLen"; prt_var(rank, size, (long int*)&arrLen, &lab); //todo remove line


  marked = (char*) calloc(arrLen, sizeof(char));
  assert(marked != NULL);

  if (rank == 0) {
    index = 0;
  }
  /* //todo optimization: skip even numbers, cut message array in half */
  prime = 2;
  do {
    if (prime * prime > lo_val) { /* find the first prime multiple in range */
      first = prime * prime - lo_val;
    } else {
      if ((lo_val % prime) == 0) {
        first = 0;
      }
      else first = prime - (lo_val % prime);
    }

    for (i = first; i < len; i += prime) { /* mark prime multiples */
      marked[i] = 1;
    }
    for(i = len; i<arrLen; i++) { /* mark pad cells at the end of marked arr */
       marked[i] = 2;
    }

    if (rank == 0) {
      while (marked[++index]);
      prime = index + 2;
    }

    if (size > 1) {
      MPI_Bcast(&prime,  1, MPI_INT, 0, MPI_COMM_WORLD);
    }
  } while (prime * prime <= n);

  MPI_Barrier(MPI_COMM_WORLD);
  prt_lBuffs(rank, size, marked, arrLen);

	MPI_Gather(marked, arrLen, MPI_CHAR, gBuff, arrLen, MPI_CHAR, ROOT, MPI_COMM_WORLD);

  cnt = 0;
  for (i = 0; i < len; i++) {
    if (marked[i] == 0) cnt++;
  }

  if (size > 1) {
    MPI_Reduce(&cnt, &gCnt, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
  } else {
    gCnt = cnt;
  }


  if (rank == ROOT) {
    time += MPI_Wtime();
    printf("\n\n[%2d/%2d]: results for n = %d  \n", rank, size, n);
    printf("  time: %f sec \n", time);
    printf("  total primes found: %d \n", gCnt);
    printf("  primes found: \n");
    prt_primes(rank, size, gBuff, n, arrLen);
    free(gBuff);
  }
  free(marked);
  MPI_Finalize();
  return 0;
}

/* private routines */
void prt_buff(int rank, int size, char* buff, long int n) {
  printf("[%2d/%2d]: ", rank, size);
  for (int j=0; j<n; j++) {
    printf("%2d ", buff[j]);
  } printf("\n"); fflush(stdout);
  return;
}

void prt_lBuffs(int rank, int size, char* buff, long int n) {
  for (int i=0; i<size; i++) {
    if (i==rank) {
      prt_buff(rank, size, buff, n); fflush(stdout);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return;
}

char* create_gBuff(int rank, int size, long int* n) {
  printf("[%2d/%2d]: <=> [rank/size]\n", rank, size);
  printf("[%2d/%2d]: Enter n, integer limit: \n", rank, size);
  scanf("%d", n);
  char* gBuff = (char*) calloc((*n), sizeof(char));
  assert(gBuff != NULL);
  memset(gBuff, 0, sizeof(char)*(*n));
  prt_buff(rank, size, gBuff, *n);
  fflush(stdout);
  return gBuff;
}

void prt_var(int rank, int size, long int* var, char** lab) {
  printf("[%2d/%2d]: %7s: %6d\n", rank, size, *lab, *var); fflush(stdout);
  return;
}

void prt_primes(int rank, int size, char* gBuff, long int n, int arrLen) {
  long int idx;
  long int num = 0;
  int len;
  for (int r=0; r<size; r++) {
    len = get_len(rank, size, n);
    for(int a=0; a<len; a++) {
      idx = (r*arrLen) + a;
      printf("%3d:%3d:%d ", num, a, gBuff[idx]);
      if(gBuff[idx] == 0) {
        /*printf("%3d ", num);*/
        num++;
      }
    }
    printf("\n");
  }
  printf("\n"); fflush(stdout);
  return;
}

int get_arrLen(int size, long int n) {
  long int rank   = size - 1;
  long int lo_val = (2 +((rank)   * (n-1)) ) / (long int)size;
  long int hi_val = (1 +((rank+1) * (n-1)) ) / (long int)size;
  long int len    = hi_val - lo_val + 1;
  assert(len<INT_MAX);
  return (int)len;
}


int get_len(int rank, int size, long int n) {
  long int lo_val = (2 +((long int)(rank)   * (n-1)) ) / (long int)size;
  long int hi_val = (1 +((long int)(rank+1) * (n-1)) ) / (long int)size;
  long int len    = hi_val - lo_val + 1;
  assert(len<INT_MAX);
  return (int)len;
}


/* eof */
