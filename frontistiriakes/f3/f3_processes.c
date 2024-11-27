#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

#define BUFFER_SIZE 5

//Buffer structure
typedef struct{
    int items[BUFFER_SIZE];
    int in; //index for insert
    int out; //index for remove
    sem_t mutex; //mutual exclusion
    sem_t full; //counts full slots in buffer
    sem_t empty; //counts emplty slots in buffer
}Buffer;

Buffer *buffer;

void producer(int rank);
int produce_item();
void consumer(int rank);
void insert_item(Buffer *buf, int item);
int remove_item(Buffer *buf);

int main(){
    //create shared memory
    buffer = mmap(NULL, sizeof(Buffer), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    //Initialize buffer and synchronization sem
    buffer->in=0;
    buffer->out=0;
    sem_init(&buffer->mutex, 1, 1);
    sem_init(&buffer->full, 1, 0);
    sem_init(&buffer->empty, 1, BUFFER_SIZE);

    //create producer, consumer processes
    pid_t ppid1, ppid2, cpid1, cpid2;

    if((ppid1=fork()) == 0){
        producer(1);
        exit(0);
    }
    else if((ppid2=fork()) == 0){
        producer(2);
        exit(0);
    }
    else if((cpid1=fork()) == 0){
        consumer(1);
        exit(0);
    }
    else if((cpid2=fork()) == 0){
        consumer(2);
        exit(0);
    }

    //wait all processes
    waitpid(ppid1, NULL, 0);
    waitpid(ppid2, NULL, 0);
    waitpid(cpid1, NULL, 0);
    waitpid(cpid2, NULL, 0);

    //clean up
    munmap(buffer, sizeof(Buffer));
    sem_destroy(&buffer->mutex);
    sem_destroy(&buffer->full);
    sem_destroy(&buffer->empty);

    return 0;
}

//producer
void producer(int rank){
    for(int i=0; i<10; i++){

        //produce item
        int item=produce_item();

        //wait for empty slot
        sem_wait(&buffer->empty);

        //insert into buffer
        sem_wait(&buffer->mutex);
        insert_item(buffer, item);
        printf("produced --> item %d\n", item);
        sem_post(&buffer->mutex);

        //signal for full slot
        sem_post(&buffer->full);

    }
}

int produce_item(){
    int item;
    item = rand() % 100;
    return item;
}

//consumer
void consumer(int rank){
    for(int i=0; i<10; i++){

        //wait for full slot
        sem_wait(&buffer->full);

        //remove from buffer
        sem_wait(&buffer->mutex);
        int item = remove_item(buffer);
        printf("---------consumed --> item %d\n", item);
        sem_post(&buffer->mutex);

        //signal for empty slot
        sem_post(&buffer->empty);

    }
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