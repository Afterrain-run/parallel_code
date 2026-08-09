#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int thread_count;
pthread_mutex_t mutex;
void *Hello(void* rank);
int shared_counter = 0;
int main(int argc, char* argv[]) {
    long thread;
    pthread_t* thread_handles;

    thread_count = strtol(argv[1], NULL, 10); 
    thread_handles = malloc(thread_count * sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, Hello, (void*)thread);
    }

    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
        for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    // 【唯一缺的这句话：把结果打印出来】
    printf("Final shared counter value: %d\n", shared_counter);

    free(thread_handles);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void *Hello(void* rank) {
    long my_rank = (long)rank;
    printf("Hello from thread %ld of %d\n", my_rank, thread_count);
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&mutex);
        shared_counter+=1;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
