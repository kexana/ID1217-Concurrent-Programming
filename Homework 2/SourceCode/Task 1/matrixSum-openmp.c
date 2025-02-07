/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
	 gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
	 ./matrixSum-openmp size numWorkers

*/

#include <omp.h>

double start_time, end_time;

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAXSIZE 10000 /* maximum matrix size */
#define MAXWORKERS 8  /* maximum number of workers */

int numWorkers;
int size;
int matrix[MAXSIZE][MAXSIZE];

int iterations = 100;

struct Min
{
	int value;
	int xCoordinate;
	int yCoordinate;
};

struct Max
{
	int value;
	int xCoordinate;
	int yCoordinate;
};

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[])
{

	/* read command line args if any */
	size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
	numWorkers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
	if (size > MAXSIZE)
		size = MAXSIZE;
	if (numWorkers > MAXWORKERS)
		numWorkers = MAXWORKERS;

	struct Min min;
	min.value = 100;
	struct Max max;
	max.value = 0;

	//loop to get median execution time
	int k;
	int threads = 1;
	double totalExectime = 0;
	double sequentialEx = 8.73899e-08;
	for (threads = 1; threads < 8;threads++){
		omp_set_num_threads(numWorkers);
		for (k = 0; k < iterations; k++)
		{
			int i, j, total = 0;

			/* initialize the matrix */
			srand(time(NULL));
			for (i = 0; i < size; i++)
			{
				// printf("[ ");
				for (j = 0; j < size; j++)
				{
					matrix[i][j] = rand() % 99;
					// printf(" %d", matrix[i][j]);
				}
				// printf(" ]\n");
			}

			start_time = omp_get_wtime();
		#pragma omp parallel for reduction(+ : total) private(j) shared(min,max)
			for (i = 0; i < size; i++){
				for (j = 0; j < size; j++)
				{
					total += matrix[i][j];
					if (matrix[i][j] < min.value)
					{
						min.value = matrix[i][j];
						min.xCoordinate = j;
						min.yCoordinate = i;
					}
					else if (matrix[i][j] > max.value)
					{
						max.value = matrix[i][j];
						max.xCoordinate = j;
						max.yCoordinate = i;
					}
				}
			}
			// implicit barrier

			end_time = omp_get_wtime();
			totalExectime += end_time - start_time;

			
			// printf("the total is %d\n", total);
			// printf("The largest element is %d at x: %d and y: %d\n", max.value,max.xCoordinate,max.yCoordinate);
			// printf("The smallest element is %d at x: %d and y: %d\n", min.value,min.xCoordinate,min.yCoordinate);
		}
		//printf("threads: %d -> speedup is %g seconds\n", threads, sequentialEx / (totalExectime / iterations));
		printf("(%d, %g)", threads, sequentialEx / (totalExectime / iterations));
	}
	printf("\n");
	
}
