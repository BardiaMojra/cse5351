# instructions

## MPICH links

[main page](http://www.mpich.org/)

[main page](http://www.mpich.org/)
[tutorials](https://anl.app.box.com/v/2019-06-21-basic-mpi)
[downloads](https://www.mpich.org/downloads/)
[developer documentation](https://wiki.mpich.org/mpich/index.php/Developer_Documentation)

## TACC and stampede2 links

[user guide](https://portal.tacc.utexas.edu/user-guides/stampede2#job-scripts)
[system access](https://portal.tacc.utexas.edu/user-guides/stampede2#access)
[workshops](https://www.youtube.com/channel/UCIyVQ1bICGCggZisXBSSRlw/videos)

## installation

    sudo apt install mpich

## compile and run

    cd ./src
    mpicc main.c -o ../out/hello_world
    mpirun -np 5 ../out/hello-world
