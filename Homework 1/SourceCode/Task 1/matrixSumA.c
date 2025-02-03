/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
			 the total sum from partial sums computed by Workers
			 and prints the total sum to the standard output

   usage under Linux:
	 gcc matrixSum.c -lpthread
	 a.out size numWorkers

*/
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000 /* maximum matrix size */
#define MAXWORKERS 10 /* maximum number of workers */

pthread_mutex_t barrier; /* mutex lock for the barrier */
pthread_mutex_t maxLock; /* mutex lock for the max */
pthread_mutex_t minLock; /* mutex lock for the min */
pthread_cond_t go;		 /* condition variable for leaving */
int numWorkers;			 /* number of workers */
int numArrived = 0;		 /* number who have arrived */

/* a reusable counter barrier */
void Barrier()
{
	pthread_mutex_lock(&barrier);
	numArrived++;
	if (numArrived == numWorkers)
	{
		numArrived = 0;
		pthread_cond_broadcast(&go);
	}
	else
	{
		pthread_cond_wait(&go, &barrier);
	}
	pthread_mutex_unlock(&barrier);
}

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

double start_time, end_time;  /* start and end times */
int size, stripSize;		  /* assume size is multiple of numWorkers */
int sums[MAXWORKERS];		  /* partial sums */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

struct Min {
  int value;
  int xCoordinate;
  int yCoordinate;
};

struct Max {
  int value;
  int xCoordinate;
  int yCoordinate;
};

struct Arguments
{
	struct Min* min;
	struct Max* max;
	long id;
};

void *Worker(void *arg);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[])
{
	int i, j;
	long l; /* use long in case of a 64-bit system */
	pthread_attr_t attr;
	pthread_t workerid[MAXWORKERS];

	struct Min min;
	min.value = 100;
	struct Max max;
	max.value = 0;

	/* set global thread attributes */
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	/* initialize mutex and condition variable */
	pthread_mutex_init(&barrier, NULL);
	pthread_cond_init(&go, NULL);

	/* read command line args if any */
	size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
	numWorkers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
	if (size > MAXSIZE)
		size = MAXSIZE;
	if (numWorkers > MAXWORKERS)
		numWorkers = MAXWORKERS;
	stripSize = size / numWorkers;

	/* initialize the matrix */
	srand(time(NULL));
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			matrix[i][j] = rand() % 99;
		}
	}

	/* print the matrix */
	for (i = 0; i < size; i++)
	{
		printf("[ ");
		for (j = 0; j < size; j++)
		{
			printf(" %d", matrix[i][j]);
		}
		printf(" ]\n");
	}

	/* do the parallel work: create the workers */
	struct Arguments pArg [numWorkers];
	start_time = read_timer();
	for (l = 0; l < numWorkers; l++)
	{
			pArg[l].min = &min;
			pArg[l].max = &max;
			pArg[l].id = l;
		pthread_create(&workerid[l], &attr, Worker, (void*) &pArg[l]);
	}
	pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg)
{
	long myid = (long)((struct Arguments*)arg)->id;
	int total, i, j, first, last;
	struct Max* maxPoint = ((struct Arguments*)arg)->max;
	struct Min* minPoint = ((struct Arguments*)arg)->min;

#ifdef DEBUG
	printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

	/* determine first and last rows of my strip */
	first = myid * stripSize;
	last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

	/* sum values in my strip */
	total = 0;
	for (i = first; i <= last; i++)
	{
		for (j = 0; j < size; j++)
		{
			total += matrix[i][j];
			if(matrix[i][j]>(*maxPoint).value){
				pthread_mutex_lock(&maxLock);
					if(matrix[i][j]>(*maxPoint).value){
						(*maxPoint).value = matrix[i][j];
						(*maxPoint).xCoordinate = i;
						(*maxPoint).yCoordinate = j;
					}
				pthread_mutex_unlock(&maxLock);
			}
			if(matrix[i][j]<(*minPoint).value){
				pthread_mutex_lock(&minLock);
					if(matrix[i][j]<(*minPoint).value){
						(*minPoint).value = matrix[i][j];
						(*minPoint).xCoordinate = i;
						(*minPoint).yCoordinate = j;
					}
				pthread_mutex_unlock(&minLock);
			}
		}
	}
	sums[myid] = total;
	Barrier();
	if (myid == 0)
	{
		total = 0;
		for (i = 0; i < numWorkers; i++)
		{
			total += sums[i];
		}
		/* get end time */
		end_time = read_timer();
		/* print results */
		printf("The total is %d\n", total);
		printf("The largest element is %d at x: %d and y: %d\n", (*maxPoint).value,(*maxPoint).xCoordinate,(*maxPoint).yCoordinate);
		printf("The smallest element is %d at x: %d and y: %d\n", (*minPoint).value,(*minPoint).xCoordinate,(*minPoint).yCoordinate);
		printf("The execution time is %g sec\n", end_time - start_time);
	}
}
