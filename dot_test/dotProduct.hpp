#ifndef H_DOTPRODUCT
#define H_DOTPRODUCT

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

void dotProductWrapper (float matrixA[10][10], float matrixB[10][10], float matrixC[10][10]);


#endif
