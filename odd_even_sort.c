#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

// 比较并交换函数
void Swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// 合并数组（只留下较小的一半）—— 书本 3.7.4 节核心
void Merge_low(int my_keys[], int recv_keys[], int temp_keys[], int local_n) {
    int m_i = 0, r_i = 0, t_i = 0;
    while (t_i < local_n) {
        if (my_keys[m_i] <= recv_keys[r_i]) {
            temp_keys[t_i] = my_keys[m_i];
            t_i++; m_i++;
        } else {
            temp_keys[t_i] = recv_keys[r_i];
            t_i++; r_i++;
        }
    }
    while (m_i < local_n) {
        temp_keys[t_i] = my_keys[m_i];
        t_i++; m_i++;
    }
    while (r_i < local_n) {
        temp_keys[t_i] = recv_keys[r_i];
        t_i++; r_i++;
    }
    for (m_i = 0; m_i < local_n; m_i++) my_keys[m_i] = temp_keys[m_i];
}
// 合并数组（只留下较大的一半）
void Merge_high(int my_keys[], int recv_keys[], int temp_keys[], int local_n) {
    // 注意这里是从数组的尾巴往前比！
    int m_i = local_n - 1;
    int r_i = local_n - 1;
    int t_i = local_n - 1;
    while (t_i >= 0) {
        if (my_keys[m_i] >= recv_keys[r_i]) {
            temp_keys[t_i] = my_keys[m_i];
            t_i--; m_i--;
        } else {
            temp_keys[t_i] = recv_keys[r_i];
            t_i--; r_i--;
        }
    }
    // 把排好序的较大的一半，复制回 my_keys 里
    for (m_i = 0; m_i < local_n; m_i++) my_keys[m_i] = temp_keys[m_i];
}

// C 语言自带的 qsort 比较函数
int compare_int(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int main(int argc, char* argv[]) {
    int my_rank, comm_sz;
    int n = 16; // 假设我们要排 16 个数
    int local_n;
    int* local_keys; // 每个进程手里的数
    int* temp_keys;  // 临时缓冲

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    local_n = n / comm_sz;
    local_keys = (int*)malloc(local_n * sizeof(int));
    temp_keys = (int*)malloc(local_n * sizeof(int));

    // 0号进程生成乱序数据
    if (my_rank == 0) {
        int full_array[16] = {15, 9, 8, 11, 3, 2, 7, 1, 20, 18, 5, 4, 12, 19, 6, 10};
        printf("Initial (unsorted): ");
        for(int i=0; i<n; i++) printf("%d ", full_array[i]);
        printf("\n");

        // 用 Scatter 把数据分发给每个进程
        MPI_Scatter(full_array, local_n, MPI_INT, local_keys, local_n, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        MPI_Scatter(NULL, local_n, MPI_INT, local_keys, local_n, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // 第一步：每个进程先把自己手里的牌理顺
    qsort(local_keys, local_n, sizeof(int), compare_int);

    // 第二步：开始奇偶循环交换
    int phase;
    for (phase = 0; phase < comm_sz; phase++) {
        int partner;
        if (phase % 2 == 0) { // 偶数阶段
            if (my_rank % 2 == 0) partner = my_rank + 1;
            else partner = my_rank - 1;
        } else { // 奇数阶段
            if (my_rank % 2 != 0) partner = my_rank + 1;
            else partner = my_rank - 1;
        }

        // 边界检查（最两头的进程在特定阶段是空闲的）
        if (partner < 0 || partner >= comm_sz) continue;

        // 核心交换：使用 MPI_Sendrecv 防死锁
        MPI_Sendrecv(local_keys, local_n, MPI_INT, partner, 0,
                     temp_keys, local_n, MPI_INT, partner, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // 决定留大还是留小
        if (my_rank < partner) {
            Merge_low(local_keys, temp_keys, temp_keys, local_n);
        } else {
            // 这里省略了 Merge_high，但逻辑一样，只是保留较大的部分
            // 为了简单演示，这里直接反过来用
            Merge_high(local_keys, temp_keys, temp_keys, local_n); 
        }
    }

    // 最后用 Gather 收回来看结果
    int* final_array = NULL;
    if (my_rank == 0) final_array = (int*)malloc(n * sizeof(int));
    MPI_Gather(local_keys, local_n, MPI_INT, final_array, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("Final (sorted):    ");
        for(int i=0; i<n; i++) printf("%d ", final_array[i]);
        printf("\n");
        free(final_array);
    }

    free(local_keys);
    free(temp_keys);
    MPI_Finalize();
    return 0;
}