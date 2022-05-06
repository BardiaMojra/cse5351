#!/bin/bash
#----------------------------------------------------
# Sample Slurm job script
#   for TACC Stampede2 SKX nodes
#
#   *** MPI Job on SKX Normal Queue ***
#
# Last revised: 20 Oct 2017
#
# Notes:
#
#   -- Launch this script by executing
#      "sbatch skx.mpi.slurm" on Stampede2 login node.
#
#   -- Use ibrun to launch MPI codes on TACC systems.
#      Do not use mpirun or mpiexec.
#
#   -- Max recommended MPI ranks per SKX node: 48
#      (start small, increase gradually).
#
#   -- If you're running out of memory, try running
#      fewer tasks per node to give each task more memory.
#
#----------------------------------------------------

#SBATCH -J test01           # Job name
#SBATCH -o test01_out.o%j       # Name of stdout output file
#SBATCH -e test01_err.e%j       # Name of stderr error file
#SBATCH -p skx-normal      # Queue (partition) name
#SBATCH -N 1               # Total # of nodes
#SBATCH -n 4              # Total # of mpi tasks
#SBATCH -t 00:10:00        # Run time (hh:mm:ss)
#SBATCH --mail-user=mojra@tacc.utexas.edu
#SBATCH --mail-type=all    # Send email at begin and end of job
#SBATCH -A hw05       # Allocation name (req'd if you have more than 1)

# Other commands must follow all #SBATCH directives...
cd $SCRATCH && cd test
pwd
module load ddt_skx intel/19.1.1 gcc/9.1.0  mvapich2/2.3.7 # load required compilers
mpicc  -g -O0 test.c -o test # compile with debug and zero optimization options
date

# Launch MPI code...
ibrun test      # Use ibrun instead of mpirun or mpiexec

# ---------------------------------------------------
