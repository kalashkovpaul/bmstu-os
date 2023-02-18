#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>

#define READERS_NUMBER 5
#define WRITERS_NUMBER 3

#define ITERS 7

#define READER_SLEEP 4000
#define WRITER_SLEEP 2000

HANDLE mutex;
HANDLE can_read;
HANDLE can_write;

int waiting_writers = 0;
int waiting_readers = 0;
int active_readers = 0;

int counter = 0;

void start_write(void)
{
    InterlockedIncrement(&waiting_writers);

    if (active_readers > 0)
        WaitForSingleObject(can_write, INFINITE);

    InterlockedDecrement(&waiting_writers);
}

void stop_write(void)
{
    ResetEvent(can_write);
    if (waiting_readers == 0)
        SetEvent(can_write);
    else
        SetEvent(can_read);
}

void start_read(void)
{
    InterlockedIncrement(&waiting_readers);

    if (waiting_writers > 0 && (WaitForSingleObject(can_write, 0) == WAIT_OBJECT_0))
        WaitForSingleObject(can_read, INFINITE);

    WaitForSingleObject(mutex, INFINITE);

    InterlockedDecrement(&waiting_readers);
    SetEvent(can_read);
    InterlockedIncrement(&active_readers);

    ReleaseMutex(mutex);
}

void stop_read(void)
{
    InterlockedDecrement(&active_readers);

    if (active_readers == 0)
    {
       SetEvent(can_write);
    }
}


DWORD WINAPI run_reader(CONST LPVOID lpParams)
{
    int rid = *(int *)lpParams;
    srand(time(NULL) + rid);

    for (size_t i = 0; i < ITERS; i++)
    {
        int stime = rand() % READER_SLEEP;
        Sleep(stime);

        start_read();
        printf("Reader %d - %2d, slept %dms\n", rid,
                    counter, stime);
        stop_read();
    }

    return 0;
}

DWORD WINAPI run_writer(CONST LPVOID lpParams)
{
    int wid = *(int *)lpParams;
    srand(time(NULL) + wid);

    for (size_t i = 0; i < ITERS + 1; i++)
    {
        int stime = rand() % WRITER_SLEEP;
        Sleep(stime);

        start_write();
        counter++;
        printf("Writer %d - %2d, slept %dms\n", wid,
                   counter, stime);
        stop_write();
    }

    return 0;
}

int main()
{
    setbuf(stdout, NULL);

    HANDLE readers_threads[READERS_NUMBER];
    HANDLE writers_threads[WRITERS_NUMBER];

    int readers_ids[READERS_NUMBER];
    int writers_ids[WRITERS_NUMBER];

    if ((mutex = CreateMutex(NULL, FALSE, NULL)) == NULL)
    {
        perror("Failed to CreateMutex");
        return -1;
    }

    if ((can_read = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
    {
        perror("Failed to CreateEvent can_read");
        return -1;
    }

    if ((can_write = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL)
    {
        perror("Failed to CreateEvent can_write");
        return -1;
    }

    for (size_t i = 0; i < READERS_NUMBER; i++)
    {
        readers_ids[i] = i + 1;
        readers_threads[i] = CreateThread(NULL, 0, run_reader, readers_ids + i, 0, NULL);

        if (readers_threads[i] == NULL)
        {
            perror("Failed to CreateThread");
            return -1;
        }
    }

    for (size_t i = 0; i < WRITERS_NUMBER; i++)
    {
        writers_ids[i] = i + 1;
        writers_threads[i] = CreateThread(NULL, 0, run_writer,
                                          writers_ids + i, 0, NULL);

        if (writers_threads[i] == NULL)
        {
            perror("Failed to CreateThread");
            return -1;
        }
    }

    WaitForMultipleObjects(READERS_NUMBER, readers_threads, TRUE, INFINITE);
    WaitForMultipleObjects(WRITERS_NUMBER, writers_threads, TRUE, INFINITE);

    for (int i = 0; i < READERS_NUMBER; i++)
        CloseHandle(readers_threads[i]);

    for (int i = 0; i < WRITERS_NUMBER; i++)
        CloseHandle(writers_threads[i]);

    CloseHandle(mutex);
    CloseHandle(can_read);
    CloseHandle(can_write);

    return 0;
}
