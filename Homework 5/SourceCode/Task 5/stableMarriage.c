#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int womanAccepts = 0;

void genRanks(int array[], int length, int min, int max, int k)
{
    int nameLen;
    char processorName[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(processorName, &nameLen);
    srand(time(NULL) * k + nameLen);
    int new_random;

    int unique;

    for (int i = 0; i < length; i++)
    {
        do
        {
            new_random = (rand() % (max - min + 1)) + min;

            unique = 1;
            for (int j = 0; j < i; j++)
            {
                if (array[j] == new_random)
                {
                    unique = 0;
                }
            }

        } while (!unique);
        array[i] = new_random;
    }
}

void man(int rank, int numWom)
{
    int ranks[numWom];
    int i = 0;
    MPI_Status status;

    // lower index means higher rank ie more wanted
    genRanks(ranks, numWom, 0, numWom - 1, (rank + 1) * numWom * 2);
    for (i = 0; i < numWom; i++)
    {
        printf("man id %d has ranked woman %d as %d\n", rank, ranks[i], i);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    int notAccepted = 1;
    int myWifesIndex;

    for (i = 0; i < numWom; i++)
    {
        MPI_Send(&rank, 1, MPI_INT, ranks[i] + numWom, 0, MPI_COMM_WORLD);
        MPI_Recv(&notAccepted, 1, MPI_INT, ranks[i] + numWom, 0, MPI_COMM_WORLD, &status);
        if (!notAccepted)
        {
            printf("->man id %d was accepted by woman %d\n", rank, ranks[i]);
            myWifesIndex = i;
        }
        else
        {
            continue;
        }
        int stillMyWife;
        printf("->man id %d waits for woman %d to confirm\n", rank, ranks[myWifesIndex]);
        MPI_Recv(&stillMyWife, 1, MPI_INT, ranks[myWifesIndex] + numWom, 0, MPI_COMM_WORLD, &status);
        if (stillMyWife)
        {
            printf("man id %d maried to woman %d\n", rank, ranks[myWifesIndex]);
            break;
        }
        printf("->man id %d was rejected by woman %d afterall\n", rank, ranks[myWifesIndex]);
    }
}

void woman(int rank, int numMen)
{
    int ranks[numMen];
    int i = 0;
    MPI_Status status;

    // lower index means higher rank ie more wanted
    genRanks(ranks, numMen, 0, numMen - 1, (rank + numMen) * numMen * 2);
    for (i = 0; i < numMen; i++)
    {
        printf("woman id %d has ranked man %d as %d\n", rank, ranks[i], i);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    int receiveMan;
    int currentManIndex;
    MPI_Recv(&receiveMan, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    printf("woman %d received man %d\n", rank, receiveMan);
    for (i = 0; i < numMen; i++)
    {
        if (ranks[i] == receiveMan)
        {
            currentManIndex = i;
            break;
        }
    }
    int accept = 0;
    int reject = 1;
    MPI_Send(&accept, 1, MPI_INT, receiveMan, 0, MPI_COMM_WORLD);
    // first wish man
    if (currentManIndex == 0)
    {
        MPI_Send(&accept, 1, MPI_INT, ranks[currentManIndex], 0, MPI_COMM_WORLD);
        printf("woman id %d maried to man %d\n", rank, ranks[currentManIndex]);
        womanAccepts++;
        MPI_Bcast(&womanAccepts, 1, MPI_INT, rank, MPI_COMM_WORLD);
        return;
    }
    int nopTimeout = numMen - 2;
    MPI_Request req;
    while ((womanAccepts < numMen-1) || nopTimeout > 0)
    {
        // key part here
        // MPI_Irecv(&receiveMan, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &req);

        int flag = 0;
        // MPI_Test(&req, &flag, &status);
        MPI_Iprobe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &flag, &status);

        if (flag)
        {
            MPI_Recv(&receiveMan, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            for (i = 0; i < numMen; i++)
            {
                if (ranks[i] == receiveMan)
                {
                    if (i < currentManIndex)
                    {
                        MPI_Send(&accept, 1, MPI_INT, ranks[i], 0, MPI_COMM_WORLD);
                        MPI_Send(&reject, 1, MPI_INT, ranks[currentManIndex], 0, MPI_COMM_WORLD);
                        currentManIndex = i;
                    }
                    else
                    {
                        printf("kolko puti rejectvash\n");
                        MPI_Send(&reject, 1, MPI_INT, ranks[i], 0, MPI_COMM_WORLD);
                        nopTimeout--;
                    }
                }
            }
        }
    }
    womanAccepts++;
    MPI_Bcast(&womanAccepts, 1, MPI_INT, rank, MPI_COMM_WORLD);
    MPI_Send(&accept, 1, MPI_INT, ranks[currentManIndex], 0, MPI_COMM_WORLD);
    printf("woman id %d maried to man %d\n", rank, ranks[currentManIndex]);
}

int main(argc, argv)
int argc;
char **argv;
{

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int menWemNum = size / 2;

    if (rank < menWemNum)
    {
        man(rank, menWemNum);
    }
    else
    {
        woman(rank - menWemNum, menWemNum);
    }

    MPI_Finalize();
    return 0;
}