#include <omp.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAXSIZE 10000 /* maximum set length */
#define MAXWORKERS 12  /* maximum number of workers */

int numWorkers;
int size;

double start_time, end_time, totalExectime;
int iterations = 100;

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
	#pragma omp parallel for shared(pivotCount)
	for (k = 0; k < currSize; k++)
	{
		if (mainSet->array[k] < pivot)
		{
			#pragma omp critical
			insertArray(&left, mainSet->array[k]);
		}
		else if (mainSet->array[k] > pivot)
		{
			#pragma omp critical
			insertArray(&right, mainSet->array[k]);
		}else{
			#pragma omp critical
			pivotCount++;
		}
	}
	left = quickSort(&left);
	right = quickSort(&right);
	Array pivotAsArr;
	initArray(&pivotAsArr, 1);
	for (k = 0; k < pivotCount;k++){
		insertArray(&pivotAsArr, pivot);
	}
	

	/*	printf("[");
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
	printf("\n");
	*/

	left = joinArrays(&left, &pivotAsArr);
	return joinArrays(&left, &right);
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
		}else{
			pivotCount++;
		}
	}
	left = quickSort(&left);
	right = quickSort(&right);
	Array pivotAsArr;
	initArray(&pivotAsArr, 1);
	for (k = 0; k < pivotCount;k++){
		insertArray(&pivotAsArr, pivot);
	}
	

	/*	printf("[");
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
	printf("\n");
	*/

	left = joinArrays(&left, &pivotAsArr);
	return joinArrays(&left, &right);
}

Array toSortSet;

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

	//loop to get median execution time
	int k;
	int threads = 1;
	double totalExectime = 0;
	double sequentialEx;

	//find the sequential execution of the problem

	/* initialize a test set */
	initArray(&toSortSet, size);
	srand(time(NULL));
	//printf("[");
	for (k = 0; k < size; k++)
	{
		insertArray(&toSortSet, rand() % 10000);
		//printf(" %d", toSortSet.array[i]);
	}

	for (k = 0; k < iterations; k++)
	{
		start_time = omp_get_wtime();
		Array sorted = quickSortSeq(&toSortSet);
		end_time = omp_get_wtime();
		totalExectime += end_time - start_time;
	}
	sequentialEx = totalExectime / iterations;
	printf("%g\n", sequentialEx);
	totalExectime = 0;

	//check perforamance for different amount of threads
	for (threads = 1; threads <= 12;threads++){
		omp_set_num_threads(threads);

		for (k = 0; k < iterations; k++)
		{
			initArray(&toSortSet, size);
			int i, j, total = 0;

			/* initialize the set */
			srand(time(NULL));
			//printf("[");
			for (i = 0; i < size; i++)
			{
				insertArray(&toSortSet, rand() % 10000);
				//printf(" %d", toSortSet.array[i]);
			}
			//printf("]\n");

			start_time = omp_get_wtime();
			Array sorted = quickSort(&toSortSet);
			end_time = omp_get_wtime();
			totalExectime += end_time - start_time;
		}
		//printf("\n %g \n",totalExectime/iterations);
		printf("(%d, %g)",threads, sequentialEx/(totalExectime/iterations));
	}
	printf("\n");
	// final printout

	// printf("[");
	// for (i = 0; i < size; i++)
	// {
	// 	printf(" %d", sorted.array[i]);
	// }
	// printf("]\n");
	
}