#!/bin/bash
#----------------------------------------------------
# Sample Slurm job script
#   for TACC Stampede2 ICX nodes
#
#   *** MPI Job on ICX Normal Queue ***
#
# Last revised: 09 Mar 2022
#
# Notes:
#
#   -- Launch this script by executing
#      "sbatch icx.mpi.slurm" on Stampede2 login node.
#
#   -- Use ibrun to launch MPI codes on TACC systems.
#      Do not use mpirun or mpiexec.
#
#   -- Max recommended MPI ranks per ICX node: 80
#      (start small, increase gradually).
#
#   -- If you're running out of memory, try running
#      on more nodes using fewer tasks and/or threads
#      per node to give each task access to more memory.
#
#   -- Don't worry about task layout.  By default, ibrun
#      will provide proper affinity and pinning.
#
#   -- You should always run out of $SCRATCH.  Your input
#      files, output files, and exectuable should be
#      in the $SCRATCH directory hierarchy.
#
#----------------------------------------------------

#SBATCH -J myjob           # Job name
#SBATCH -o myjob.o%j       # Name of stdout output file
#SBATCH -e myjob.e%j       # Name of stderr error file
#SBATCH -p icx-normal      # Queue (partition) name
#SBATCH -N 4               # Total # of nodes
#SBATCH -n 320             # Total # of mpi tasks
#SBATCH -t 01:30:00        # Run time (hh:mm:ss)
#SBATCH --mail-user=myname@myschool.edu
#SBATCH --mail-type=all    # Send email at begin and end of job
#SBATCH -A myproject       # Allocation name (req'd if you have more than 1)

# Other commands must follow all #SBATCH directives...

module list
pwd
date

# You should always run out of $SCRATCH.  Your input
#      files, output files, and exectuable should be
#      in the $SCRATCH directory hierarchy.
# Change directories to your $SCRATCH directory where your executable is

cd $SCRATCH

# Launch MPI code...

ibrun ./myprogram         # Use ibrun instead of mpirun or mpiexec

#---------------------------------------------------
