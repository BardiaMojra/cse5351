/* not sieving to solve for primes */

#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( int argc, char *argv[]);
int prime_number(int n, int id, int p);

int main ( int argc, char *argv[] )
{
  int id;
  int n;
  int n_factor;
  int n_hi;
  int n_lo;
  int p;
  int primes;
  int primes_part;
  double wtime;

  n_lo = 1;
  n_hi = 262144;
  n_factor = 2;

  MPI_Init ( &argc, &argv );
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  if ( id == 0 )
  {
    printf("\nPRIME_MPI\n");
    printf("C/MPI version\n\n
            An MPI example program to count the number of primes.\n");
    printf("The number of processes is %d\n\n", p);
    printf("         N        Pi          Time\n\n");
  }

  n = n_lo;
  while(n <= n_hi)
  {
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    primes_part = prime_number(n,id,p);
    MPI_Reduce(&primes_part, &primes, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (id == 0)
    {
      printf ("  %8d  %8d  %d \n", n, primes, 0);
    }
    n = n*n_factor;
  }

  MPI_Finalize();

  if(id == 0)
  {
    printf("\n");
    printf("PRIME_MPI - Master process:\n");
    printf("Normal end of execution.\n");
  }

  return 0;
}

int prime_number(int n, int id, int p)
  /*
    Purpose:
    PRIME_NUMBER returns the number of primes between 1 and N.
    Discussion:
      In order to divide the work up evenly among P processors, processor
      ID starts at 2+ID and skips by P.
      A naive algorithm is used.
      Mathematica can return the number of primes less than or equal to N
      by the command PrimePi[N].

                  N  PRIME_NUMBER
                  1           0
                 10           4
                100          25
              1,000         168
             10,000       1,229
            100,000       9,592
          1,000,000      78,498
         10,000,000     664,579
        100,000,000   5,761,455
      1,000,000,000  50,847,534

    Parameters:
      Input, int N, the maximum number to check.
      Input, int ID, the ID of this process,
      between 0 and P-1.
      Input, int P, the number of processes.
      Output, int PRIME_NUMBER, the number of prime numbers up to N.
  */
{
  int prime;
  int total = 0;

  for (int i = 2+id; i <= n; i=i+p ) {
    prime = 1;
    for (int j=2; j < i; j++) {
      if ((i%j) == 0)
      {
        prime = 0;
        break;
      }
    }
    total = total + prime;
  }
  return total;
}
