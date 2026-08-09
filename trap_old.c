#include <stdio.h>
#include <mpi.h>

double f(double x) {
    return x * x;
}

double Trap(double left_endpt, double right_endpt, long long trap_count, double base_len) {
    double estimate, x;
    long long i;
    estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
    for (i = 1; i <= trap_count - 1; i++) {
        x = left_endpt + i * base_len;
        estimate += f(x);
    }
    estimate = estimate * base_len;
    return estimate;
}

int main(int argc, char* argv[]) {
    int my_rank, comm_sz;
    long long n, local_n;
    double a = 0.0, b = 100.0, h, local_a, local_b;
    double local_int, total_int;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    n = 10000000000LL; // 100亿次

    h = (b - a) / (double)n;
    local_n = n / comm_sz;
    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;
    local_int = Trap(local_a, local_b, local_n, h);

    MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // 【注意这里没有打印语句！】

    MPI_Finalize();
    return 0;
}
