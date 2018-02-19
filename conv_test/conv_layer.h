
#ifndef _CONV_LAYER_H
#define _CONV_LAYER_H
#include "convType.h"

// Limits
#define MAX_BATCH 10
#define MAX_KERNEL_SIZE 7
#define MAX_INPUT_DIMS 10
#define MAX_OUTPUT_DIMS 64
#define MAX_INPUT_WIDTH 230
#define MAX_INPUT_HEIGHT 230
#define MAX_OUTPUT_WIDTH 112
#define MAX_OUTPUT_HEIGHT 112
#define MAX_CONV_INPUT MAX_INPUT_DIMS*MAX_INPUT_WIDTH*MAX_INPUT_HEIGHT 
#define MAX_CONV_OUTPUT MAX_OUTPUT_DIMS*MAX_OUTPUT_WIDTH*MAX_OUTPUT_HEIGHT 

void conv_layer(float weights[MAX_INPUT_DIMS * MAX_OUTPUT_DIMS * MAX_KERNEL_SIZE * MAX_KERNEL_SIZE],
                float biases[MAX_OUTPUT_DIMS],
                float input[MAX_CONV_INPUT*MAX_BATCH],
                float output[MAX_CONV_OUTPUT*MAX_BATCH],
	            const int b,
	            const int od,
	            const int ox,
	            const int oy,
	            const int id,
	            const int ix,
	            const int iy,
	            const int s,
	            const int k);

template <typename Type, int SIZE>
void gemm(Type matA[SIZE][SIZE], Type matB[SIZE][SIZE], Type matC[SIZE][SIZE])
{
//#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=matA complete dim = 0
#pragma HLS ARRAY_PARTITION variable=matB complete dim = 0
#pragma HLS ARRAY_PARTITION variable=matC complete dim = 0
FOR_GEMM_OUT:
	for (int i=0; i<SIZE; i++)
	{
#pragma HLS UNROLL skip_exit_check
FOR_GEMM_IN:
		for(int j=0; j<SIZE; j++)
		{
#pragma HLS UNROLL skip_exit_check
FOR_GEMM_SUM:
			for(int k=0; k<SIZE; k++)
			{
#pragma HLS PIPELINE
				matC[i][j] += matA[i][k]*matB[k][j];
			}
		}
	}
}

#endif
