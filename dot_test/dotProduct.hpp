#ifndef H_DOTPRODUCT
#define H_DOTPRODUCT
#include "ap_fixed.h"

typedef ap_fixed<20,10> local_t;
//typedef float local_t;

template <typename Type, int SIZE> 
void dotProduct(Type vectorA[SIZE], Type vectorB[SIZE], Type* sum)
{
//#pragma HLS INLINE
//#pragma HLS ARRAY_PARTITION variable=vectorB complete dim=0
	//Type sum = 0;
    FOR_DOTPRODUCT:
    for (int i=0; i<SIZE; i++)
    {
#pragma HLS PIPELINE
#pragma HLS EXPRESSION_BALANCE
        *sum += vectorA[i] * vectorB[i];
    }
    //return sum;
}

void dotProductWrapper (local_t matrixA[10][10], local_t matrixB[10][10], local_t matrixC[10][10]);


#endif
