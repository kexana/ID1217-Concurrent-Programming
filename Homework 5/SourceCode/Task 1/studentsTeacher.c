#include <mpi.h>
#include <stdio.h>

void teacher(int stNum)
{
    int i;
    int students[stNum];
    MPI_Status status;
    int unpaired = 0; // increment when a message is recieved and if it gets to two then pair the current student with the previous one

    for (i = 0; i < stNum; i++)
    {
        //receive message from any source since we do not know which student will request first
        MPI_Recv(&students[i], 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        printf("Received request from %d\n", students[i]);
        unpaired++;
        printf("Curently unpaired %d\n", unpaired);
        if (unpaired == 2)
        {
            MPI_Send(&students[i], 1, MPI_INT, students[i - 1], 0, MPI_COMM_WORLD);
            MPI_Send(&students[i - 1], 1, MPI_INT, students[i], 0, MPI_COMM_WORLD);
            unpaired = 0;
        }
    }
    //if any student was left unpaired (odd num of students) => pair by itself
    if(unpaired == 1){
        MPI_Send(&students[stNum-1], 1, MPI_INT, students[stNum - 1], 0, MPI_COMM_WORLD);
    }

    for (i = 0; i < stNum;i++){
        int done = 1;
        MPI_Send(&done, 1, MPI_INT, students[i], 0, MPI_COMM_WORLD);
    }
}

void student(int id)
{
    int pairStId;
    MPI_Status status;

    //ask teacher process rank 0 for a pairing
    MPI_Send(&id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    //receive pairing from teacher
    MPI_Recv(&pairStId, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    //wait for teacher to pair evryone before printing
    int teacherDone = 0;
    MPI_Recv(&teacherDone, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    if(teacherDone){
        printf("The student %d was paired with the student %d\n", id, pairStId);
    }
}

int main(argc, argv)
int argc;
char **argv;
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    //the 0 rank process will be the teacher, anything else is a student
    if(!rank){
        //exclude teacher from the student pool
        teacher(size-1);
    }else{
        student(rank);
    }

    MPI_Finalize();
    return 0;
}
