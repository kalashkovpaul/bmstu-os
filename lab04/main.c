#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE_BUF 256
#define PRODUCERS_AMOUNT 3
#define CONSUMERS_AMOUNT 3
#define PERMS S_IRWXU | S_IRWXG | S_IRWXO

int shmid, semid;
char* shared_buffer;

char* prod_ptr;
char* cons_ptr;
char* alpha_ptr;

#define SB 0
#define SE 1
#define SF 2

#define P -1
#define V 1

struct sembuf start_produce[2] = { {SE, P, 0}, {SB, P, 0} };
struct sembuf stop_produce[2] =  { {SB, V, 0}, {SF, V, 0} };
struct sembuf start_consume[2] = { {SF, P, 0}, {SB, P, 0} };
struct sembuf stop_consume[2] =  { {SB, V, 0}, {SE, V, 0} };

void producer(const int semid)
{
	while (1)
	{
        int sem_op_p = semop(semid, start_produce, 2);
        if (sem_op_p == -1)
        {
            perror("semop error\n");
            exit(1);
        }

        shared_buffer[*prod_ptr] = *alpha_ptr;
        printf("Producer %d - %c\n", getpid(), shared_buffer[*prod_ptr]);

        (*prod_ptr)++;
        (*alpha_ptr)++;

        sleep(rand() % 3);

        int sem_op_v = semop(semid, stop_produce, 2);
        if (sem_op_v == -1)
        {
            perror("semop error\n");
            exit(1);
        }
    }
}

void consumer(const int semid)
{
	while(1)
	{
        int sem_op_p = semop(semid, start_consume, 2);
        if (sem_op_p == -1)
        {
            perror("semop error\n");
            exit(1);
        }

        printf("Consumer %d - %c\n", getpid(), shared_buffer[*cons_ptr]);
        (*cons_ptr)++;

        sleep(rand() % 3);

        int sem_op_v = semop(semid, stop_consume, 2);
        if (sem_op_v == -1)
        {
            perror("semop error\n");
            exit(1);
        }
    }
}

void create_producers() {
    int pid = -1;

	for (int i = 0; i < PRODUCERS_AMOUNT; i++)
    {
        pid = fork();
        if (pid == -1)
        {
            perror("fork error\n");
            exit(1);
        }

        if (pid == 0)
        {
            producer(semid);
            return;
        }
	}
}

void create_consumers() {
    int pid = -1;

    for (int i = 0; i < CONSUMERS_AMOUNT; i++)
    {
        pid = fork();
        if (pid == -1)
        {
            perror("fork error\n");
            exit(1);
        }

        if (pid == 0)
        {
            consumer(semid);
            return;
        }
	}
}

int main()
{
    key_t sem_key = ftok("key_file1",0);
    if (sem_key == -1)
    {
        printf("ftok error\n");
        return 1;
    }

    key_t mem_key = ftok("key_file2",0);
    if (mem_key == -1)
    {
        printf("ftok error\n");
        return 1;
    }

	if ((shmid = shmget(mem_key, (SIZE_BUF + 3) * sizeof(char), IPC_CREAT | PERMS)) == -1)
	{
		perror("shmget error\n");
		exit(1);
	}

	prod_ptr = shmat(shmid, NULL, 0);
	if (prod_ptr == (void*) -1)
	{
		perror("shmat error\n");
		exit(1);
	}

	cons_ptr = prod_ptr + sizeof(char);
    alpha_ptr = prod_ptr + 2 * sizeof(char);
	shared_buffer = prod_ptr + 3 * sizeof(char);

	(*prod_ptr) = 0;
	(*cons_ptr) = 0;
    (*alpha_ptr) = 'a';

	if ((semid = semget(sem_key, 3, IPC_CREAT | PERMS)) == -1)
	{
		perror("semget error\n");
		exit(1);
	}

	int c_sb = semctl(semid, SB, SETVAL, 1);
	int c_se = semctl(semid, SE, SETVAL, SIZE_BUF);
	int c_sf = semctl(semid, SF, SETVAL, 0);

	if (c_se == -1 || c_sf == -1 || c_sb == -1)
	{
		perror("semctl error\n");
		exit(1);
	}

    create_producers();
    create_consumers();

	for (int i = 0; i < (CONSUMERS_AMOUNT + PRODUCERS_AMOUNT); i++)
		wait(NULL);

	if (shmdt(prod_ptr) == -1)
	{
		perror("shmdt error\n");
		exit(1);
	}
}