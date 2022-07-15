/**
  @author Bardia Mojra
  @link https://www.math.tu-cottbus.de/~kd/parallel/mpi/mpi-course.book_133.html
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
#define NBUG

/* private declarations */
void prt_buff(int rank, int size, char* buff, int len);
void prt_lBuffs(int rank, int size, char* buff, int n);
void getInput_n(int rank, int size, int* n);
int get_loVal(int rank, int size, int n);
int get_hiVal(int rank, int size, int n);
int get_len(int rank, int size, int n);
void prt_var(int rank, int size, int* var, char** lab);
void prt_varOrdered(int rank, int size, int* var, char** lab);
void find_primes(int rank, int size, int n, char* marked);
void prt_primesOrdered(int rank, int size, int n, char* marked);
void prt_primes(int rank, int size, int n, char* marked);
void get_gCnt(int rank, int size, int len, char* marked, int* cnt, int* gCnt);

int main (int argc, char ** argv) {
  int rank;
  int size;
  int n; // input number, find primes smaller and equal to n
  char* marked; // marked as "not a prime", use char to reduce comm time
  double time; // parallel compute time
  int len; // number of assigned local prime numbers

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if(rank == ROOT){ getInput_n(rank, size, &n); /* get limit n */ }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  assert(~((2+(n-1/size)) < (int) sqrt((double) n)) && \
    "[assert]->> too many processors!!");

  if(rank == ROOT) { time = -MPI_Wtime(); /* start timer and process */ }

  len = get_len(rank, size, n);
  MPI_Barrier(MPI_COMM_WORLD);

  find_primes(rank, size, n, marked); // ------------->> calculate prime numbers
  MPI_Barrier(MPI_COMM_WORLD);

  int gCnt = 0; // get total count
  int cnt = 0; // local cnt
  get_gCnt(rank, size, len, marked, &cnt, &gCnt);

  if(rank == ROOT) { // ---------------------------------------------->> results
    time += MPI_Wtime();
    printf("\n\n[%2d/%2d]: results for n = %d\n", rank, size, n);fflush(stdout);
    printf("  - time: %f sec \n", time); fflush(stdout); fflush(stdout);
    printf("  - total primes found: %d \n", gCnt); fflush(stdout);
    printf("  - prime numbers found: \n"); fflush(stdout);
  }

  prt_primesOrdered(rank, size, n, marked); // ------------------>> print primes

  free(marked);
  MPI_Finalize();
  return 0;
}

/* private routines */
void prt_buff(int rank, int size, char* buff, int len) {
  printf("[%2d/%2d]: ", rank, size);
  for(int j=0; j<len; j++) {
    printf("%2d ", buff[j]);
  } printf("\n"); fflush(stdout);
  return;
}

void prt_lBuffs(int rank, int size, char* buff, int n) {
  for(int i=0; i<size; i++) {
    if(i==rank) {
      prt_buff(rank, size, buff, n); fflush(stdout);
    } MPI_Barrier(MPI_COMM_WORLD);
  } MPI_Barrier(MPI_COMM_WORLD);
  return;
}

void getInput_n(int rank, int size, int* n) {
  printf("[%2d/%2d]: <=> [rank/size]\n", rank, size);
  printf("[%2d/%2d]: number of processors: %d\n", rank, size, size);
  printf("[%2d/%2d]: Enter n, integer limit: \n", rank, size); fflush(stdout);
  scanf("%d", n);
  return;
}

int get_loVal(int rank, int size, int n) {
  if(rank==ROOT) {
    return 1;
  } else {
    /* return (2 + (int)(((rank*(n-1)))/size)); */
    return (int)(2 +(long int)(rank) * (long int)(n-1) / (long int)size);
  }
}

int get_hiVal(int rank, int size, int n) {
  /* return (1 + (int)(((rank+1)*(n-1))/size)); */
  return (int)(1 +(long int)(rank+1) * (long int)(n-1) / (long int)size);
}

int get_len(int rank, int size, int n) {
  int lo_val = get_loVal(rank, size, n);
  int hi_val = get_hiVal(rank, size, n);
  int len    = hi_val - lo_val + 1;
  assert((len<INT_MAX) && "len is greater than INT_MAX!!");
  return (int)len;
}

void prt_var(int rank, int size, int* var, char** lab) {
  printf("[%2d/%2d]: %7s: %6d\n", rank, size, *lab, *var); fflush(stdout);
  return;
}

void prt_varOrdered(int rank, int size, int* var, char** lab) {
  for(int i=0; i<size; i++) {
    if(i==rank) {
      prt_var(rank, size, var, lab);
    } MPI_Barrier(MPI_COMM_WORLD);
  } MPI_Barrier(MPI_COMM_WORLD);
  if(rank == ROOT) { printf("\n"); fflush(stdout); }
  return;
}

void find_primes(int rank, int size, int n, char* marked) {
  int idx;
  int cPrime; // current prime
  int fPrime; // first prime in local array
  int lo_val = get_loVal(rank, size, n);
  int hi_val = get_hiVal(rank, size, n);
  int len    = get_len(rank, size, n);
  marked = (char*) calloc(len, sizeof(char));
  assert(marked != NULL);  memset(marked, 0, len);
#ifdef NBUG
  char* lab = NULL;
  lab = "lo_val"; prt_var(rank, size, &lo_val, &lab); // NBUG
  lab = "hi_val"; prt_var(rank, size, &hi_val, &lab); // NBUG
  lab = "len"; prt_var(rank, size, &len, &lab); // NBUG
#endif // NBUG

  if(rank == ROOT) { idx = 0; /* root finds primes<sqrt(n) n broadcasts */ }
  cPrime = 2;
  while(cPrime*cPrime <= n) {
    if(cPrime*cPrime > lo_val) { /* find the fPrime prime multiple in range */
      fPrime = cPrime * cPrime - lo_val;
    } else {
      if((lo_val % cPrime) == 0) { // not a prime
        fPrime = 0;
      } else {
        fPrime = cPrime - (lo_val % cPrime); // first prime in local array
      }
    }
    for(int i=fPrime; i<len; i+=cPrime) { /* mark prime multiples */
      marked[i] = 1;
    }
    if(rank == ROOT) {
      while(marked[++idx]);
      cPrime = idx + 2;
    }

    if(size > 1) { MPI_Bcast(&cPrime, 1, MPI_INT, 0, MPI_COMM_WORLD); }
    if(rank == ROOT) { lab = "cPrime"; prt_var(rank, size, &cPrime, &lab); }
  } // end of while(cPrime*cPrime <= n)

  lab = "lo_val"; prt_var(rank, size, &lo_val, &lab); // NBUG
  return;
}

void prt_primesOrdered(int rank, int size, int n, char* marked) {
  for(int i=0; i<size; i++) {
    if(i==rank) {
      prt_primes(rank, size, n, marked);
    } MPI_Barrier(MPI_COMM_WORLD);
  } MPI_Barrier(MPI_COMM_WORLD);
  if(rank == ROOT) { printf("\n"); fflush(stdout); }
  return;
}

void prt_primes(int rank, int size, int n, char* marked) {
  int lo_val = get_loVal(rank, size, n);
  int len = get_len(rank, size, n);
  printf("[%2d/%2d]: ", rank, size);
  for(int i=0; i<len; i++) {
    if(marked[i]==0) {
      printf("%3d ", marked[i]+lo_val);
    }
  } printf("\n"); fflush(stdout);
  return;
}

void get_gCnt(int rank, int size, int len, char* marked, int* cnt, int* gCnt) {
  for(int i = 0; i<len; i++) {
    if(marked[i] == 0) cnt++;
  }
  if(size > 1) {
    MPI_Reduce(&cnt, &gCnt, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
  } else {
    gCnt = cnt;
  }
  return;
}
/* eof */
