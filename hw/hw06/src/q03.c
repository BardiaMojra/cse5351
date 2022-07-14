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
void getInput_n(int rank, int size, int* n);
void prt_buff(int rank, int size, char* buff, int len);
void prt_lBuffs(int rank, int size, char* buff, int n);
void prt_var(int rank, int size, int* var, char** lab);
void prt_varOrdered(int rank, int size, int* var, char** lab);
int get_arrLen(int size, int n);
int get_len(int rank, int size, int n);
void find_primes(int rank, int size, int n, char* marked);
void find_primesOrdered(int rank, int size, int n, char* marked);
void prt_primesOrdered(int rank, int size, int n, char* marked, int lenMX);
void prt_primes(int rank, int size, int n, char* marked, int lenMX);

int main (int argc, char ** argv) {
  int rank;
  int size;
  int n; // input number, find primes smaller and equal to n
  int index;
  int prime;
  int cnt; // local num of primes // NBUG
  int gCnt; // global num of primes // NBUG
  int fPrime;
  int hi_val; // local high val integer
  int lo_val; // local low val integer
  char* marked; // marked as "not a prime" - keep char to reduce comm message size
  double time; // parallel compute time
  int len; // number of assigned local prime numbers
  int lenMX; // length of local array buffer

#ifdef NBUG
  char* lab = NULL;
#endif // NBUG

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == ROOT){
    getInput_n(rank, size, &n);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

  assert(~((2+(n-1/size)) < (int) sqrt((double) n)) && \
    "[assert]->> too many processors!!");


  if(rank == ROOT) {
    /* gbuf = (char*) calloc(n, sizeof(char)); */
    /* assert(gbuf != NULL);  memset(gbuf, 0, n*sizeof(char)); */
    time = -MPI_Wtime(); // start timer and process
  }

  len    = get_len(rank, size, n);
  lenMX  = get_arrLen(size, n);

#ifdef NBUG
  find_primesOrdered(rank, size, n, marked);
#else // normal
  find_primes(rank, size, n, marked); // --->> calculate prime numbers
#endif // NBUG

  prt_primesOrdered(rank, size, n, marked, lenMX);

  cnt = 0; // get total count
  for (int i = 2; i < len; i++) {
    if (marked[i] == 0) cnt++;
  }
  if (size > 1) {
    MPI_Reduce(&cnt, &gCnt, 1, MPI_INT, MPI_SUM, ROOT, MPI_COMM_WORLD);
  } else {
    gCnt = cnt;
  }

  if (rank == ROOT) { // ---->>> results
    time += MPI_Wtime();
    printf("\n\n[%2d/%2d]: results for n = %d  \n", rank, size, n); fflush(stdout);
    printf("  - time: %f sec \n", time); fflush(stdout); fflush(stdout);
    printf("  - total primes found: %d \n", gCnt); fflush(stdout);
#ifdef NBUG
    printf("  - format: [ num : local-idx : global-idx : mark ]\n"); fflush(stdout);
#endif // NBUG
    printf("  - prime numbers found: \n"); fflush(stdout);
    prt_primes(rank, size, n, marked, lenMX);
  }
  free(marked);
  MPI_Finalize();
  return 0;
}

/* private routines */
void prt_buff(int rank, int size, char* buff, int len) {
  printf("[%2d/%2d]: ", rank, size);
  for (int j=0; j<len; j++) {
    printf("%2d ", buff[j]);
  } printf("\n"); fflush(stdout);
  return;
}

void prt_lBuffs(int rank, int size, char* buff, int n) {
  for (int i=0; i<size; i++) {
    if (i==rank) {
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

void prt_var(int rank, int size, int* var, char** lab) {
  printf("[%2d/%2d]: %7s: %6d\n", rank, size, *lab, *var); fflush(stdout);
  return;
}



int get_loVal(int rank, int size, int n) {
  return (((2 +((int)(rank) * (n-1))) / (int)size));
}

int get_hiVal(int rank, int size, int n) {
  return (((1 +((rank+1) * (n-1))) / (int)size));
}

int get_arrLen(int size, int n) {
  return get_len(size-1, size, n);
}

int get_len(int rank, int size, int n) {
  int lo_val = get_loVal(rank, size, n);
  int hi_val = get_hiVal(rank, size, n);
  int len    = hi_val - lo_val + 1;
  assert((len<INT_MAX) && "len is greater than INT_MAX!!");
  return (int)len;
}

void prt_varOrdered(int rank, int size, int* var, char** lab) {
  for (int i=0; i<size; i++) {
    if (i==rank) {
      prt_var(rank, size, var, lab);
    } MPI_Barrier(MPI_COMM_WORLD);
  } MPI_Barrier(MPI_COMM_WORLD);
  if(rank == ROOT) { printf("\n"); fflush(stdout); }
  return;
}


void find_primes(int rank, int size, int n, char* marked) {
  int index;
  int cPrime; // current prime
  int fPrime; // first prime in local array
  int lo_val = get_loVal(rank, size, n);
  int hi_val = get_hiVal(rank, size, n);
  int len    = hi_val - lo_val + 1;
  int lenMX  = get_arrLen(size, n);

#ifdef NBUG
  char* lab = NULL;
  lab = "lo_val"; prt_var(rank, size, &lo_val, &lab); // NBUG
  lab = "hi_val"; prt_var(rank, size, &hi_val, &lab); // NBUG
  lab = "len"; prt_var(rank, size, &len, &lab); // NBUG
  lab = "lenMX"; prt_var(rank, size, &lenMX, &lab); // NBUG
#endif // NBUG

  marked = (char*) calloc(lenMX, sizeof(int));
  assert(marked != NULL);  memset(marked, 0, lenMX);

  if (rank == ROOT) {
    index = 0;
  }
  /* //todo optimization: skip even numbers, cut message array in half */
  cPrime = 2;
  while (cPrime*cPrime <= n) {

    lab = "cPrime"; prt_var(rank, size, &cPrime, &lab); // NBUG

    if (cPrime*cPrime > lo_val) { /* find the fPrime prime multiple in range */
      fPrime = cPrime * cPrime - lo_val;
    } else {
      if ((lo_val % cPrime) == 0) { // not a prime
        fPrime = 0;
      } else {
        fPrime = cPrime - (lo_val % cPrime); // first prime in local array
      }
    }

    /* lab = "fPrime"; prt_var(rank, size, &fPrime, &lab); // NBUG */

    for(int i = fPrime; i < len; i += cPrime) { /* mark prime multiples */
      marked[i] = 1;
    }
    for(int i = len; i<lenMX; i++) { /* mark pad cells at the end of marked arr */
      marked[i] = 2;
    }
    if(rank == 0) {
      while (marked[++index]);
      cPrime = index + 2;
    }

    /*lab = "cPrime"; prt_var(rank, size, &cPrime, &lab); // NBUG */

    if (size > 1) {
      MPI_Bcast(&cPrime,  1, MPI_INT, 0, MPI_COMM_WORLD);
    }
  } // end of while (cPrime*cPrime <= n)


  return;
}


void find_primesOrdered(int rank, int size, int n, char* marked) {
  for (int i=0; i<size; i++) {
    if (i==rank) {
      find_primes(rank, size, n, marked);
    } MPI_Barrier(MPI_COMM_WORLD);
  } MPI_Barrier(MPI_COMM_WORLD);
  if(rank == ROOT) { printf("\n"); fflush(stdout); }
  return;
}

void prt_primesOrdered(int rank, int size, int n, char* marked, int lenMX) {
  for (int i=0; i<size; i++) {
    if (i==rank) {
      prt_primes(rank, size, n, marked, lenMX);
    } MPI_Barrier(MPI_COMM_WORLD);
  } MPI_Barrier(MPI_COMM_WORLD);
  if(rank == ROOT) { printf("\n"); fflush(stdout); }
  return;
}

void prt_primes(int rank, int size, int n, char* marked, int lenMX) {
  int num = 0;
  for (int r=0; r<size; r++) {
    int len = get_len(rank, size, n); // get local arr length
    for(int li=0; li<len; li++) {
      int gi = (r*lenMX) + li; // get global index
#ifdef NBUG
      printf("%2d:%2d:%2d:%2d\n", num, li, gi, marked[gi]); fflush(stdout);
#endif // NBUG
      if(marked[gi] == 0) { // is prime!!
#ifndef NBUG
        printf("%3d ", num);
#endif // NBUG
        num++;
      }
    } printf("\n");
  } printf("\n"); fflush(stdout);
  return;
}

/* eof */
