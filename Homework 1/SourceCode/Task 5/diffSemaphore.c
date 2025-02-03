#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#define SHARED 1
 
//Diff file implementation using the semaphores library.

sem_t empty, full;
 
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
 
void *Producer(void *arg);
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
    pthread_t producerid;
    pthread_t consumerid;
 
    /* set global thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
 
    sem_init(&empty, SHARED, 1); /* sem empty = 1 */
    sem_init(&full, SHARED, 0);  /* sem full = 0 */
 
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
    pthread_create(&producerid, &attr, Producer, (void *)&dArg);
    pthread_create(&consumerid, &attr, Consumer, (void *)&dArg);
    pthread_join(producerid, NULL);
    pthread_join(consumerid, NULL);
    end_time = read_timer();
 
    printf("The execution time is %g sec\n", end_time - start_time);
    fclose(file1);
    fclose(file2);
}
 
int filesEnd = 0;
 
void *Producer(void *arg)
{
    while (!filesEnd)
    {
        sem_wait(&empty);
        char *statusf1 = fgets(((struct Arguments *)arg)->linef1, 100, ((struct Arguments *)arg)->file1);
        char *statusf2 = fgets(((struct Arguments *)arg)->linef2, 100, ((struct Arguments *)arg)->file2);
        if (!statusf1 && !statusf2)
        {
            filesEnd = 1;
            memset((((struct Arguments *)arg)->linef1), 0, strlen((((struct Arguments *)arg)->linef1)));
            memset((((struct Arguments *)arg)->linef2), 0, strlen((((struct Arguments *)arg)->linef2)));
        }
        else
        {
            if (!statusf1)
            {
                memset((((struct Arguments *)arg)->linef1), 0, strlen((((struct Arguments *)arg)->linef1)));
            }
            else if (!statusf2)
            {
                memset((((struct Arguments *)arg)->linef2), 0, strlen((((struct Arguments *)arg)->linef2)));
            }
        }
        sem_post(&full);
    }
    // pthread_exit(NULL);
}
 
void *Consumer(void *arg)
{
    while (!filesEnd)
    {
        sem_wait(&full);
        if ((!(((struct Arguments *)arg)->linef1) && !((struct Arguments *)arg)->linef2))
        {
            filesEnd = 1;
        }
        else if (strcmp(((struct Arguments *)arg)->linef1, ((struct Arguments *)arg)->linef2) != 0)
        {
            char path1[100];
            char path2[100];
            strcpy(path1, ((struct Arguments *)arg)->linef1);
            strcpy(path2, ((struct Arguments *)arg)->linef2);
            path1[strcspn(path1, "\n")] = 0;
            path2[strcspn(path2, "\n")] = 0;
            printf("f1: %s -> f2: %s\n", path1, path2);
        }
        sem_post(&empty);
    }
 
    // pthread_exit(NULL);
}
 