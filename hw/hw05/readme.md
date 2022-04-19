# instructions

## installation

    sudo apt install mpich

## compile and run

    cd ./src
    mpicc main.c -o ../out/hello_world
    mpirun -np 5 ../out/hello-world
