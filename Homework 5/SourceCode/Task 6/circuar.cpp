#include <bits/stdc++.h>
#include <mpi.h>

using namespace std;

mutex cout_mutex;
void print_message(int my_id, int largest, int smallest, int loop)
{
    lock_guard<mutex> lock(cout_mutex);
    cout <<"Loop num: "<< loop << " Thread: " << my_id << " smallest val: " << smallest << " largest val: " << largest << "\n";
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
    int x;
    if (my_id == 0)
    {
        MPI_Send(&smallest, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 1, MPI_COMM_WORLD);
        MPI_Recv(&x, 1, MPI_INT, (my_id + num_of_processes - 1) % num_of_processes, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        smallest = min(smallest, x);
        largest = max(largest, x);
        print_message(my_id + num_of_processes, largest, smallest,1);

        MPI_Send(&largest, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 2, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Recv(&x, 1, MPI_INT, (my_id + num_of_processes - 1) % num_of_processes, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        smallest = min(smallest, x);
        largest = max(largest, x);
        MPI_Send(&smallest, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 2, MPI_COMM_WORLD);
        print_message(my_id + num_of_processes, largest, smallest,1);

        MPI_Recv(&x, 1, MPI_INT, (my_id + num_of_processes - 1) % num_of_processes, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        smallest = min(smallest, x);
        largest = max(largest, x);
        MPI_Send(&largest, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 2, MPI_COMM_WORLD);
    
    }

    print_message(my_id, largest, smallest,2);
    MPI_Finalize();
}
