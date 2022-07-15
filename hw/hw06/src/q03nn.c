#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

/* config */
#define ROOT 0


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
  double time;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Barrier(MPI_COMM_WORLD);
  time = -MPI_Wtime();

  /* Check for the command line argument */
  if (argc != 2) {
    if (rank == 0) printf("Please supply a range.\n");
    MPI_Finalize();
    exit(1);
  }

  n = atoi(argv[1]);

  if ((2 + (n-1/size)) < (int) sqrt((double) n)) {
    if (rank == 0) printf("Too many processes.\n");
    MPI_Finalize();
    exit(1);
  }


  loVal  = 2 +(long int)(rank) * (long int)(n-1) / (long int)size;
  hiVal = 1 +(long int)(rank+1) * (long int)(n-1) / (long int)size;
  len = hiVal - loVal + 1;

  marked = (char*) calloc(len, sizeof(char));
  if (marked == NULL) {
   printf("Cannot allocate enough memory.\n");
   MPI_Finalize();
   exit(1);
  }

  if (rank == 0)
  {
    idx = 0;
  }

  prime = 2;
  do {
    if (prime * prime > loVal) {
      first = prime * prime - loVal;
    } else {
      if ((loVal % prime) == 0) {
        first = 0;
      }
      else first = prime - (loVal % prime);
    }

    for (i = first; i < len; i += prime) {
      marked[i] = 1;
    }

    if (rank == 0) {
      while (marked[++idx]);
      prime = idx + 2;
    }

    if (size > 1) {
      MPI_Bcast(&prime,  1, MPI_INT, 0, MPI_COMM_WORLD);
    }
  } while (prime * prime <= n);

  cnt = 0;
  for (i = 0; i < len; i++) if (marked[i] == 0) cnt++;

  if (size > 1) {
    MPI_Reduce(&cnt, &gCnt, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  } else {
    gCnt = cnt;
  }

  if(rank == ROOT) { // ---------------------------------------------->> results
    time += MPI_Wtime();
    printf("\n\n[%2d/%2d]: results for n = %d\n", rank, size, n);fflush(stdout);
    printf("  - time: %f sec \n", time); fflush(stdout); fflush(stdout);
    printf("  - total primes found: %d \n", gCnt); fflush(stdout);
    printf("  - prime numbers found: \n"); fflush(stdout);
  }

  prt_primesOrdered(rank, size, n, marked, len, loVal); // ------>> print primes

  MPI_Finalize();
  return 0;
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
