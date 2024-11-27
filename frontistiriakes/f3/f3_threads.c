#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5

//Buffer structure
typedef struct{
    int items[BUFFER_SIZE];
    int in; //index for insert
    int out; //index for remove
    pthread_mutex_t mutex; //mutual exclusion
    sem_t full; //counts full slots in buffer
    sem_t empty; //counts emplty slots in buffer
}Buffer;

Buffer buffer;

void *producer(void *rank);
int produce_item();
void *consumer(void *rank);
void insert_item(Buffer *buf, int item);
int remove_item(Buffer *buf);

int main(){
    //Initialize buffer and synchronization sem/mutex
    buffer.in=0;
    buffer.out=0;
    pthread_mutex_init(&buffer.mutex, NULL);
    sem_init(&buffer.full, 0, 0);
    sem_init(&buffer.empty, 0, BUFFER_SIZE);

    //create producer, consumer threads
    pthread_t t1_producer, t2_producer, t1_consumer, t2_consumer;
    pthread_create(&t1_producer, NULL, producer, NULL);
    pthread_create(&t2_producer, NULL, producer, NULL);
    pthread_create(&t1_consumer, NULL, consumer, NULL);
    pthread_create(&t2_consumer, NULL, consumer, NULL);

    //join threads
    pthread_join(t1_producer, NULL);
    pthread_join(t2_producer, NULL);
    pthread_join(t1_consumer, NULL);
    pthread_join(t2_consumer, NULL);

    //clean up
    pthread_mutex_destroy(&buffer.mutex);
    sem_destroy(&buffer.full);
    sem_destroy(&buffer.empty);

    return 0;
}

//producer
void *producer(void* rank){
    for(int i=0; i<10; i++){

        //produce item
        int item=produce_item();

        //wait for empty slot
        sem_wait(&buffer.empty);

        //insert into buffer
        pthread_mutex_lock(&buffer.mutex);
        insert_item(&buffer, item);
        printf("produced --> item %d\n", item);
        pthread_mutex_unlock(&buffer.mutex);

        //signal for full slot
        sem_post(&buffer.full);

    }

    pthread_exit(NULL);
}

int produce_item(){
    int item;
    item = rand() % 100;
    return item;
}

//consumer
void *consumer(void* rank){
    for(int i=0; i<10; i++){

        //wait for full slot
        sem_wait(&buffer.full);

        //remove from buffer
        pthread_mutex_lock(&buffer.mutex);
        int item = remove_item(&buffer);
        printf("---------consumed --> item %d\n", item);
        pthread_mutex_unlock(&buffer.mutex);

        //signal for empty slot
        sem_post(&buffer.empty);

    }

    pthread_exit(NULL);
}

void insert_item(Buffer *buf, int item){
    buf->items[buf->in] = item;
    buf->in = (buf->in +1) % BUFFER_SIZE;
}

int remove_item(Buffer *buf){
    int item = buf->items[buf->out];
    buf->out = (buf->out +1) % BUFFER_SIZE;
    return item;
}