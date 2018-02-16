#include "dotProduct.hpp"
#include <algorithm>


void dotProductWrapper (local_t matrixA[10][10], local_t matrixB[10][10], local_t matrixC[10][10])
{
#pragma HLS ARRAY_PARTITION variable=matrixC complete dim=0
#pragma HLS ARRAY_PARTITION variable=matrixA complete dim=1
    FOR_TESTING_OUT:   for (int colC = 0; colC < 10; colC++) {
        local_t buffBCol [10];
#pragma HLS ARRAY_PARTITION variable=buffBCol complete dim=0
FOR_BUFFER:
		for (int i=0; i<10; i++)
        {
#pragma HLS PIPELINE
            buffBCol[i] = (local_t) matrixB[i][colC];
        }
FOR_INNER:
		for (int rowC = 0; rowC < 10; rowC++)
        {
#pragma HLS UNROLL skip_exit_check
			matrixC[rowC][colC] = 0;
			dotProduct<local_t, 10> (buffBCol, matrixA[rowC], &matrixC[rowC][colC] );
			matrixC[rowC][colC] = std::max(0.0f, (float) matrixC[rowC][colC]);
        }
    }
}
