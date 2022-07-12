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
#include <limits.h>

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
void prt_shift_cmd(int rank, int size, char* cmd, int dir, int disp);
void prt_inputOptions(int rank, int size);
void getInput_shift(int rank, int size, char* input, char* cmd, int* dir, int* disp);
int  get_dest_rank(int rank, int size, int dir);
void SHIFT(int rank, int size, int dest, int disp);

int main (int argc, char ** argv) {
  char* input = NULL;
  char* cmd = NULL; // user input buff -- for shift cmd or exit
  char* lab = NULL; //todo NBUG
  int dir, disp;
  int len = 0;
  int* arr = NULL;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (rank == ROOT){
    get_userInput(rank, size, &len);
  }
  MPI_Bcast(&len, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
  lab = "len"; prt_var(rank, size, &len, &lab); //todo remove line
  arr = (int*)calloc(len, sizeof(int));
  assert(arr != NULL);
  memset(arr, 0, len);
  prt_lBuffs(rank, size, arr, len);
  getInput_arrEntries(rank, size, arr, len);
  prt_lBuffs(rank, size, arr, len);


  input = (char*)calloc(INPUT_LEN, sizeof(char));
  cmd   = (char*)calloc(INPUT_LEN, sizeof(char));
  assert(input != NULL);
  assert(cmd != NULL);
  while (1) {
    if(rank == ROOT) {
      prt_inputOptions(rank, size);
      getInput_shift(rank, size, input, cmd, &dir, &disp);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&cmd, INPUT_LEN, MPI_CHAR, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&dir, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&disp, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    if(strcmp(cmd, "shift")) {
      dest = get_dest_rank(rank, size, dir);
      MPI_Barrier(MPI_COMM_WORLD);
      if(rank == ROOT) {
        prt_shift_cmd(rank, size, cmd, dir, disp);
      }
      prt_lBuffs(rank, size, arr, len);
      SHIFT(rank, size, dest, disp); // perform circular SHIFT
      prt_lBuffs(rank, size, arr, len);
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
  free(input);
  free(cmd);
  MPI_Finalize();
  return 0;
}

/* private definitions */
void get_userInput(int rank, int size, int* len) {
  printf("[%2d/%2d]: <=> [rank/size]\n", rank, size);
  printf("[%2d/%2d]: number of processors 'size': %d\n", rank, size, size);
  printf("[%2d/%2d]: note: all arrays will have the same size.\n", rank, size); fflush(stdout);
  printf("[%2d/%2d]: enter array length: \n", rank, size); fflush(stdout);
  scanf("%d", len); fflush(stdout);
  printf("[%2d/%2d]: next, each processor will ask for its array entries.\n", rank, size); fflush(stdout);
  printf("[%2d/%2d]: note: array entries and the length variable are type int.\n", rank, size); fflush(stdout);
  return;
}



void prt_lBuffs(int rank, int size, int* buff, int len) {
  for (int i=0; i<size; i++) {
    if (i==rank) {
      prt_buff(rank, size, buff, len); fflush(stdout);
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
  printf("[%2d/%2d]: ", rank, size);
  for (int j=0; j<len; j++) {
    printf("%2d ", buff[j]);
  } printf("\n"); fflush(stdout);
  return;
}

void getInput_arr(int rank, int size, int* arr, int len);

void prt_inputOptions(int rank, int size);
void getInput_shift(int rank, int size, char* input, char* cmd, int* dir, int* disp);
int  get_dest_rank(int rank, int size, int dir);
void SHIFT(int rank, int size, int dest, int disp);

/* eof */
