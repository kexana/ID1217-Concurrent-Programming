#include <bits/stdc++.h>
#include <mpi.h>

using namespace std;

mutex cout_mutex;
void print_message(int my_id, int largest, int smallest)
{
    lock_guard<mutex> lock(cout_mutex);
    cout << "Thread: " << my_id << " smallest val: " << smallest << " largest val: " << largest << "\n";
}

int main(int argc, char *argv[])
{
    // Initialize MPI
    MPI_Init(&argc, &argv);

    int num_of_processes, my_id;
    MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes); // Get the number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    srand(my_id);
    int v = rand();
    int smallest = v, largest = v;
    for (int i = 0; i < num_of_processes; i++)
    {
        if (i == my_id)
            continue;
        MPI_Send(&smallest, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        MPI_Send(&largest, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
    }

    for (int i = 0; i < num_of_processes; ++i)
    {
        if (i == my_id) continue;
        int x;
        MPI_Recv(&x, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
        smallest = min(smallest, x);
        largest = max(largest, x);
    }

    print_message(my_id, largest, smallest);
    MPI_Finalize();

}
