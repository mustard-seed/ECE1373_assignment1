#include "dotProduct.hpp"

template <typename Type, int SIZE> 
Type dotProduct(Type vectorA[SIZE], Type vectorB[SIZE])
{
//#pragma HLS ARRAY_PARTITION variable=vectorB complete dim=0
    Type sum = 0;
    FOR_DOTPRODUCT:
    for (int i=0; i<SIZE; i++)
    {
#pragma HLS PIPELINE
#pragma HLS EXPRESSION_BALANCE
        sum += vectorA[i] * vectorB[i];
    }
    
    return sum;
}

void dotProductWrapper (float matrixA[10][10], float matrixB[10][10], float matrixC[10][10])
{
#pragma HLS ARRAY_PARTITION variable=matrixC complete dim=0
#pragma HLS ARRAY_PARTITION variable=matrixA complete dim=1
    float sum = 0;
    FOR_TESTING_OUT:   for (int colC = 0; colC < 10; colC++) {
        float buffBCol [10];
#pragma HLS ARRAY_PARTITION variable=buffBCol complete dim=0
FOR_BUFFER:
		for (int i=0; i<10; i++)
        {
#pragma HLS PIPELINE
            buffBCol[i] = matrixB[i][colC];
        }
FOR_INNER:
		for (int rowC = 0; rowC < 10; rowC++)
        {
#pragma HLS UNROLL skip_exit_check
            matrixC[rowC][colC] = dotProduct<float, 10> (buffBCol, matrixA[rowC]);
        }
    }
}
