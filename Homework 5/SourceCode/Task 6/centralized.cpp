#include <bits/stdc++.h>
#include <mpi.h>
#define DEBUG 0

using namespace std;

mutex cout_mutex;
void print_message(int my_id, int largest, int smallest)
{
    if(!DEBUG) return;
    lock_guard<mutex> lock(cout_mutex);
    cout << "Thread: " << my_id << " smallest val: " << smallest << " largest val: " << largest << "\n";
}

int main(int argc, char *argv[])
{
    // Initialize MPI
    MPI_Init(&argc, &argv);
    auto start = chrono::high_resolution_clock::now();
    int num_of_processes, my_id;
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes); // Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    srand(my_id);
    int v = rand();
    int smallest = v, largest = v;
    int results[num_of_processes];
    MPI_Barrier(MPI_COMM_WORLD);
    if (my_id == 0)
    {
        for (int i = 1; i < num_of_processes; ++i)
        {
            smallest = min(smallest, results[i]);
            largest = max(largest, results[i]);
        }

        cout << "Thread: " << my_id << " recived all needed values" << "\n";    //no need for lock here
        print_message(my_id,largest,smallest);

        for (int i = 1; i < num_of_processes; i++)
        {
            MPI_Send(&smallest, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&largest, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Send(&v, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Recv(&smallest, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&largest, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        print_message(my_id,largest,smallest);
    }
    MPI_Finalize();
    auto end = chrono::high_resolution_clock::now();

    // Calculate the elapsed time in seconds.
    chrono::duration<double> elapsed = end - start;
    cout << "Execution time: " << elapsed.count() << " seconds." << endl;
    

}