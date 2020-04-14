#include "Utilities.h"

int RemoveItem();

void ConsumeItem(int, unsigned int);

int gSemaphoreId = 0, gSharedMemId = 0;

/*************************************************************************************/

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        printf("Arg1 = items/sec\n");
        exit(EXIT_FAILURE);
    }
    gSemaphoreId = GetSemaphore();
    gSharedMemId = GetSharedMem();

    int items_per_sec = atoi(argv[1]); //get rate of consumption from command line arguments
    unsigned int usec_per_item = 1000000 / items_per_sec; //number of microseconds needed to consume one item

    int item;
    while (1) {
        SemaphoreDown(gSemaphoreId, SEM_FULL);
        SemaphoreDown(gSemaphoreId, SEM_MUTEX);
        item = RemoveItem();
        SemaphoreUp(gSemaphoreId, SEM_EMPTY);
        SemaphoreUp(gSemaphoreId, SEM_MUTEX);
        ConsumeItem(item, usec_per_item);
    }
}

/*************************************************************************************/

int RemoveItem() {
    int length = GetSemaphoreValue(gSemaphoreId, SEM_FULL); //get the current number of items in the buffer
    int *pBuffer = (int *) AttachShardMem(gSharedMemId); //attach shared memory
    int item = pBuffer[length]; //remove the last item placed in the buffer
    shmdt(pBuffer); //detach shared memory
    printf("\n/*********************************************************/\n");
    printf("Item %d removed from buffer\n", item);
    return item;
}

void ConsumeItem(int item, unsigned int consumption_time) {
    printf("Consuming item...\n");
    usleep(consumption_time); //sleep until consumption is done
    printf("Item %d consumed.\n", item);
}