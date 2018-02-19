#ifndef H_DOTPRODUCT
#define H_DOTPRODUCT
#include "ap_fixed.h"

//typedef ap_fixed<20,10> local_t;
typedef float local_t;

template <typename Type, int SIZE> 
void dotProduct(Type vectorA[SIZE], Type vectorB[SIZE], Type* sum)
{
//#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=vectorB complete dim=0
#pragma HLS ARRAY_PARTITION variable=vectorA complete dim=0
#pragma HLS PIPELINE
	//Type sum = 0;
    FOR_DOTPRODUCT:
    for (int i=0; i<SIZE; i++)
    {
//#pragma HLS UNROLL skip_exit_check
//#pragma HLS PIPELINE
#pragma HLS EXPRESSION_BALANCE
        *sum += vectorA[i] * vectorB[i];
    }
    //return sum;
}

void dotProductWrapper (local_t matrixA[5][5], local_t matrixB[5][5], local_t matrixC[5][5]);


#endif