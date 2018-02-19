#include "dotProduct.hpp"
#include <iostream>
#include <algorithm>

using namespace std;


float matrixA[5][5], matrixB[5][5], matrixC[5][5];
local_t matrixAt[5][5], matrixBt[5][5], matrixCt[5][5];

int main()
{
	int retval = 0;

	/*
	 * Initialize test arrays
	 */
	for (int i=0; i < 5; i++)
	{
		for (int j=0; j<5; j++)
		{
			matrixA[i][j] = (float) (i+j-10.0f);
			matrixAt[i][j] = (local_t) (i+j-10.0f);
			matrixB[i][j] = (float) (i+j-10.0f);
			matrixBt[i][j] = (local_t) (i+j-10.0f);
		}
	}

	/*
	 * Calculate the correct result
	 */

	for (int i=0; i < 5; i++)
	{
		for (int j=0; j<5; j++)
		{
			matrixC[i][j] = 0;
			for (int k=0; k<5; k++)
			{
				matrixC[i][j] += matrixA[i][k]*matrixB[k][j];
			}
			matrixC[i][j] = std::max(0.0f, matrixC[i][j]);
		}
	}

	dotProductWrapper(matrixAt, matrixBt, matrixCt);

	float total = 0.0f;

	for (int i=0; i < 5; i++)
		{
			for (int j=0; j<5; j++)
			{
				float err = matrixC[i][j] - (float) matrixCt[i][j];
				total += err*err;
			}
		}

	cout << "Mean squared error: " << total / 100.0 << endl;
	return retval;
}

