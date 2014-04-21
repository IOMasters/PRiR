#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include "mpi.h"

#define SIMPLE_SPRNG		/* simple interface                        */
#define USE_MPI			/* MPI version of SPRNG                    */
#include "sprng_cpp.h"
#define SEND_TAG 123
using namespace std;

int main(int argc, char *argv[])
{
	MPI_Init(&argc,&argv);	/* Initialize MPI                          */

	int nprocs,myid;
	int segment[5];
	bool arrival;
	int wait_cnt=0;
	int arrival_cnt=0;
	int queue=0;
	int queue_accum=0;

	for (int i = 0; i < 5; ++i)
	{
		segment[i]=-1;
	}

	MPI_Status status;
	int seed = make_sprng_seed();	
  	MPI_Comm_size(MPI_COMM_WORLD,&nprocs); /* Find number of processes       */
  	MPI_Comm_rank(MPI_COMM_WORLD,&myid); /* Find rank of process             */

  	init_sprng(seed,CRAYLCG, 2);	/* initialize stream               */
    print_sprng();

    for (int i = 0; i < 10000; ++i)
	{
		if(sprng()<0.5)
		{
			arrival=true;
			arrival_cnt++;
		}
		else
		{
			arrival=false;
		}
		int temp=segment[4];
		MPI_Send(segment, 1, MPI_INT, (myid+1)%nprocs, SEND_TAG, MPI_COMM_WORLD); 
		MPI_Recv(segment+4, 1, MPI_INT, (myid-1+nprocs)%nprocs, SEND_TAG,MPI_COMM_WORLD, &status);
		for (int i = 0; i < 4; ++i)
		{
			segment[i]=segment[i+1];
		}
		segment[3]=temp;
		
	}


    MPI_Finalize();	
}