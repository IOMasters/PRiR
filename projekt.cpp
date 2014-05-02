#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include "mpi.h"
#define SIMPLE_SPRNG    /* simple interface                        */
#define USE_MPI
#define SIMPLE_SPRNG		/* simple interface                        */
#define USE_MPI			/* MPI version of SPRNG                    */
#include "sprng_cpp.h"
#include "mpe.h"
#include "mpe_graphics.h"
#define SEND_TAG 123
using namespace std;
int chodeExit(int myid,int nprocs);
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
	int exit_cnt=0;

	for (int i = 0; i < 5; ++i)
	{
		segment[i]=-1;
	}

	MPI_Status status;
	int seed = make_sprng_seed();	
  	MPI_Comm_size(MPI_COMM_WORLD,&nprocs); /* Find number of processes       */
  	MPI_Comm_rank(MPI_COMM_WORLD,&myid); /* Find rank of process             */

  	init_sprng(seed,CRAYLCG, 2);	/* initialize stream               */

    for (int i = 0; i < 10000; ++i)
	{
		if(sprng()<1.0/(double)nprocs)
		{
			arrival=true;
			arrival_cnt++;
		}
		else
		{
			arrival=false;
		}
		MPI_Send(segment, 1, MPI_INT, (myid+1)%nprocs, SEND_TAG, MPI_COMM_WORLD); 
		for (int i = 0; i < 4; ++i)
		{
			segment[i]=segment[i+1];
		}
		MPI_Recv(segment+4, 1, MPI_INT, (myid-1+nprocs)%nprocs, SEND_TAG,MPI_COMM_WORLD, &status);
		if(segment[2]==myid)
		{
			exit_cnt++;
			segment[2]=-1;
		}
		if(segment[2] == -1)
		{
			if(queue>0)
			{
				queue--;
				segment[2]=chodeExit(myid,nprocs);
			}
			else if(arrival)
			{
				arrival=0;
				segment[2]=chodeExit(myid,nprocs);
			}
		}
		if(arrival)
		{
			wait_cnt++;
			queue++;
		}

	
		
	}
	printf("Proces numer: %d Wait: %d Przybyło:%d Kolejka %d %d Opuściło: %d \n", myid,wait_cnt,arrival_cnt,queue,queue_accum,exit_cnt);

    MPI_Finalize();	
}

int chodeExit(int myid,int nprocs)
{
	double count=1.0/(double)nprocs;
	double exitNumber=sprng();
	int exitProces;
	for(int i=1;i<=nprocs;++i)
	{
		if(exitNumber<count*i)
		{
			exitProces=i-1;
			break;
		}
	}
	
	return exitProces;
}