#include <stdio.h>
#include "mpi.h"
int sizeval=0;

int values=0;
void Arrayshift(int original_array[], int new_array[],int factorforshift, int shift_portion[] );
void Right(int original_array[], int new_array[], int factorforshift, int shift_portion[]);
void Left(int original_array[], int new_array[], int factorforshift, int shift_portion[]);

int main( int cvalue, char *valuev[] )
{
	int factorforshift;
	int	new_position;
	int element;
	int	i= 0;
  int procrank;
  int	sizeval;

  MPI_Status status;

  MPI_Init(&cvalue, &valuev);

  MPI_Comm_rank(MPI_COMM_WORLD, &procrank);
	MPI_Comm_size(MPI_COMM_WORLD, &sizeval);

  if(procrank == 0)
	{
		printf("Input Array Size:\n");
		scanf("%d", &values);
		printf("\n Shift factor ");
		scanf("%d", &shift_cnt);
		int input_array[values];
		int new_array[values];
		int shift_portion[values];

		printf("Enter processor rank: %d\n", procrank);
		for(i = 0; i < values; i++)
		{
			printf("\nA[%d]:",i);
			scanf("%d", &input_array[i]);
		}
		if(shift_cnt < 0)
		{
			while(shift_cnt < (-values))
			{
				shift_cnt = shift_cnt + values;
			}
		}
		else if(shift_cnt > 0)
		{
			while(shift_cnt > values)
			{
				shift_cnt = shift_cnt - values;
			}

		}

		if(sizeval > 1)
		{
			for (element = 1; element < sizeval; element++)
			{
				MPvaluesend(&shift_cnt, 1, MPI_INT, element, element, MPI_COMM_WORLD);
				MPvaluesend(&values, 1, MPI_INT, element, element, MPI_COMM_WORLD);
				printf("Enter pricessor rank : %d\n", element);
				for(i = 0; i < values; i++)
				{
					printf("\nA[%d]:",i);
					scanf("%d", &new_array[i]);
				}
				MPvaluesend(&new_array, values, MPI_INT, element, element, MPI_COMM_WORLD);
			}
		}


		shiftArray(input_array, new_array, shift_cnt,shift_portion );

		if(shift_cnt < 0)
		{
			new_position = (-shift_cnt );
		}
		else
		{
			new_position = shift_cnt;
		}

		if(sizeval > 1)
		{
			if(shift_cnt >= 0)
			{
				MPvaluesend(&shift_portion, new_position, MPI_INT, 1, 1, MPI_COMM_WORLD);
				MPI_Recv(&shift_portion, new_position, MPI_INT, (sizeval - 1), (sizeval - 1), MPI_COMM_WORLD, status);
			}
			else
			{
				MPvaluesend(&shift_portion, new_position, MPI_INT, (sizeval - 1), (sizeval - 1), MPI_COMM_WORLD);
				MPI_Recv(&shift_portion, new_position, MPI_INT, 1, procrank, MPI_COMM_WORLD, status);
			}

		}
			if(shift_cnt >= 0)
			{
				for(i = 0; i < shift_cnt; i++)
				{
					new_array[i] = shift_portion[i];
				}
			}
			if(shift_cnt < 0)
			{
				for(i = 0, element = (values + shift_cnt); element < values; i++, element++)
				{
					new_array[element] = shift_portion[i];
				}
			}
		for(i = 0; i < values; i++)
		{
			printf("Original Array : %d, New Array  is : %d \n",input_array[i], new_array[i]);
		}
		if(sizeval > 1)
		{
			for (element = 1; element < sizeval; element++)
			{
				MPvaluesend(&shift_cnt, 1, MPI_INT, element, element, MPI_COMM_WORLD);
				MPI_Recv(&shift_cnt, 1, MPI_INT, element, procrank, MPI_COMM_WORLD, status);
			}
		}
	}
    else if (procrank !=0)
    {
		MPI_Recv(&shift_cnt, 1, MPI_INT, 0, procrank, MPI_COMM_WORLD, status);
		MPI_Recv(&values, 1, MPI_INT, 0, procrank, MPI_COMM_WORLD, status);
        int input_array[values];
        int new_array[values];
		int shift_portion[values];
		int inter_array[values];

		if(shift_cnt < 0)
		{
			new_position = shift_cnt * -1;
		}
		else
		{
			new_position = shift_cnt;
		}

		MPI_Recv(&input_array, values, MPI_INT, 0, procrank, MPI_COMM_WORLD, status);
		shiftArray(input_array, new_array, shift_cnt,shift_portion);
		if(shift_cnt >= 0)
		{
			MPI_Recv(&inter_array, new_position, MPI_INT, (procrank - 1), procrank, MPI_COMM_WORLD, status);
			for(i = 0; i < shift_cnt; i++)
			{
				new_array[i] = inter_array[i];
			}
		}
		if(shift_cnt < 0)
		{
			if(procrank == (sizeval - 1))
			{
				MPI_Recv(&inter_array, new_position, MPI_INT, 0, procrank, MPI_COMM_WORLD, status);
			}
			else
			{
				MPI_Recv(&inter_array, new_position, MPI_INT, (procrank + 1), procrank, MPI_COMM_WORLD, status);
			}
			for(i = 0, element = (values + factorforshift); element < values; i++, element++)
			{
				new_array[element] = inter_array[i];
			}
		}

        if(procrank == (sizeval - 1))
		{
			if(factorforshift >= 0)
			{
				MPvaluesend(&shift_portion, new_position, MPI_INT, 0, procrank, MPI_COMM_WORLD);
			}
			else
			{
				MPvaluesend(&shift_portion, new_position, MPI_INT, (procrank - 1), (procrank - 1), MPI_COMM_WORLD);
			}
		}
		else
		{
			if(factorforshift >= 0)
			{
				MPvaluesend(&shift_portion, new_position, MPI_INT, (procrank + 1), (procrank + 1), MPI_COMM_WORLD);
			}
			else
			{
				MPvaluesend(&shift_portion, new_position, MPI_INT, (procrank - 1), (procrank - 1), MPI_COMM_WORLD);
			}

		}
		MPI_Recv(&factorforshift, 1, MPI_INT, 0, procrank, MPI_COMM_WORLD, status);
		for(i = 0; i < values; i++)
		{
			printf("Input array: %d, Array shift : %d \n",input_array[i], new_array[i]);
		}
		MPvaluesend(&factorforshift, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
	MPI_Finalize();
    return 0;
}
void shiftArray(int original_array[], int new_array[], int factorforshift, int shift_portion[])
{
	int inter_array[values];

	if(factorforshift >= 0)
	{
		Right(original_array, new_array, factorforshift, shift_portion);

	}

	if(factorforshift < 0)
	{
		Left(original_array, new_array, factorforshift, shift_portion);
	}

}
void Right(int original_array[], int new_array[], int factorforshift, int shift_portion[])
{
	int i = 0, j = 0;
	for(j = 0; j <= factorforshift; j++)
		{
			new_array[j] = original_array[i];
		}
		for(i = 1; i < ( values - factorforshift); i++, j++)
		{
			new_array[j] = original_array[i];
		}

		for(i = 1; i <= factorforshift; i++)
		{
			shift_portion[i-1] = original_array[values - factorforshift + i - 1];
		}
}
void Left(int original_array[], int new_array[], int factorforshift, int shift_portion[])
{
	int i = 0, j = 0;
	i = -1;
		for(j = 0; j <= (values + factorforshift); j++)
		{
			new_array[j] = original_array[(factorforshift * i) + j];
		}
		for(j = 0; j <= (factorforshift * i); j++)
		{
			shift_portion[j] = original_array[j];
		}
}
