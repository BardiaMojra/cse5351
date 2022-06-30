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

/* config */
#define ROOT 0
#define PRT_TIME_EN
#define PRT_COUNT_EN
#define PRT_PRIMES_EN

char* create_gBuff(int rank, int size, long int* n);
void prt_buff(int rank, int size, char* buff, long int n);
void prt_lBuffs(int rank, int size, char* buff, long int n);
void prt_var(int rank, int size, long int* var, char** lab);
void prt_primes(int rank, int size, char* gBuff, long int n);


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
  long int low_val;
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
        printf("[ROOT]: Too many processors...\n");
      }
      MPI_Finalize();
      exit(1);
    }
    gBuff = (char*) create_gBuff(rank, size, &n);
    gArrLens = (int*) calloc(size, sizeof(int));
  }


  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  lab = "n"; prt_var(rank, size, (long int*)&n, &lab);




  MPI_Barrier(MPI_COMM_WORLD);
  time = -MPI_Wtime();


  low_val = 2 +(long int)(rank) * (long int)(n-1) / (long int)size;
  hi_val  = 1 +(long int)(rank+1) * (long int)(n-1) / (long int)size;
  len     = hi_val - low_val + 1;



	MPI_Gather(&len, 1, MPI_INT, gArrLens, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
  if (rank == ROOT) {
    maxLen = get_maxArrLen(gArrLens);
  }
  //todo
  MPI_Bcast(arrLen);


  MPI_Barrier(MPI_COMM_WORLD); // nbug
  lab = "len"; prt_var(rank, size, (long int*)&len, &lab);

  MPI_Barrier(MPI_COMM_WORLD); // nbug
  lab = "arrLen"; prt_var(rank, size, (long int*)&arrLen, &lab);


  marked = (char*) calloc(arrLen, sizeof(char));
  assert(marked != NULL);

  if (rank == 0) {
    index = 0;
  }

  prime = 2;
  do {
    if (prime * prime > low_val) {
      first = prime * prime - low_val;
    } else {
      if ((low_val % prime) == 0) {
        first = 0;
      }
      else first = prime - (low_val % prime);
    }

    for (i = first; i < len; i += prime) {
      marked[i] = 1;
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
    printf("[ROOT]: results: n = %d \n", n);
    #ifdef(PRT_TIME_EN)
      printf("    time: %f sec \n", time);
    #endif
    #ifdef(PRT_COUNT_EN)
      printf("    total primes found: %d \n", gCnt);
    #endif
    #ifdef(PRT_PRIMES_EN)
      printf("    primes found: \n");
      prt_primes(rank, size, gBuff, n);
    #endif
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
  for ( int i=0; i<size; i++) {
    if (i==rank) {
      prt_buff(rank, size, buff, n);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  return;
}

char* create_gBuff(int rank, int size, long int* n) {
  printf("[%2d/%2d]: <=> [rank/size]\n", rank, size);
  printf("[%2d/%2d]: Enter n, integer limit: \n", rank, size); fflush(stdout);
  scanf("%d", n);
  char* gBuff = (char*) calloc((*n), sizeof(char));
  assert(gBuff != NULL);
  memset(gBuff, 0, sizeof(char)*(*n));
  prt_buff(rank, size, gBuff, *n);
  return gBuff;
}

void prt_var(int rank, int size, long int* var, char** lab) {
  printf("[%2d/%2d]: %8s: %8d\n", rank, size, *lab, *var); fflush(stdout);
  return;
}

void prt_primes(int rank, int size, char* gBuff, long int n) {
  printf("\n  2 ", rank, size);
  for (int i=1; i<=n; i++) {
    if (gBuff[i] == 0) {
      printf(" %2d ", i);
    }
  }fflush(stdout);
  return;
}
