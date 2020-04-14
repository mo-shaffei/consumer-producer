#ifndef CODE_UTILITIES_H
#define CODE_UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL 2

/*************************************************************************************/
union SemaphoreCtrl {
    int val;                /* value for SETVAL */
    struct semid_ds *buf;   /* buffer for IPC_STAT & IPC_SET */
    ushort *array;          /* array for GETALL & SETALL */
    struct seminfo *__buf;  /* buffer for IPC_INFO */
    void *__pad;
};

/*************************************************************************************/

int CreateSemaphore(const int *, int);

int GetSemaphore();

void DestroySemaphore(int);

void SemaphoreUp(int, int);

void SemaphoreDown(int, int);

int GetSemaphoreValue(int, int);

int CreateSharedMem(size_t);

int GetSharedMem();

void DestroySharedMem(int);

void *AttachShardMem(int);

/*************************************************************************************/
/*
 * create sem_count number of semaphores initialized with initial_values array
 * return the semaphore id
*/
int CreateSemaphore(const int *initial_values, int sem_count) {
    key_t key = ftok("ftokfile", 1);
    int sem_id;
    sem_id = semget(key, sem_count, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Error in semget");
        exit(EXIT_FAILURE);
    }

    union SemaphoreCtrl semaphore_ctrl;
    for (int i = 0; i < sem_count; ++i) {
        semaphore_ctrl.val = initial_values[i];
        if (semctl(sem_id, i, SETVAL, semaphore_ctrl) == -1) {
            perror("Error in initializing semaphore");
            exit(EXIT_FAILURE);
        }
    }

    return sem_id;
}

/*************************************************************************************/
/*
 * return the semaphore id of an already existing semaphore
*/
int GetSemaphore() {
    key_t key = ftok("ftokfile", 1);
    int sem_id;
    sem_id = semget(key, 0, 0);
    if (sem_id == -1) {
        perror("Error in semget");
        exit(EXIT_FAILURE);
    }
    return sem_id;
}

/*************************************************************************************/
/*
 * destroy the semaphore with id sem_id
 */
void DestroySemaphore(int sem_id) {
    if (semctl(sem_id, 0, IPC_RMID) == -1)
        perror("Error in deleting semaphore");
}

/*************************************************************************************/
/*
 * increment semaphore with id sem_id and number sem_num by one
 */
void SemaphoreUp(int sem_id, int sem_num) {
    struct sembuf sem_operation;
    sem_operation.sem_num = sem_num;
    sem_operation.sem_op = 1;
    sem_operation.sem_flg = !IPC_NOWAIT;

    if (semop(sem_id, &sem_operation, 1) == -1)
        perror("Error in sem up");
}

/*************************************************************************************/
/*
 * decrement with id sem_id and number sem_num by one, block execution if semaphore value is < 1
 */
void SemaphoreDown(int sem_id, int sem_num) {
    struct sembuf sem_operation;
    sem_operation.sem_num = sem_num;
    sem_operation.sem_op = -1;
    sem_operation.sem_flg = !IPC_NOWAIT;

    if (semop(sem_id, &sem_operation, 1) == -1)
        perror("Error in sem down");
}

/*************************************************************************************/
/*
 * get the semaphore value whose with id sem_id and number sem_num
 */
int GetSemaphoreValue(int sem_id, int sem_num) {
    int value = semctl(sem_id, sem_num, GETVAL);
    if (value == -1)
        perror("Error in semaphore getval");
    return value;
}

/*************************************************************************************/
/*
 * create a shared memory block with the specified size
 * return the shared memory segment id
 */
int CreateSharedMem(size_t size) {
    int key = ftok("ftokfile", 2);
    int shared_mem_id = shmget(key, size, IPC_CREAT | 0666);
    if (shared_mem_id == -1) {
        perror("Error in shmget");
        exit(EXIT_FAILURE);
    }
    return shared_mem_id;
}

/*************************************************************************************/
/*
 * get the shared memory segment id of an already existing segment
 */
int GetSharedMem() {
    int key = ftok("ftokfile", 2);
    int shared_mem_id = shmget(key, 0, 0);
    if (shared_mem_id == -1) {
        perror("Error in shmget");
        exit(EXIT_FAILURE);
    }
    return shared_mem_id;
}

/*************************************************************************************/
/*
 * destroy shared memory segment whose id is shm_id
 */
void DestroySharedMem(int shm_id) {
    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
        perror("Error in deleting shared mem");
}

/*************************************************************************************/
/*
 * attach shared memory segment with id shm_id to the address space of the caller process
 * returns a pointer to the shared memory segment
 */
void *AttachShardMem(int shm_id) {
    void *p = shmat(shm_id, NULL, 0);
    if (p == -1) {
        perror("Error in shmat");
        exit(EXIT_FAILURE);
    }
    return p;
}
/*************************************************************************************/

#endif //CODE_UTILITIES_H
