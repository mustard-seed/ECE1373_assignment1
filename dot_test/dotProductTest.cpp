#include "dotProduct.hpp"
#include <iostream>

using namespace std;


float matrixA[10][10], matrixB[10][10], matrixC[10][10], matrixD[10][10];

int main()
{
	int retval = 0;

	/*
	 * Initialize test arrays
	 */
	for (int i=0; i < 10; i++)
	{
		for (int j=0; j<10; j++)
		{
			matrixA[i][j] = (float) (i+j);
			matrixB[i][j] = (float) (i+j);
		}
	}

	/*
	 * Calculate the correct result
	 */

	for (int i=0; i < 10; i++)
	{
		for (int j=0; j<10; j++)
		{
			matrixC[i][j] = 0;
			for (int k=0; k<10; k++)
			{
				matrixC[i][j] += matrixA[i][k]*matrixB[k][j];
			}
		}
	}

	dotProductWrapper(matrixA, matrixB, matrixD);

	float total = 0.0f;

	for (int i=0; i < 10; i++)
		{
			for (int j=0; j<10; j++)
			{
				float err = matrixC[i][j] - matrixD[i][j];
				total += err*err;
			}
		}

	cout << "Mean squared error: " << total / 100.0 << endl;
	return retval;
}

