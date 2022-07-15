/**
 * @file q3.c
 * @author Bardia Mojra
 * @brief compile with "mpicc -g q3.c -o q3 -lm -O3"
 * @version 0.1
 * @date 2022-07-15
 *
 * @copyright Copyright (c) 2022
 *
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

void getInput_n(int rank, int size, int* n, int* opt);
void prt_primes(int rank, int size, int n, char* marked, int len, int loVal);
void prt_primesOrdered(int rank, int size, int n, char* marked, int len, int loVal);

int main (int argc, char ** argv) {
  int i;
  int n;
  int idx;
  int len;
  int prime;
  int cnt;
  int gCnt;
  int first;
  long int hiVal;
  long int loVal;
  int rank;
  int size;
  char* marked;
  double t1, t2;
  int opt = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if(rank == ROOT){ getInput_n(rank, size, &n, &opt); /* get limit n */ }

  MPI_Barrier(MPI_COMM_WORLD); // wait for ROOT to finish
  MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
  MPI_Bcast(&opt, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  assert(~((2+(n-1/size)) < (int)sqrt((double)n)) && \
    "[assert]->> too many processors!!");

  if(rank==ROOT) { t1 = MPI_Wtime(); /* start timer and process */ }

  loVal  = 2 + (long int)(rank)   * (long int)(n-1) / (long int)size;
  hiVal  = 1 + (long int)(rank+1) * (long int)(n-1) / (long int)size;
  len = hiVal - loVal + 1;

  marked = (char*) calloc(len, sizeof(char));
  assert(marked != NULL);  memset(marked, 0, len);
  if(rank==ROOT) { idx = 0; /* root finds primes<sqrt(n) n broadcasts */ }
  prime = 2; /* start at 2 */
  while (prime * prime <= n) {  /* find first prime multiple in range */
    if(prime * prime > loVal) {
      first = prime * prime - loVal;
    } else {
      if((loVal % prime) == 0) { // not a prime
        first = 0;
      }
      else first = prime - (loVal % prime); // first prime in local array
    }
    for (i = first; i < len; i += prime) { /* mark prime multiples */
      marked[i] = 1;
    }
    if(rank==ROOT) { /* check next unmarked number */
      while (marked[++idx]);
      prime = idx + 2;
    }
    if(size > 1) { /* bcast prime to be marked off */
      MPI_Bcast(&prime,  1, MPI_INT, 0, MPI_COMM_WORLD);
    }
  }

  cnt = 0; // cnt local primes and sum via MPI_Reduce
  for( i=0; i<len; i++) {
    if(marked[i] == 0) { cnt++; }
  }

  if(size > 1) {
    MPI_Reduce(&cnt, &gCnt, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  } else {
    gCnt = cnt;
  }

  if(rank == ROOT) { // ---------------------------------------------->> results
    t2 = MPI_Wtime();
    printf("\n\n[%2d/%2d]: results for n = %d\n", rank, size, n);
    printf("  - time: %f sec \n", t2-t1);
    printf("  - total primes found: %d \n", gCnt);
    printf("  - prime numbers found: \n"); fflush(stdout);
  }
  if(opt==0) {
    prt_primesOrdered(rank, size, n, marked, len, loVal); // ---->> print primes
  }
  MPI_Finalize();
  return 0;
}

void getInput_n(int rank, int size, int* n, int* opt) {
  printf("[%2d/%2d]: <=> [rank/size]\n", rank, size);
  printf("[%2d/%2d]: number of processors: %d\n", rank, size, size);
  printf("[%2d/%2d]: Enter n, integer limit: \n", rank, size); fflush(stdout);
  scanf("%d", n);
  printf("[%2d/%2d]: Enter opt, 0 for default setting, 1 to disable prime print: \n", \
    rank, size); fflush(stdout);
  scanf("%d", opt);
  return;
}


void prt_primesOrdered(int rank, int size, int n, char* marked, int len, int loVal) {
  for(int i=0; i<size; i++) {
    if(i==rank) {
      prt_primes(rank, size, n, marked, len, loVal);
    } MPI_Barrier(MPI_COMM_WORLD);
  } MPI_Barrier(MPI_COMM_WORLD);
  if(rank == ROOT) { printf("\n"); fflush(stdout); }
  return;
}

void prt_primes(int rank, int size, int n, char* marked, int len, int loVal) {
  printf("[%2d/%2d]: ", rank, size);
  for(int i=0; i<len; i++) {
    if(marked[i]==0) {
      printf("%3d ", i+loVal);
    }
  } printf("\n"); fflush(stdout);
  return;
}
