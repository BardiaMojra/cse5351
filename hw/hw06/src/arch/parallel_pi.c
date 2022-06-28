#include <stdio.h>
#include <math.h>
#include "mpi.h"
int main(argumentc, argumentv)
	int argumentc;
	char **argumentv;
	{
	MPI_Init(&argumentc, &argumentv);
	int rankc;
	int sizec;
	float tvalue = 0;
	float lowboundry;
	float upboundry=0;
	int i=0;
	float mul = 0;
	int gap=0;
	float pi=0;
	float sumofpi=0;
	float answer = 0;
	float exactpi = 3.14159265358;
	MPI_Status status;
	float total;
	MPI_Comm_rank(MPI_COMM_WORLD, &rankc);
	MPI_Comm_size(MPI_COMM_WORLD, &sizec);

	if(rankc !=0)
	{
		MPI_Recv(&gap, 1, MPI_INT, 0, rankc, MPI_COMM_WORLD, &status);
		for(i=rankc; i<gap; i=(i-1)+sizec)
		{
			tvalue = ((float)i/gap);
			pi = ((float)4/(1+(tvalue*tvalue)));
			if(i%2!=0)
			{
				pi = pi*4;
			}
			else
			{
				pi = pi*2;
			}
			sumofpi = sumofpi + pi;
		}
		MPI_Send(&sumofpi,1,MPI_FLOAT,0,rankc,MPI_COMM_WORLD);
	}
	else if(rankc==0)
	{
		printf("\nNumber of gaps(>400000): ");
		scanf("%d", &gap);
		lowboundry=((float)4/(1+0));
		upboundry=((float)4/(1+1));
		float temp_pi_res[gap];
		for(i=1; i<sizec; i++)
		{
			MPI_Send(&gap,1,MPI_INT,i,i,MPI_COMM_WORLD);
			MPI_Recv(&temp_pi_res[i], 1, MPI_FLOAT, i, i, MPI_COMM_WORLD, &status);
			answer = answer+temp_pi_res[i];
		}
		total = lowboundry + upboundry;
		answer = answer+total;
		mul = (3*gap);
		answer = answer/mul;
		printf("\nPI value=% .16f\n Error is % .16f\n", answer, fabs(answer - exactpi));
	}
	MPI_Finalize();
	return 0;
}
© 2022 GitHub, Inc.
Terms
Privacy
Security
Status
Docs
Contact GitHub
Pricing
