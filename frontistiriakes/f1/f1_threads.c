#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// global variable: accessible to all threads
long int thread_count;
long int N;

// thread function
void *work(void* rank) {
    long tid = (long) rank;
    long int i, v=0;
    
    for(i=(tid*(N/thread_count))+1; i<=(tid+1)*(N/thread_count); i++)
        v += i;

    printf("thread %ld produces %ld\n", tid, v);
    pthread_exit((void*)v);
    
}

//--------------------------------
int main(int argc, char* argv[]) {
    long t;
    pthread_t* thread_handles;
    long int j, sum=0;
    
    // get number of threads from command line
    if (argc > 1){
        thread_count = atoi(argv[1]);
        N = atoi(argv[2]);
    }           
    
    thread_handles = malloc (thread_count * sizeof(pthread_t));

    // create thread_count independent threads
    for (t = 0; t < thread_count; t++)
        pthread_create(&thread_handles[t], NULL, work, (void*) t);

    sleep(1);
    printf("---Hello from the main thread---\n");

    // wait until threads are completed before main continues
    void* r;
    for (t = 0; t < thread_count; t++) {
        pthread_join(thread_handles[t], &r);

        // 1.
        printf("\tthread %ld returns %ld to the main thread\n", t, (long)r);
        sum +=(long)r;
    }

    // main thread, check total sum
    for(j=N; j>N-(N%thread_count); j--)
        sum += j;

    if(sum==N*(N+1)/2)
        printf("The total sum is %ld\n", sum);

    free(thread_handles);
    return 0;
}