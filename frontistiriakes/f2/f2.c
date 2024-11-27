#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int a1,a2,b1,b2,c1,c2,y,z,x,w;
sem_t sem1, sem2, sem3; 

void* f1()
{
    sem_wait(&sem2);
    a1=10;
    a2=11;
    y=a1+c1;
//  printf("y=%d\n", y);
    sem_post(&sem3);

    sem_wait(&sem2);
    printf("x=%d\n", x);
    sem_post(&sem2);
}

void* f2()
{
    sem_wait(&sem3);
    b1=20;
    b2=21;
    w=b2+c2;
    sem_post(&sem1);

    sem_wait(&sem3);
    x=z-y+w;
//  printf("w=%d   x=%d\n", w,x);
    sem_post(&sem2);
}

void* f3()
{
    sem_wait(&sem1);
    c1=30;
    c2=31;
    sem_post(&sem2);

    sem_wait(&sem1);
    z=a2+b1;
//  printf("z=%d\n", z);
    sem_post(&sem3);
}

int main()
{
    //initialize an UNNAMED semaphore to 1
    sem_init(&sem1, 0, 1);
    sem_init(&sem2, 0, 0);
    sem_init(&sem3, 0, 0);
    pthread_t thread1, thread2, thread3;

    //create independent threads to execute f1, f2
    pthread_create(&thread1, NULL, f1, NULL);
    pthread_create(&thread2, NULL, f2, NULL);
    pthread_create(&thread3, NULL, f3, NULL);

    //wait until threads are completed before main thread continues
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    //destroy the UNNAMED semaphore
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);
    return 0;
}