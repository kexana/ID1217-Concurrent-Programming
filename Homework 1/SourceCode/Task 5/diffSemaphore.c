#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#define SHARED 1

// Diff file implementation using the semaphores library.

sem_t empty1, empty2, full1, full2;

/* timer */
double read_timer()
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if (!initialized)
    {
        gettimeofday(&start, NULL);
        initialized = true;
    }
    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */

void *ProducerF1(void *arg);
void *ProducerF2(void *arg);
void *Consumer(void *arg);

struct Arguments
{
    FILE *file1;
    FILE *file2;
    char *linef1;
    char *linef2;
};

int main(int argc, char *argv[])
{

    pthread_attr_t attr;
    pthread_t producerf1id;
    pthread_t producerf2id;
    pthread_t consumerid;

    /* set global thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    sem_init(&empty1, SHARED, 1); /* sem empty = 1 */
    sem_init(&empty2, SHARED, 1); /* sem empty = 1 */
    sem_init(&full1, SHARED, 0);  /* sem full = 0 */
    sem_init(&full2, SHARED, 0);  /* sem full = 0 */

    FILE *file1;
    FILE *file2;
    char path1[20];
    char path2[20];
    if (argc > 1)
    {
        strcpy(path1, argv[1]);
    }
    if (argc > 2)
    {
        strcpy(path2, argv[2]);
    }
    strcat(path1, ".txt");
    strcat(path2, ".txt");
    file1 = (path1) ? fopen(path1, "r") : NULL;
    file2 = (path2) ? fopen(path2, "r") : NULL;

    char f1String[100];
    char f2String[100];

    struct Arguments dArg;

    dArg.file1 = file1;
    dArg.file2 = file2;
    dArg.linef1 = f1String;
    dArg.linef2 = f2String;

    start_time = read_timer();
    pthread_create(&producerf1id, &attr, ProducerF1, (void *)&dArg);
    pthread_create(&producerf2id, &attr, ProducerF2, (void *)&dArg);
    pthread_create(&consumerid, &attr, Consumer, (void *)&dArg);
    pthread_join(producerf1id, NULL);
    pthread_join(producerf2id, NULL);
    pthread_join(consumerid, NULL);
    end_time = read_timer();

    printf("The execution time is %g sec\n", end_time - start_time);
    fclose(file1);
    fclose(file2);
}

int filesEnd = 0;

void *ProducerF1(void *arg)
{
    int linecounter = 0;
    while (!filesEnd)
    {
        sem_wait(&empty1);
        char *statusf1 = fgets(((struct Arguments *)arg)->linef1, 100, ((struct Arguments *)arg)->file1);
        if (!statusf1)
        {
            memset((((struct Arguments *)arg)->linef1), 0, strlen((((struct Arguments *)arg)->linef1)));
        }
        sem_post(&full1);
    }
    // pthread_exit(NULL);
}

void *ProducerF2(void *arg)
{
    int linecounter = 0;
    while (!filesEnd)
    {
        sem_wait(&empty2);
        char *statusf2 = fgets(((struct Arguments *)arg)->linef2, 100, ((struct Arguments *)arg)->file2);
        if (!statusf2)
        {
            memset((((struct Arguments *)arg)->linef2), 0, strlen((((struct Arguments *)arg)->linef2)));
        }
        sem_post(&full2);
    }
    // pthread_exit(NULL);
}

void *Consumer(void *arg)
{
    while (!filesEnd)
    {
        sem_wait(&full1);
        sem_wait(&full2);

        char path1[100];
        char path2[100];
        strcpy(path1, ((struct Arguments *)arg)->linef1);
        strcpy(path2, ((struct Arguments *)arg)->linef2);

        if (*path1 == 0 && *path2 == 0)
        {
            filesEnd = 1;
        }

        //cache the buffers and release the semaphore for the next read
        sem_post(&empty2);
        sem_post(&empty1);
          
        if (strcmp(path1, path2) != 0)
        {
            path1[strcspn(path1, "\n")] = 0;
            path2[strcspn(path2, "\n")] = 0;
            printf("f1: %s -> f2: %s\n", path1, path2);
        }
    }

    // pthread_exit(NULL);
}
