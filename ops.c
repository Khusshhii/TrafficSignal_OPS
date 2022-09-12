#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include<windows.h>
#define N 4
#define RED 3
#define YELLOW 1
#define GREEN 0
#define FRONT (phnum+2)% N
#define LEFT (phnum + 3) % N
#define RIGHT (phnum + 1) % N
int state[N], count=0;
int sig[N] = { 0, 1, 2, 3 };
sem_t mutex;
sem_t S[N];
void test(int phnum)
{
    if (count==50)
    exit(0);
    count++;
    if (state[phnum] == YELLOW && state[LEFT] != GREEN && state[FRONT] != GREEN  && state[RIGHT] != GREEN)
    {
        // state that GREEN
        state[phnum] = GREEN;
        sleep(10);
        system("Color 0A");
        printf("SIGNAL %d is GREEN for 10 seconds\n", phnum + 1);
        sem_post(&S[phnum]);
    }
}

void ready(int phnum)
{
    sem_wait(&mutex);
    // state that READY(YELLOW)
    state[phnum] = YELLOW;
    //system("Color 0E");
    printf("SIGNAL %d is YELLOW\n", phnum + 1);

    test(phnum);
    sem_post(&mutex);

    sem_wait(&S[phnum]);
    sleep(1);
}

void stop(int phnum)
{
    sem_wait(&mutex);
    // state that RED
    state[phnum] = RED;
    sleep(10);
    system("Color 0C");
    printf("SIGNAL %d is RED for 10 seconds\n", phnum + 1);

    test(LEFT);
    test(FRONT);
    test(RIGHT);
    sem_post(&mutex);
}
void* tsignal(void* num)
{
    while (1)
    {
        int* i = num;
        sleep(1);
        ready(*i);
        sleep(0);
        stop(*i);
    }
}
int main()
{
    int i;
    pthread_t thread_id[N];
    // initialize the semaphores
    sem_init(&mutex, 0, 1);
    for (i = 0; i < N; i++)
    sem_init(&S[i], 0, 0);
    for (i = 0; i < N; i++)
    {
        pthread_create(&thread_id[i], NULL, tsignal, &sig[i]);
        printf("SIGNAL %d is RED\n", i + 1);
    }
    for (i = 0; i < N; i++)
    pthread_join(thread_id[i], NULL);
}