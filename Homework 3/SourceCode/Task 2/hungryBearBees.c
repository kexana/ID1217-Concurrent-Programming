#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <unistd.h>
#define SHARED 1
#define MAXHONEYJAR_CAP 10000 /* maximum jar capacity */
#define MAXBEES 10            /* maximum number of bees */

sem_t jarFull, beeWork;

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
int numBees;
int jarCapacity;

void *Bees(void *arg);
void *Bear(void *arg);

struct Arguments
{
    long id;
    int *jarAmount;
};

int main(int argc, char *argv[])
{

    pthread_attr_t attr;
    pthread_t beesId[MAXBEES];
    pthread_t bearId;

    /* set global thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    sem_init(&jarFull, SHARED, 0); /* sem the jar is full = 0 */
    sem_init(&beeWork, SHARED, 1); /* sem bee can work = 1 */

    jarCapacity = (argc > 1) ? atoi(argv[1]) : MAXHONEYJAR_CAP;
    numBees = (argc > 2) ? atoi(argv[2]) : MAXBEES;
    if (jarCapacity > MAXHONEYJAR_CAP)
        jarCapacity = MAXHONEYJAR_CAP;
    if (numBees > MAXBEES)
        numBees = MAXBEES;


    int honeyJarContence = 0; // how full the honey jar is
    long i;                   /* use long in case of a 64-bit system */
    struct Arguments beeArg[numBees];

    start_time = read_timer();
    pthread_create(&bearId, &attr, Bear, (void *)&honeyJarContence);
    for (i = 0; i < numBees; i++)
    {
        beeArg[i].id = i;
        beeArg[i].jarAmount = &honeyJarContence;
        pthread_create(&beesId[i], &attr, Bees, (void *)&beeArg[i]);
    }
    pthread_join(bearId, NULL);
    for (i = 0; i < numBees; i++){
		//wait for threads to terminate before continuing 
        pthread_join(beesId[i],NULL);
	}
    end_time = read_timer();
}

int end = 0;
int itterations = 2;

void *Bees(void *arg)
{
    while (!end)
    {
        // add to jar enter critical section
        sem_wait(&beeWork);
        int curr = ++*(((struct Arguments *)arg)->jarAmount);
        printf("bee id %d added to the jar, current: %d\n", (int)((struct Arguments *)arg)->id, curr);

        //when full do not continue beework, but let bear deal with it
        if (curr >= jarCapacity)
        {
            printf("bee id %d filled the jar\n", (int)((struct Arguments *)arg)->id);
            sem_post(&jarFull);
        }else{
            sem_post(&beeWork);
        }
        //simulate work being done and also help with thread fairness
        usleep(1000);
    }
    pthread_exit(NULL);
}

void *Bear(void *arg)
{
    while (!end)
    {
        sem_wait(&jarFull);
        *(((int *)arg)) = 0;
        printf("the bear ate the jar\n");
        itterations = itterations - 1;
        if (itterations <= 0)
        {
            end = 1;
        }
        //signal that bear is finished and the bees can resume
        sem_post(&beeWork);
    }
    pthread_exit(NULL);
}