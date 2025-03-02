#include <bits/stdc++.h>
#include <mpi.h>
#define DEBUG 0
using namespace std;

mutex cout_mutex;
void print_message(int my_id, int largest, int smallest, int loop)
{
    if(!DEBUG) return;
    lock_guard<mutex> lock(cout_mutex);
    cout <<"Loop num: "<< loop << " Thread: " << my_id << " smallest val: " << smallest << " largest val: " << largest << "\n";
}
void print_number(int my_id, int x)
{
    if(!DEBUG) return;
    lock_guard<mutex> lock(cout_mutex);
    cout << "Thread: " << my_id << " value: " << x << "\n";
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
    int v = rand()%1000;
    int smallest = v, largest = v;
    print_number(my_id, v);
    int x;
    MPI_Barrier(MPI_COMM_WORLD);
    if (my_id == 0)
    {
        MPI_Send(&v, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 1, MPI_COMM_WORLD);
        MPI_Send(&v, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 2, MPI_COMM_WORLD);
        MPI_Recv(&x, 1, MPI_INT, (my_id + num_of_processes - 1) % num_of_processes, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        smallest = min(smallest, x);
        largest = max(largest, x);
        MPI_Recv(&x, 1, MPI_INT, (my_id + num_of_processes - 1) % num_of_processes, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        smallest = min(smallest, x);
        largest = max(largest, x);
        print_message(my_id, largest, smallest,1);

        MPI_Send(&smallest, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 1, MPI_COMM_WORLD);
        MPI_Send(&largest, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 2, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Recv(&x, 1, MPI_INT, (my_id + num_of_processes - 1) % num_of_processes, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        smallest = min(smallest, x);
        largest = max(largest, x);
        MPI_Recv(&x, 1, MPI_INT, (my_id + num_of_processes - 1) % num_of_processes, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        smallest = min(smallest, x);
        largest = max(largest, x);
        print_message(my_id, largest, smallest,1);
        MPI_Send(&smallest, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 1, MPI_COMM_WORLD);
        MPI_Send(&largest, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 2, MPI_COMM_WORLD);

        MPI_Recv(&x, 1, MPI_INT, (my_id + num_of_processes - 1) % num_of_processes, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        smallest = min(smallest, x);
        largest = max(largest, x);
        MPI_Recv(&x, 1, MPI_INT, (my_id + num_of_processes - 1) % num_of_processes, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        smallest = min(smallest, x);
        largest = max(largest, x);
        print_message(my_id , largest, smallest,2);

        MPI_Send(&smallest, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 1, MPI_COMM_WORLD);
        MPI_Send(&largest, 1, MPI_INT, (my_id + num_of_processes + 1) % num_of_processes, 2, MPI_COMM_WORLD);

    }

    print_message(my_id, largest, smallest,2);
    MPI_Finalize();
}
