#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int my_rank, comm_sz;
    int data = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        data = 100;
        printf("Process %d set data to 100, starting broadcast...\n", my_rank);
    }

    MPI_Bcast(&data, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // 注意：这里用的是变量 %d，千万不能写死成 0！
    printf("Process %d received data = %d\n", my_rank, data);

    MPI_Finalize();
    return 0;
}
