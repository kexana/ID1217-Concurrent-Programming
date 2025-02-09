#include <omp.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAXSIZE 10000 /* maximum set length */
#define MAXWORKERS 12 /* maximum number of workers */

int numWorkers;
int size;

double start_time, end_time, totalExectime;
int iterations = 100; //test iterations

// dynamic array implementation by @casablanca
typedef struct
{
	int *array;
	size_t used;
	size_t size;
} Array;

void initArray(Array *a, size_t initialSize)
{
	a->array = malloc(initialSize * sizeof(int));
	a->used = 0;
	a->size = initialSize;
}

void insertArray(Array *a, int element)
{
	// a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
	// Therefore a->used can go up to a->size
	if (a->used == a->size)
	{
		a->size *= 2;
		a->array = realloc(a->array, a->size * sizeof(int));
	}
	a->array[a->used++] = element;
}

void freeArray(Array *a)
{
	free(a->array);
	a->array = NULL;
	a->used = a->size = 0;
}

// own array join function
Array joinArrays(Array *a, Array *b)
{
	Array tot;
	int sizeA = a->used;
	int sizeB = b->used;
	initArray(&tot, sizeA + sizeB);

	int i;
	for (i = 0; i < sizeA; i++)
	{
		insertArray(&tot, a->array[i]);
	}
	for (i = 0; i < sizeB; i++)
	{
		insertArray(&tot, b->array[i]);
	}

	return tot;
}

Array quickSort(Array *mainSet)
{
	int currSize = mainSet->used;

	if (currSize <= 1)
	{
		return *mainSet;
	}

	int pivot = mainSet->array[currSize / 2];
	int k;

	Array left;
	Array right;
	initArray(&left, currSize / 2 + 1);
	initArray(&right, currSize / 2);

	int pivotCount = 0;
	for (k = 0; k < currSize; k++)
	{
		if (mainSet->array[k] < pivot)
		{
			insertArray(&left, mainSet->array[k]);
		}
		else if (mainSet->array[k] > pivot)
		{
			insertArray(&right, mainSet->array[k]);
		}
		else
		{
			pivotCount++;
		}
	}

	#pragma omp task shared(left)
	{
		left = quickSort(&left);
	}

	#pragma omp task shared(right)
	{
		right = quickSort(&right);
	}
	
	#pragma omp taskwait

	Array pivotAsArr;
	initArray(&pivotAsArr, 1);
	for (k = 0; k < pivotCount; k++)
	{
		insertArray(&pivotAsArr, pivot);
	}

	//debug print
	{/*
		printf("[");
		for (k = 0; k < currSize; k++)
		{
			printf(" %d", mainSet->array[k]);
		}
		printf("]\n");

		printf("[");
		for (k = 0; k < left.used; k++)
		{
			printf(" %d", left.array[k]);
		}
		printf("]  ");

		printf("pivot %d  ", pivot);

		printf("[");
		for (k = 0; k < right.used; k++)
		{
			printf(" %d", right.array[k]);
		}
		printf("] \n");
	*/}
	left = joinArrays(&left, &pivotAsArr);
	left = joinArrays(&left, &right);

	// printf("joined: [");
	// for (k = 0; k < left.used; k++)
	// {
	// 	printf(" %d", left.array[k]);
	// }
	// printf("]\n\n");
	return left;
}

Array quickSortSeq(Array *mainSet)
{
	int currSize = mainSet->used;

	if (currSize <= 1)
	{
		return *mainSet;
	}

	int pivot = mainSet->array[currSize / 2];
	int k;

	Array left;
	Array right;
	initArray(&left, currSize / 2 + 1);
	initArray(&right, currSize / 2);

	int pivotCount = 0;
	for (k = 0; k < currSize; k++)
	{
		if (mainSet->array[k] < pivot)
		{
			insertArray(&left, mainSet->array[k]);
		}
		else if (mainSet->array[k] > pivot)
		{
			insertArray(&right, mainSet->array[k]);
		}
		else
		{
			pivotCount++;
		}
	}
	left = quickSortSeq(&left);
	right = quickSortSeq(&right);
	Array pivotAsArr;
	initArray(&pivotAsArr, 1);
	for (k = 0; k < pivotCount; k++)
	{
		insertArray(&pivotAsArr, pivot);
	}

	//debug print
	/*printf("[");
	for (k = 0; k < currSize; k++)
	{
		printf(" %d", mainSet->array[k]);
	}
	printf("]\n");

	printf("[");
	for (k = 0; k < left.used; k++)
	{
		printf(" %d", left.array[k]);
	}
	printf("]  ");

	printf("pivot %d  ", pivot);

	printf("[");
	for (k = 0; k < right.used; k++)
	{
		printf(" %d", right.array[k]);
	}
	printf("] \n");*/

	left = joinArrays(&left, &pivotAsArr);
	left = joinArrays(&left, &right);

	// printf("joined: [");
	// for (k = 0; k < left.used; k++)
	// {
	// 	printf(" %d", left.array[k]);
	// }
	// printf("]\n\n");

	return left;
}


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

	// loop to get median execution time
	int k,i;
	int threads = 1;
	double totalExectime = 0;
	double sequentialEx;

	// find the sequential execution of the problem
	for (k = 0; k < iterations; k++)
	{
		Array toSortSet;
		initArray(&toSortSet, size);

		/* initialize a test set */
		srand(time(NULL)*k);
		// printf("[");
		for (i = 0; i < size; i++)
		{
			insertArray(&toSortSet, rand() % 10000);
			// printf(" %d", toSortSet.array[i]);
		}
		// printf("]\n");

		start_time = omp_get_wtime();
		Array sorted = quickSortSeq(&toSortSet);
		end_time = omp_get_wtime();
		double currtime = end_time - start_time;
		totalExectime += currtime;
		freeArray(&toSortSet);
	}
	sequentialEx = totalExectime / (double) iterations;
	printf("%g\n", sequentialEx);

	// check perforamance for different amount of threads
	for (threads = 1; threads <= numWorkers; threads++)
	{
		omp_set_num_threads(threads);
		
		totalExectime = 0;
		for (k = 0; k < iterations; k++)
		{
			Array toSortSet;
			initArray(&toSortSet, size);

			/* initialize the set */
			srand(time(NULL)*k);
			//  printf("given: [");
			for (i = 0; i < size; i++)
			{
				insertArray(&toSortSet, rand() % 10000);
				//  printf(" %d", toSortSet.array[i]);
			}
			// printf("]\n");

			Array sorted;
			start_time = omp_get_wtime();
			#pragma omp parallel
			{
				#pragma omp single
				{
					sorted = quickSort(&toSortSet);
				}
			}
			end_time = omp_get_wtime();
			double currtime = end_time - start_time;
			totalExectime += currtime;
			freeArray(&toSortSet);
			// final printout

			// printf("[");
			// for (i = 0; i < size; i++)
			// {
			// 	printf(" %d", sorted.array[i]);
			// }
			// printf("]\n");
		}
		//printf("%g \n",totalExectime/(double)iterations);
		printf("(%d, %g)", threads, sequentialEx / (totalExectime / (double) iterations));
	}
	printf("\n");
	
}