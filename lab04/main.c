#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

const int MY_FLAG = IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO;
const int num = 10;
const int count = 15;

int* buf;
int* arr;

int* prod_ptr;
int* cons_ptr;
int* alpha_ptr;

#define CONS (0)
#define PROD (1)
#define BIN (2)

struct sembuf start_prod[2] = {{PROD,-1,0},{BIN,-1,0}};
struct sembuf stop_prod[2]  = {{CONS,1,0},{BIN,1,0}};
struct sembuf start_cons[2] = {{CONS,-1,0},{BIN,-1,0}};
struct sembuf stop_cons[2]  = {{PROD,1,0},{BIN,1,0}};

//TODO: функции создания потребителей и производителей,
//      проверять на ошибки semstl, semop
//      производить алфавит, а не числа (соответственно, char,  а не int),
//          для этого alpha_ptr - указатель на последний записанный, count=26 - количество пустых ячеек
//      3 потребителя, 3 производителя
//      очередь, а не стек - для этого prot_prt, cons_ptr

int main()
{

	key_t mem_key;
	key_t sem_key = ftok("key_file",0);
    if (sem_key == -1)
    {
        printf("ftok error\n");
        return 1;
    }

 	int shmid, semid, status;

	if ((shmid = shmget(mem_key, (num + 1)*sizeof(int), MY_FLAG)) == -1)
	{
		printf("shmget error\n");
		return 1;
	}

	arr = shmat(shmid, 0, 0);
	buf = arr + sizeof(int);

	(*arr) = 0;

	if (*buf == -1)
	{
		printf("shmat error\n");
		return 1;
	}

	if ((semid = semget(sem_key, 3, MY_FLAG)) == -1)
	{
		printf("semget error\n");
		return 1;
	}

	semctl(semid, 2, SETVAL, 0); // TODO: проверять на ошибку
	semctl(semid, 1, SETVAL, num);
	semctl(semid, 2, SETVAL, 1);

	srand(time(NULL));

	pid_t pid;
	if ((pid = fork()) == -1)
	{
		printf("fork error\n");
		return 1;
	}

	int k = 0;

    alpha_ptr = buf;

	if (pid != 0)
	{
	    while(k < count)
		{
			semop(semid, start_prod, 2);
            *alpha_ptr = 1 + rand() % 10;
            printf("\t Producting: %d - %d\n", k, *alpha_ptr);
            alpha_ptr += sizeof(int);
			semop(semid, stop_prod, 2);
			sleep(rand() % 2);
			k++;
		}
        if (shmdt(arr) == -1)
		{
			printf("shmdt error\n");
			return 1;
		}
        wait(&status);
    } else {
		while (k < count)
		{
			semop(semid, start_cons, 2);

            alpha_ptr -= sizeof(int);
            printf("Consuming  %d - %d\n", k, *alpha_ptr);
			semop(semid, stop_cons, 2);
			sleep(rand() % 2);
			k++;
		}
	}
	return 0;
}