#include "Utilities.h"

int ProduceItem(unsigned int);

void InsertItem(int);

void InterruptHandler(int);

int gSemaphoreId = 0, gSharedMemId = 0;

/*************************************************************************************/

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Error: Invalid number of arguments\n");
        printf("Arg1 = items/sec, and arg2 = buffer length\n");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, InterruptHandler);

    int buffer_length = atoi(argv[2]); //get buffer length from command line arguments
    int sem_initials[3] = {1, buffer_length, 0}; //values to initialize the three semaphores
    gSemaphoreId = CreateSemaphore(sem_initials, 3); //create three semaphores with the specified initials
    gSharedMemId = CreateSharedMem(buffer_length * sizeof(int)); //create the bounded buffer block

    int items_per_sec = atoi(argv[1]); //get rate of producing from command line arguments
    unsigned int usec_per_item = 1000000 / items_per_sec; //number of microseconds needed to produce one item

    int item;
    while (1) {
        item = ProduceItem(usec_per_item);
        SemaphoreDown(gSemaphoreId, SEM_EMPTY); //decrement number of empty slots/wait until there is an empty slot
        SemaphoreDown(gSemaphoreId, SEM_MUTEX); //wait on sem mutex to enter critical region
        InsertItem(item); //insert produced item on the buffer
        SemaphoreUp(gSemaphoreId, SEM_FULL); //increment number of full slots
        SemaphoreUp(gSemaphoreId, SEM_MUTEX); //leave critical region
    }
}

/*************************************************************************************/

int ProduceItem(unsigned int production_time) {
    printf("\n/*********************************************************/\n");
    printf("Producing item...\n");
    int item = GetSemaphoreValue(gSemaphoreId, SEM_EMPTY); //item is the number of empty slots at the current time
    usleep(production_time); //sleep until production is done
    printf("Item %d produced.\n", item);
    return item;
}

void InsertItem(int item) {
    int length = GetSemaphoreValue(gSemaphoreId, SEM_FULL); //get the current number of items in the buffer
    int *pBuffer = (int *) AttachShardMem(gSharedMemId); //attach shared memory
    pBuffer[length] = item; //place item in the first empty slot in the buffer
    shmdt(pBuffer); //detach shared memory
    printf("Item %d placed in buffer\n", item);
}

void InterruptHandler(int signum) {
    if (gSemaphoreId)
        DestroySemaphore(gSemaphoreId);
    if (gSharedMemId)
        DestroySharedMem(gSharedMemId);
    exit(EXIT_SUCCESS);
}

