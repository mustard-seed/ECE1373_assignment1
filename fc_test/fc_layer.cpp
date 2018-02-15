#include <algorithm>
#include <cassert>
#include "fc_layer.h"

#ifdef FC_DOTENG
#include "dotProduct.hpp"
#endif

/*
 * ECE1373 Code begins
 */
#define PRAGMA_SUB(x) _Pragma (#x)
#define PRAGMA_HLS(x) PRAGMA_SUB(x)
/*
 * ECE1373 Code ends
 */

void fc_layer(float weights[MAX_INPUT_SIZE*MAX_OUTPUT_SIZE],
			  float biases[MAX_OUTPUT_SIZE],
			  float input[MAX_INPUT_SIZE*MAX_BATCH],
			  float output[MAX_OUTPUT_SIZE*MAX_BATCH],
              const int batch_size,
              const int num_inputs,
              const int num_outputs)
{
#ifdef FC_DEFAULT
  assert(batch_size <= MAX_BATCH);
  assert(num_outputs <= MAX_OUTPUT_SIZE);
  assert(num_inputs <= MAX_INPUT_SIZE);
  // Batch Iterator
FOR_BATCH:
  for (int b = 0; b < batch_size; b++) {

FOR_OUTPUT:
    // Output Node Iterator
    for (int o = 0; o < num_outputs; o++) {

      // Set bias
      output[b*num_outputs+o] = biases[o];

FOR_INPUT:
      // Accumulate weighted sum
      for (int i = 0; i < num_inputs; i++) {

        output[b*num_outputs+o] += (t_weight)input[b*num_inputs+i] * (t_weight)weights[o*num_inputs+i];
      }

      // Compute activation
      output[b*num_outputs+o] = std::max(0.0f, ((t_output)output[b*num_outputs+o]);
    }
  }
#elif defined FC_PIPELINEUNROLL
  assert(batch_size <= MAX_BATCH);
  assert(num_outputs <= MAX_OUTPUT_SIZE);
  assert(num_inputs <= MAX_INPUT_SIZE);
  // Batch Iterator
FOR_BATCH:
  for (int b = 0; b < batch_size; b++) {

FOR_OUTPUT:
    // Output Node Iterator
    for (int o = 0; o < num_outputs; o++) {

      // Set bias
      output[b*num_outputs+o] = biases[o];

////Set buffer for wegiths
//      t_weight *pbuff_weights = &weights[o*num_inputs];

FOR_INPUT:
      // Accumulate weighted sum
      for (int i = 0; i < num_inputs; i++) {

        output[b*num_outputs+o] += input[b*num_inputs+i]* (t_weight) weights[o*num_inputs+i];
      }
      // Compute activation
      output[b*num_outputs+o] = std::max(0.0f, (t_output) output[b*num_outputs+o]);
    }
  }
#elif defined FC_DOTENG
#define BLOCKDIM  8 //Break matrix-matrix multipilcation into 5 by 5 block matrix multiplications
  assert(batch_size <= MAX_BATCH);
  assert(num_outputs <= MAX_OUTPUT_SIZE);
  assert(num_inputs <= MAX_INPUT_SIZE);

// Batch Iterator
FOR_BATCH:
for (int b = 0; b < batch_size; b += BLOCKDIM)
{
	//Output iterator {
FOR_OUTPUT:
 	 // Output Node Iterator
  	  for (int o = 0; o < num_outputs; o += BLOCKDIM) {
  		  t_output outputBuffer [BLOCKDIM][BLOCKDIM] = {0}; //Output buffer
#pragma HLS ARRAY_PARTITION variable=outputBuffer complete dim = 0
    	FOR_INPUT:
    	      // Accumulate weighted sum
		for (int i = 0; i < num_inputs; i += BLOCKDIM) {
		// Set bias
			//output[b*num_outputs+o] = biases[o];
			//TODO: Add bias back!!!!!

			//Load Buffers
			static t_weight weightBuffer [BLOCKDIM][BLOCKDIM]; //Weight buffer is ROW major
			static t_input inputBuffer [BLOCKDIM][BLOCKDIM]; //Input buffer is COL major
#pragma HLS ARRAY_PARTITION variable=weightBuffer complete dim = 0
#pragma HLS ARRAY_PARTITION variable=inputBuffer complete dim = 0
			FOR_INBUFFERS_OUTER:
			for (int l = 0; l < BLOCKDIM; l++)
			{
#pragma HLS PIPELINE
				FOR_INBUFFERS_INNER:
				for (int m = 0; m < BLOCKDIM; m++)
				{
					//Load weight buffer only
					if (o+l < num_outputs && m+i < num_inputs)
					{
						weightBuffer [l][m] = weights[num_inputs*(o+l) + m+i];
					}
					else
					{
						weightBuffer [l][m] = 0;
					}

					//Load input buffer only
					if (i+m < num_inputs && l + b < batch_size)
					{
						inputBuffer [l][m] = input[(b+l)*num_inputs + m + i];
					}
					else
					{
						inputBuffer [l][m] = 0;
					}
				}
			}

			//Do matrix-matrix multiplication
FOR_MATMATMUL_OUTER:
			for (int l = 0; l < BLOCKDIM; l++)
			{
#pragma HLS UNROLL skip_exit_check
FOR_MATMATMUL_INNER:
				for (int m = 0; m < BLOCKDIM; m++)
				{
#pragma HLS UNROLL skip_exit_check
					outputBuffer[l][m] += dotProduct<t_weight, BLOCKDIM>(weightBuffer[m], inputBuffer[l]);
				}
			}
		}
      //TODO: Compute activation
      //output[b*num_outputs+o] = std::max(0.0f, output[b*num_outputs+o]);

		//Add bias and compute activation, then write to buffer
FOR_STREAMOUT_OUTER:
		for (int l = 0; l < BLOCKDIM; l++)
		{
FOR_STREAMOUT_INNNER:
		for (int m = 0; m < BLOCKDIM; m++)
			{
				if (o+m < num_outputs && b+l < batch_size)
				{
					t_output temp = outputBuffer[l][m] + (t_output) biases[o+m];
					output[(b+l)*num_outputs + o + m] = std::max((t_output) 0.0, temp);
				}
			}
		}
    }
  }
#endif
}
