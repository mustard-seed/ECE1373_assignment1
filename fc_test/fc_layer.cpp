#include <algorithm>
#include <cassert>
#include "fc_layer.h"

#if defined  (FC_DOTENG) || defined (FC_DOTENGNOPIPE)
#include "dotProduct.hpp"
#endif

#ifdef FC_DOTENGPINGPONG
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
#if defined(FC_DEFAULT)
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
	  t_weight tempAdd = (t_weight)  biases[o];
//#pragma HLS RESOURCE variable=tempAdd core=AddSub_DSP

FOR_INPUT:
      // Accumulate weighted sum
      for (int i = 0; i < num_inputs; i++) {

    	t_weight tempMul = (t_weight)input[b*num_inputs+i] * (t_weight)weights[o*num_inputs+i];
//#pragma HLS RESOURCE variable=tempMul core=DSP48


    	tempAdd += tempMul;
      }

      // Compute activation
      output[b*num_outputs+o] = std::max(0.0f, (float)tempAdd);
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

        output[b*num_outputs+o] += input[b*num_inputs+i]* (float) weights[o*num_inputs+i];
      }
      // Compute activation
      output[b*num_outputs+o] = std::max(0.0f, (float) output[b*num_outputs+o]);
    }
  }
#elif defined FC_DOTENG
#define BLOCKDIM  8 //Break matrix-matrix multipilcation into 5 by 5 block matrix multiplications
  assert(batch_size <= MAX_BATCH);
  assert(num_outputs <= MAX_OUTPUT_SIZE);
  assert(num_inputs <= MAX_INPUT_SIZE);
  t_output outputBuffer [BLOCKDIM][BLOCKDIM];
#pragma HLS ARRAY_PARTITION variable=outputBuffer complete dim = 0

// Batch Iterator
FOR_BATCH:
for (int b = 0; b < batch_size; b += BLOCKDIM)
{
	//Output iterator {
FOR_OUTPUT:
 	 // Output Node Iterator
  	  for (int o = 0; o < num_outputs; o += BLOCKDIM) {
FOR_INITOUTPUT:
  		  for (int p = 0; p < BLOCKDIM; p++)
  		  {
#pragma HLS PIPELINE
  			  for (int q=0; q < BLOCKDIM; q++)
  			  {
  				  	 outputBuffer[p][q] = (t_output) 0;
  			  }
  		  }
    	FOR_INPUT:
    	      // Accumulate weighted sum
		for (int i = 0; i < num_inputs; i += BLOCKDIM) {
#pragma HLS PIPELINE
		// Set bias
			//output[b*num_outputs+o] = biases[o];
			//TODO: Add bias back!!!!!

			//Load Buffers
			t_weight weightBuffer [BLOCKDIM][BLOCKDIM]; //Weight buffer is ROW major
			t_input inputBuffer [BLOCKDIM][BLOCKDIM]; //Input buffer is COL major
#pragma HLS ARRAY_PARTITION variable=weightBuffer complete dim = 0
#pragma HLS ARRAY_PARTITION variable=inputBuffer complete dim = 0
			FOR_INBUFFERS_OUTER:
			for (int l = 0; l < BLOCKDIM; l++)
			{
//#pragma HLS LOOP_FLATTEN off
				FOR_INBUFFERS_INNER:
				for (int m = 0; m < BLOCKDIM; m++)
				{
//#pragma HLS LOOP_FLATTEN off
					//Load weight buffer only
					if (o+l < num_outputs && m+i < num_inputs)
					{
						weightBuffer [l][m] = (t_weight) weights[num_inputs*(o+l) + m+i];
					}
					else
					{
						weightBuffer [l][m] = (t_weight) 0.0f;
					}

					//Load input buffer only
					if (i+m < num_inputs && l + b < batch_size)
					{
						inputBuffer [l][m] = (t_weight) input[(b+l)*num_inputs + m + i];
					}
					else
					{
						inputBuffer [l][m] = (t_weight) 0.0f;
					}
				}
			}

			//Do matrix-matrix multiplication
FOR_MATMATMUL_OUTER:
			for (int l = 0; l < BLOCKDIM; l++)
			{
#pragma HLS UNROLL skip_exit_check
//#pragma HLS PIPELINE off
//#pragma HLS LOOP_FLATTEN off
FOR_MATMATMUL_INNER:
				for (int m = 0; m < BLOCKDIM; m++)
				{
#pragma HLS UNROLL skip_exit_check
//#pragma HLS PIPELINE off
//#pragma HLS LOOP_FLATTEN off
					//t_weight temp = 0;
//#pragma HLS DEPENDENCE variable=temp pointer intra RAW true
					dotProduct<t_weight, BLOCKDIM>(weightBuffer[m], inputBuffer[l], &outputBuffer[l][m]);
					//outputBuffer[l][m] = temp;
				}
			}
		}
      //TODO: Compute activation
      //output[b*num_outputs+o] = std::max(0.0f, output[b*num_outputs+o]);

		//Add bias and compute activation, then write to buffer
FOR_STREAMOUT_OUTER:
		for (int l = 0; l < BLOCKDIM; l++)
		{
#pragma HLS PIPELINE
//#pragma HLS LOOP_FLATTEN off
FOR_STREAMOUT_INNNER:
		for (int m = 0; m < BLOCKDIM; m++)
			{
//#pragma HLS LOOP_FLATTEN off
				if (o+m < num_outputs && b+l < batch_size)
				{
					t_output temp = (t_output) outputBuffer[l][m] + (t_output) biases[o+m];
//#pragma HLS RESOURCE variable=temp core=AddSub
					output[(b+l)*num_outputs + o + m] = (float) std::max((float) 0.0, (float) (temp));
				}
			}
		}
    }
  }
#endif

#if defined(FC_DOTENGPINGPONG)
#define BLOCKDIM  6 //Break matrix-matrix multipilcation into 5 by 5 block matrix multiplications
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

		t_weight weightBufferA [BLOCKDIM][BLOCKDIM], weightBufferB [BLOCKDIM][BLOCKDIM]; //Weight buffer is ROW major
		t_input inputBufferA [BLOCKDIM][BLOCKDIM], inputBufferB [BLOCKDIM][BLOCKDIM]; //Input buffer is COL major
		#pragma HLS ARRAY_PARTITION variable=weightBufferA complete dim = 0
		#pragma HLS ARRAY_PARTITION variable=weightBufferB complete dim = 0
		#pragma HLS ARRAY_PARTITION variable=inputBufferA complete dim = 0
	    #pragma HLS ARRAY_PARTITION variable=inputBufferB complete dim = 0

		/*
		 * Weight/Input Ping-pong buffers' control variables.
		 */
		bool readFirstTime = true;
		bool readIntoA = true;

    	FOR_INPUT:
    	      // Accumulate weighted sum
		for (int i = 0; i < num_inputs; i += BLOCKDIM) {
#pragma HLS PIPELINE
			//Load Buffers
			FOR_INBUFFERS_OUTER:
			for (int l = 0; l < BLOCKDIM; l++)
			{
//#pragma HLS LOOP_FLATTEN off
				FOR_INBUFFERS_INNER:
				for (int m = 0; m < BLOCKDIM; m++)
				{
//#pragma HLS LOOP_FLATTEN off
					//Load weight buffer only
					if (o+l < num_outputs && m+i < num_inputs)
					{
						if (readIntoA)
						{
							weightBufferA [l][m] = (t_weight) weights[num_inputs*(o+l) + m+i];
						}
						else
						{
							weightBufferB [l][m] = (t_weight) weights[num_inputs*(o+l) + m+i];
						}
					}
					else
					{
						if (readIntoA)
						{
							weightBufferA [l][m] = (t_weight) 0.0f;
						}
						else
						{
							weightBufferB [l][m] = (t_weight) 0.0f;
						}
					}

					//Load input buffer only
					if (i+m < num_inputs && l + b < batch_size)
					{
						if (readIntoA)
						{
							inputBufferA [l][m] = (t_weight) input[(b+l)*num_inputs + m + i];
						}
						else
						{
							inputBufferB [l][m] = (t_weight) input[(b+l)*num_inputs + m + i];
						}
					}
					else
					{
						if (readIntoA)
						{
							inputBufferA [l][m] = (t_weight) 0.0f;
						}
						else
						{
							inputBufferB [l][m] = (t_weight) 0.0f;
						}
					}
				}
			}

			//Do matrix-matrix multiplication
FOR_MATMATMUL_OUTER:
			for (int l = 0; l < BLOCKDIM; l++)
			{
				#pragma HLS UNROLL skip_exit_check
				//#pragma HLS PIPELINE off
				//#pragma HLS LOOP_FLATTEN off
FOR_MATMATMUL_INNER:
				for (int m = 0; m < BLOCKDIM; m++)
				{
					#pragma HLS UNROLL skip_exit_check
					//#pragma HLS PIPELINE off
					//#pragma HLS LOOP_FLATTEN off
					//#pragma HLS DEPENDENCE variable=temp pointer intra RAW true
					if (!readFirstTime)
					{
						if (readIntoA)
						{
							dotProduct<t_weight, BLOCKDIM>(weightBufferB[m], inputBufferB[l], &outputBuffer[l][m]);
						}
						else
						{
							dotProduct<t_weight, BLOCKDIM>(weightBufferA[m], inputBufferA[l], &outputBuffer[l][m]);
						}
					}
				}
			}
			readFirstTime = false;
			readIntoA = !readIntoA;
		}

		//Read the left over
FOR_REMAININGMATMATMUL_OUTER:
			for (int l = 0; l < BLOCKDIM; l++)
			{
				#pragma HLS UNROLL skip_exit_check
				//#pragma HLS PIPELINE off
				//#pragma HLS LOOP_FLATTEN off
FOR_REMAININGMATMATMUL_INNER:
				for (int m = 0; m < BLOCKDIM; m++)
				{
					#pragma HLS UNROLL skip_exit_check
					//#pragma HLS PIPELINE off
					//#pragma HLS LOOP_FLATTEN off
					//#pragma HLS DEPENDENCE variable=temp pointer intra RAW true

					if (readIntoA)
					{
						dotProduct<t_weight, BLOCKDIM>(weightBufferB[m], inputBufferB[l], &outputBuffer[l][m]);
					}
					else
					{
						dotProduct<t_weight, BLOCKDIM>(weightBufferA[m], inputBufferA[l], &outputBuffer[l][m]);
					}
				}
			}
      //TODO: Compute activation
      //output[b*num_outputs+o] = std::max(0.0f, output[b*num_outputs+o]);

		//Add bias and compute activation, then write to buffer
FOR_STREAMOUT_OUTER:
		for (int l = 0; l < BLOCKDIM; l++)
		{
//#pragma HLS LOOP_FLATTEN off
FOR_STREAMOUT_INNNER:
		for (int m = 0; m < BLOCKDIM; m++)
			{
//#pragma HLS LOOP_FLATTEN off
				if (o+m < num_outputs && b+l < batch_size)
				{
					t_output temp = (t_output) outputBuffer[l][m] + (t_output) biases[o+m];
//#pragma HLS RESOURCE variable=temp core=AddSub
					output[(b+l)*num_outputs + o + m] = (float) std::max((float) 0.0, (float) (temp));
				}
			}
		}
    }
  }
#endif

#if defined (FC_DOTENGNOPIPE)
#define BLOCKDIM  5 //Break matrix-matrix multipilcation into 5 by 5 block matrix multiplications
  assert(batch_size <= MAX_BATCH);
  assert(num_outputs <= MAX_OUTPUT_SIZE);
  assert(num_inputs <= MAX_INPUT_SIZE);
  t_output outputBuffer [BLOCKDIM][BLOCKDIM];
#pragma HLS ARRAY_PARTITION variable=outputBuffer complete dim = 0

// Batch Iterator
FOR_BATCH:
for (int b = 0; b < batch_size; b += BLOCKDIM)
{
	//Output iterator {
FOR_OUTPUT:
 	 // Output Node Iterator
  	  for (int o = 0; o < num_outputs; o += BLOCKDIM) {
FOR_INITOUTPUT:
  		  for (int p = 0; p < BLOCKDIM; p++)
  		  {
#pragma HLS PIPELINE
  			  for (int q=0; q < BLOCKDIM; q++)
  			  {
  				  	 outputBuffer[p][q] = (t_output) 0;
  			  }
  		  }
    	FOR_INPUT:
    	      // Accumulate weighted sum
		for (int i = 0; i < num_inputs; i += BLOCKDIM) {
//#pragma HLS PIPELINE
		// Set bias
			//output[b*num_outputs+o] = biases[o];
			//TODO: Add bias back!!!!!

			//Load Buffers
			t_weight weightBuffer [BLOCKDIM][BLOCKDIM]; //Weight buffer is ROW major
			t_input inputBuffer [BLOCKDIM][BLOCKDIM]; //Input buffer is COL major
#pragma HLS ARRAY_PARTITION variable=weightBuffer complete dim = 0
#pragma HLS ARRAY_PARTITION variable=inputBuffer complete dim = 0
			FOR_INBUFFERS_OUTER:
			for (int l = 0; l < BLOCKDIM; l++)
			{
#pragma HLS PIPELINE
//#pragma HLS LOOP_FLATTEN off
				FOR_INBUFFERS_INNER:
				for (int m = 0; m < BLOCKDIM; m++)
				{
//#pragma HLS LOOP_FLATTEN off
					//Load weight buffer only
					if (o+l < num_outputs && m+i < num_inputs)
					{
						weightBuffer [l][m] = (t_weight) weights[num_inputs*(o+l) + m+i];
					}
					else
					{
						weightBuffer [l][m] = (t_weight) 0.0f;
					}

					//Load input buffer only
					if (i+m < num_inputs && l + b < batch_size)
					{
						inputBuffer [l][m] = (t_weight) input[(b+l)*num_inputs + m + i];
					}
					else
					{
						inputBuffer [l][m] = (t_weight) 0.0f;
					}
				}
			}

			//Do matrix-matrix multiplication
FOR_MATMATMUL_OUTER:
			for (int l = 0; l < BLOCKDIM; l++)
			{
#pragma HLS UNROLL skip_exit_check
//#pragma HLS PIPELINE off
//#pragma HLS LOOP_FLATTEN off
FOR_MATMATMUL_INNER:
				for (int m = 0; m < BLOCKDIM; m++)
				{
#pragma HLS UNROLL skip_exit_check
//#pragma HLS PIPELINE off
//#pragma HLS LOOP_FLATTEN off
					//t_weight temp = 0;
//#pragma HLS DEPENDENCE variable=temp pointer intra RAW true
					dotProduct<t_weight, BLOCKDIM>(weightBuffer[m], inputBuffer[l], &outputBuffer[l][m]);
					//outputBuffer[l][m] = temp;
				}
			}
		}
      //TODO: Compute activation
      //output[b*num_outputs+o] = std::max(0.0f, output[b*num_outputs+o]);

		//Add bias and compute activation, then write to buffer
FOR_STREAMOUT_OUTER:
		for (int l = 0; l < BLOCKDIM; l++)
		{
#pragma HLS PIPELINE
//#pragma HLS LOOP_FLATTEN off
FOR_STREAMOUT_INNNER:
		for (int m = 0; m < BLOCKDIM; m++)
			{
//#pragma HLS LOOP_FLATTEN off
				if (o+m < num_outputs && b+l < batch_size)
				{
					t_output temp = (t_output) outputBuffer[l][m] + (t_output) biases[o+m];
//#pragma HLS RESOURCE variable=temp core=AddSub
					output[(b+l)*num_outputs + o + m] = (float) std::max((float) 0.0, (float) (temp));
				}
			}
		}
    }
  }
#endif

}
