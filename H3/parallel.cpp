#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>
#include <sys/time.h>

#define MASTER  0
#define TAG     0
#define MSGSIZE 1000
#define MAX 25

using namespace std;

int main(int argc, char* argv[])
{
    int my_rank, source, num_nodes, workers;
	int buffer[MSGSIZE];

	auto start_time = std::chrono::high_resolution_clock::now();
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_nodes);
	workers = num_nodes - 1;
	
	// workers has to be < 1k
	int **matrix;	
	matrix = new int *[workers];
	for(int i = 0; i < workers; i++)
    {
		matrix[i] = new int[workers];
	}	
	for(int i = 0, x = 0; i < workers; i++)
	{
		for(int j = 0; j < workers; j++, x++)
		{
			matrix[i][j] = x;
		}
	}
	
    if (my_rank != MASTER) 
    {
		int send[1];
		
		for(int i = workers - my_rank, x = 0; x < workers; i++, x++)
		{
			if(i >= workers)
				i = 0;
			
			// Can't talk to yourself
			if((i + 1) == my_rank)
				continue;
				
			send[0] = matrix[my_rank-1][i];
			
			// Send first
			if(my_rank < (i + 1))
			{
				MPI_Send(send, 1, MPI_INT, (i + 1), TAG, MPI_COMM_WORLD);		
				MPI_Recv(buffer, 1, MPI_INT, (i + 1), TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				matrix[my_rank-1][i] = buffer[0];		
			}
			
			// Receive first
			else
			{
				MPI_Recv(buffer, 1, MPI_INT, (i + 1), TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				matrix[my_rank-1][i]=buffer[0];
				MPI_Send(send, 1, MPI_INT, (i + 1), TAG, MPI_COMM_WORLD);				
			}			
		}
				
		MPI_Send(matrix[my_rank-1], MSGSIZE, MPI_INT, MASTER, TAG, MPI_COMM_WORLD);
	}
    else 
	{
        printf ("N nodes: %d\n", num_nodes);
        printf ("Master (process %d)\n", my_rank);
		cout << "Initial array:" << endl;
		for(int i = 0, x = 0; i < workers; i++)
		{
			for(int j = 0; j < workers; j++, x++)
			{
				if(matrix[i][j] < 10)
					cout << " ";
				cout << matrix[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
		
		// Get results
        for (source = 1; source < num_nodes; source++) 
        {
            MPI_Recv(buffer, MSGSIZE, MPI_INT, source, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(int i = 0; i < workers; i++)
				matrix[source-1][i] = buffer[i];
        }
		
		cout << "Transposed :" << endl;
		for(int i = 0, x = 0; i < workers; i++)
		{
			for(int j = 0; j < workers; j++, x++)
			{
				if(matrix[i][j] < 10)
					cout << " ";
				cout << matrix[i][j] << " ";
			}
			cout << endl;
		}
	}

    MPI_Finalize();
	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    double elapsed_time = duration.count() / 1000.0;  // s

    double data_size = (workers * workers * sizeof(int) * 2) / (1024.0 * 1024.0);
    double bandwidth = data_size / elapsed_time; // 
    printf("%f s\t%f GB/s\n", elapsed_time, bandwidth);
    printf("%f\n", bandwidth);
	
	return 0;
}