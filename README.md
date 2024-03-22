# Game-of-life-MPI
During first semester at Master's course one of the tasks was to implement a game of life with some tweaks such as added pollution to the map, with Parallelization using MPI

Main.cpp is written to present how the task was performed by sequential and parallel implementation there are performance variables such as:

MPI Size, Total cells number, used RAM, border size etc.

To test this program you first have to compile it using: `mpiCC -O2 Alloc.cpp Life.cpp LifeParallelImplementation.cpp LifeSequentialImplementation.cpp Main.cpp Rules.cpp SimpleRules.cpp -o <name>` then you just start it `./<name>`. IMPORTANT! You have to insstall mpi to be able to test it!

The goal was to prepare parallelized implementation and updated main, sequential implementation with the rest of the project was prepared by PhD Piotr Oramus
