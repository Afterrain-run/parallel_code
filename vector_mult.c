#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int my_rank, comm_sz;
    int n = 4; 
    int A_full[4];
    int B_full[4];
    int local_A;
    int local_B;   

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        A_full[0] = 1;
        A_full[1] = 2;
        A_full[2] = 3;
        A_full[3] = 4;
        B_full[0] = 5;
        B_full[1] = 6;
        B_full[2] = 7;
        B_full[3] = 8;
    }

    MPI_Scatter(A_full, 1, MPI_INT, &local_A, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B_full, 1, MPI_INT, &local_B, 1, MPI_INT, 0, MPI_COMM_WORLD);

    local_A = local_A * local_B;

    MPI_Gather(&local_A, 1, MPI_INT, A_full, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("The gathered array is: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", A_full[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
