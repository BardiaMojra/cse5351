/**
  @author Bardia Mojra
  @link https://www.math.tu-cottbus.de/~kd/parallel/mpi/mpi-course.book_133.html
  @link http://mpi.deino.net/mpi_functions/MPI_Cart_shift.html
*/


#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h> // for INT_MAX

/* config */
#define ROOT 0
#define INPUT_LEN 10 // user input cmd buffer size

/* private declarations */
void get_userInput(int rank, int size, int* len);
void prt_var(int rank, int size, int* var, char** lab);
void getInput_arrEntries(int rank, int size, int* arr, int len);
void getInput_arr(int rank, int size, int* arr, int len);
void prt_buff(int rank, int size, int* buff, int len);
void prt_lBuffs(int rank, int size, int* buff, int len);
void getInput_shift(int rank, int size, int len, int* stat, int* shift);
int  get_dest_rank(int rank, int size, int dir);
void SHIFT(int rank, int size, int len,  int shift);

int main (int argc, char ** argv) {
  // char* input = NULL;
  // char* cmd = NULL; // user input buff -- for shift cmd or exit
  char* lab = NULL; //todo NBUG
  int len = 0;
  int* arr = NULL;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == ROOT){
    get_userInput(rank, size, &len);
  }
  MPI_Bcast(&len, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
  lab = "len"; prt_var(rank, size, &len, &lab); //todo remove line (NBUG)
  arr = (int*)calloc(len, sizeof(int));
  assert(arr != NULL);
  memset(arr, 0, len);
  prt_lBuffs(rank, size, arr, len);

  getInput_arrEntries(rank, size, arr, len);
  prt_lBuffs(rank, size, arr, len);

  int shift = 0;
  int stat = 0;  //todo add gracefull exit (low priority)
  while(1) {
    if(rank == ROOT) {
      getInput_shift(rank, size, len, &stat, &shift);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&shift, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    prt_var
    MPI_Barrier(MPI_COMM_WORLD);
    if(shift!=0) {

      lab = "shift"; prt_var(rank, size, &shift, &lab); //todo remove line (NBUG)

      prt_lBuffs(rank, size, arr, len);
      dest = get_dest_rank(rank, size, dir);
      SHIFT(rank, size, dest, shift); // perform circular SHIFT
      prt_lBuffs(rank, size, arr, len);

    }

    if(strcmp(cmd, "shift")) {
      MPI_Barrier(MPI_COMM_WORLD);
      if(rank == ROOT) {
        prt_shift_cmd(rank, size, cmd, dir, disp);
      }

    }
    else if(strcmp(cmd, "exit")) {
      MPI_Barrier(MPI_COMM_WORLD);
      if(rank == ROOT) {
        printf("[%2d/%2d]: program will exit...\n", rank, size); fflush(stdout);
      }
      MPI_Barrier(MPI_COMM_WORLD);
      break;
    } else {
      if(rank == ROOT) {
        printf("[%2d/%2d]: unknown command: %s \n", rank, size, cmd); fflush(stdout);
      }
    } // end of if else
  } // end of while(1)
  free(arr);
  MPI_Finalize();
  return 0;
}

/* private definitions */
void get_userInput(int rank, int size, int* len) {
  printf("[%2d/%2d]: <=> [rank/size]\n", rank, size); fflush(stdout);
  printf("[%2d/%2d]: number of processors 'size': %d\n", rank, size, size); fflush(stdout);
  printf("[%2d/%2d]: note: all arrays will have the same size.\n", rank, size); fflush(stdout);
  printf("[%2d/%2d]: enter array length: \n", rank, size); fflush(stdout);
  scanf("%d", len);
  printf("[%2d/%2d]: next, each processor will ask for its array entries.\n", rank, size); fflush(stdout);
  printf("[%2d/%2d]: note: array entries and the length variable are type int.\n", rank, size); fflush(stdout);
  return;
}



void prt_lBuffs(int rank, int size, int* buff, int len) {
  for (int i=0; i<size; i++) {
    if (i==rank) {
      prt_buff(rank, size, buff, len);
    } MPI_Barrier(MPI_COMM_WORLD);
  } MPI_Barrier(MPI_COMM_WORLD);
  return;
}

void prt_shift_cmd(int rank, int size, char* cmd, int dir, int disp) {
  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == ROOT) {
    printf("[%2d/%2d]: cmd:%s dir:%d  disp:%d \n", rank, size, cmd, dir, disp); fflush(stdout);
  } MPI_Barrier(MPI_COMM_WORLD);
  return;
}

void prt_var(int rank, int size, int* var, char** lab) {

  return;
}

void getInput_arrEntries(int rank, int size, int* arr, int len) {
  for (int i=0; i<size; i++) {
    if (i==rank) {
      getInput_arr(rank, size, arr);
    } MPI_Barrier(MPI_COMM_WORLD);
  } MPI_Barrier(MPI_COMM_WORLD);
  return;
}

void prt_buff(int rank, int size, int* buff, int len) {
  printf("[%2d/%2d]: ", rank, size); fflush(stdout);
  for (int j=0; j<len; j++) {
    printf("%2d ", buff[j]); fflush(stdout);
  } printf("\n"); fflush(stdout);
  return;
}

void getInput_arr(int rank, int size, int* arr, int len) {
  char* input;
  input = (char*) calloc(10, sizeof(char));
  printf("[%2d/%2d]: generate random input integers?[yes/no]\n", rank, size); fflush(stdout);
  scanf("%s", input);
  if strcmp(input, "yes") {
    printf("[%2d/%2d]: will generate random entries.\n", rank, size); fflush(stdout);
    get_randArr(rank, size, arr, len);
  } else if strcmp(input, "no") {
    printf("[%2d/%2d]: will get user entries.\n", rank, size); fflush(stdout);
    get_userArr(rank, size, arr, len);
  } else {
    printf("[%2d/%2d]: unknown input!\n", rank, size); fflush(stdout);
    printf("[%2d/%2d]: will generate random entries.\n", rank, size); fflush(stdout);
    get_randArr(rank, size, arr, len);
  }
  prt_buff(rank, size, arr, len);
  return;
}

void get_randArr(int rank, int size, int* arr, int len) {
  for(int i=0; i<len, i++) {
    printf("[%2d/%2d]: %2d: \n", rank, size, i); fflush(stdout);
    arr[i] = srand(time(NULL) + rank); // scanf("%d", arr[i]);
    printf("[%2d/%2d]: %2d: %2d\n", rank, size, i, arr[i]); fflush(stdout);
  }
  return;
}

void get_userArr(int rank, int size, int* arr, int len) {
  for(int i=0; i<len, i++) {
    printf("[%2d/%2d]: %2d: \n", rank, size, i); fflush(stdout);
    scanf("%d", arr[i]);
    printf("[%2d/%2d]: %2d: %2d\n", rank, size, i, arr[i]); fflush(stdout);
  }
  return;
}

void getInput_shift(int rank, int size, int len, int* stat, int* shift) {

}

int  get_dest_rank(int rank, int size, int dir);
void SHIFT(int rank, int size, int shift);

/* eof */
