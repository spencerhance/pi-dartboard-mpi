// Adapted from method here: https://computing.llnl.gov/tutorials/mpi/samples/C/mpi_pi_reduce.c

// Estimate pi with the dartboard method
// All processes compute their own pi, the total
// is then averaged across all procs

#include <stdio.h> // use stdio.h instead of iostream
#include <mpi.h> // mpi header (so we can use MPI_*routines)
#include <stdlib.h>
#include <time.h>

// Dart function
double throwDarts(int num_darts);


// Main
int main(int argc, char *argv[]) 
{
	int num_procs;
	int rank;
	int name_len;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	double pi_avg_per_round = 0;
	double pi_avg = 0;
	double pi_sum = 0;
	double rank_pi;
	
	int num_darts = 50000;
	int num_rounds = 100;
	
	// Seed rand
	srand(time(NULL));

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &name_len);

	printf("Starting process %i\n", rank);
	
	for (int i = 0; i < num_rounds; i++)
	{
		rank_pi = throwDarts(num_darts);
		
		int error = MPI_Reduce(&rank_pi, &pi_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

		// Check if master
		// Compute average pi value
		if (rank == 0)
		{
			// Get average for this round
			pi_avg_per_round = pi_sum / num_procs;
			
			// Update overall average
			pi_avg = ((pi_avg * i) + pi_avg_per_round) / (i + 1);

			printf("Round %i, avg %f\n", i, pi_avg);			
		}
		
	}	

	// Done, print final average
	if (rank == 0)
	{
		printf("Computed pi: %f\n", pi_avg);
	}
		

	MPI_Finalize();
	
	return 0;
}


// Essentially the single threaded implementation to throw the darts
// and calculate pi
double throwDarts(int num_darts)
{	
	int count = 0;

	// Throw the darts
	for (int i = 0; i < num_darts; i++)
	{
		// Get x and y coords, random values between 0 and 1
		double x_coord = (float) rand() / (float) RAND_MAX;
		double y_coord = (float) rand() / (float) RAND_MAX;
		
		// Check if coords are inside circle
		if ((x_coord*x_coord + y_coord*y_coord) < 1.0)
			count++;
	}


	// Calculate pi for this process
	double pi = 4 * ((double) count / (double) num_darts);

	return pi;
}
