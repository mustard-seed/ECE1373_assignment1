#include <algorithm>
#include <assert.h>
#include "conv_layer.h"
#include <iostream>

/*
 * ECE1373 Code begins
 */
#define PRAGMA_SUB(x) _Pragma (#x)
#define PRAGMA_HLS(x) PRAGMA_SUB(x)
/*
 * ECE1373 Code ends
 */

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
	            const int k)
{
	assert (b <= MAX_BATCH);
	assert (od <= MAX_OUTPUT_DIMS);
	assert (ox <= MAX_OUTPUT_WIDTH);
	assert (oy <= MAX_OUTPUT_HEIGHT);
	assert (id <= MAX_INPUT_DIMS);
	assert (ix <= MAX_INPUT_WIDTH);
	assert (iy <= MAX_INPUT_HEIGHT);
	assert (k <= MAX_KERNEL_SIZE);
	assert (s == 1 || s == 2);
#ifdef CONV_DEFAULT
  // Batch
FOR_BATCH:
  for (int b_=0; b_< b; b_++)
  {
#ifndef __SYNTHESIS__
	  std::cout <<"Batch iterator: "<<b_<<std::endl;
#endif
    // Output Dimensions (Feature Maps)
FOR_OUTPUT_DIM_D:
    for (int o_d = 0; o_d < od; o_d++)
    {
      // Output Y Dimension
FOR_OUTPUT_DIM_Y:
      for (int o_y = 0; o_y < oy; o_y++)
      {
        // Output X Dimension
FOR_OUTPUT_DIM_X:
        for (int o_x = 0; o_x < ox; o_x++)
        {
          // Set bias 
          output[b_*od*ox*oy +o_d*ox*oy + o_y*ox + o_x] = biases[o_d];

          // Weighted Sum:
   
          // Input Dimensions (Feature Maps)
FOR_INPUT_DIM_D:
          for (int i_d = 0; i_d < id; i_d++)
          {
       // Input Y Dimension
FOR_INPUT_DIM_Y:
            for (int i_y = o_y*s, iiy = 0; i_y < o_y*s+k; i_y++, iiy++)
            {
            	PRAGMA_HLS (HLS LOOP_TRIPCOUNT max=MAX_KERNEL_SIZE)
              // Input X Dimension
FOR_INPUT_DIM_X:
              for (int i_x = o_x*s, iix = 0; i_x < o_x*s+k; i_x++, iix++)
              {
            	  PRAGMA_HLS (HLS LOOP_TRIPCOUNT max=MAX_KERNEL_SIZE)
                output[b_*od*ox*oy + o_d*ox*oy + o_y*ox + o_x] = (type) output[b_*od*ox*oy + o_d*ox*oy + o_y*ox + o_x] +
                       ( (type) input[b_*id*ix*iy + i_d*ix*iy + i_y*ix + i_x] *
                       (type) weights[o_d*id*k*k + i_d*k*k + iiy*k + iix]);
              }
            }
          }

          // Activaton Function
          output[b_*od*ox*oy + o_d*ox*oy + o_y*ox + o_x] = 
                 std::max( (type)0.0f, (type) output[b_*od*ox*oy + o_d*ox*oy + o_y*ox + o_x]);
        }
      }
    }
  }
#endif
#ifdef CONV_PIPELINEINNER
  // Batch
FOR_BATCH:
  for (int b_=0; b_< b; b_++)
  {
      int batchOffset;
#pragma HLS RESOURCE variable=batchOffset core=fMul_nodsp
      batchOffset = b_*od*ox*oy;

#ifndef __SYNTHESIS__
	  std::cout <<"Batch iterator: "<<b_<<std::endl;
#endif
    // Output Dimensions (Feature Maps)
FOR_OUTPUT_DIM_D:
    for (int o_d = 0; o_d < od; o_d++)
    {
      int outputKernelOffset;
#pragma HLS RESOURCE variable=outputKernelOffset core=fMul_nodsp
      outputKernelOffset = o_d*ox*oy;
      // Output Y Dimension
FOR_OUTPUT_DIM_Y:
      for (int o_y = 0; o_y < oy; o_y++)
      {
		    int yOffset;
#pragma HLS RESOURCE variable=yOffset core=fMul_nodsp
		    yOffset = o_y*s;

	        int outputYOffset;
#pragma HLS RESOURCE variable=outputYOffset core=fMul_nodsp
	        outputYOffset = o_y*ox;
        // Output X Dimension
FOR_OUTPUT_DIM_X:
        for (int o_x = 0; o_x < ox; o_x++)
        {
        	int xOffset;
#pragma HLS RESOURCE variable=yOffset core=fMul_nodsp
		    xOffset = o_x*s;
          // Set bias

          //output[b_*od*ox*oy +o_d*ox*oy + o_y*ox + o_x] = biases[o_d];
          type outputBuffer = biases[o_d];
          // Weighted Sum:

          // Input Dimensions (Feature Maps)
FOR_INPUT_DIM_D:
          for (int i_d = 0; i_d < id; i_d++)
          {
       // Input Y Dimension
FOR_INPUT_DIM_Y:
            for (int i_y = yOffset, iiy = 0; i_y < yOffset+k; i_y++, iiy++)
            {
            	PRAGMA_HLS (HLS LOOP_TRIPCOUNT max=MAX_KERNEL_SIZE)
              // Input X Dimension
FOR_INPUT_DIM_X:
              for (int i_x = xOffset, iix = 0; i_x < xOffset+k; i_x++, iix++)
              {
				#pragma HLS PIPELINE
            	  PRAGMA_HLS (HLS LOOP_TRIPCOUNT max=MAX_KERNEL_SIZE)
				  outputBuffer = (type) outputBuffer +
                       ( (type) input[b_*id*ix*iy + i_d*ix*iy + i_y*ix + i_x] *
                       (type) weights[o_d*id*k*k + i_d*k*k + iiy*k + iix]);
              }
            }
          }

          // Activaton Function
          output[batchOffset +outputKernelOffset + outputYOffset + o_x] =
                 std::max( (type)0.0f, (type) outputBuffer);
        }
      }
    }
  }
#endif
#ifdef CONV_GEMM
#define GEMM_SIZE 8
#define NBANKS 3;
  /*
   * Convert 2D convolution into matrix-matrix multiplication
   * Reuse the weights!!!!
   * The left matrix is Hf by Wf, where Hf is the number of kernels (output feature maps), and Wf is the
   * number of weights in each kernel.
   * The right matrix is Hi by Wi, where Hi = Wf, and Hi is the number number of elements in the output volume
   * times the number images in the batches.
   */

  /*
   * Set counter limites
   */
  int limitWf = k*k*id;
  int limitHf = od;
  int limitWi = b*ox*oy;
  //int honStride = GEMM_SIZE*NBANKS;

  //Iterate along axis of different kernels
  for (int countHf = 0; countHf < limitHf; countHf += GEMM_SIZE)
  {
	  //Iterate along the axis of the same kernels
	  for (int countWf = 0; countWf < limitWf; countWf += GEMM_SIZE)
	  {

		  //TODO: Load weights into a GEMM_SIZE by GEMM_SIZE block

		  //Iterate along different input windows
		  for (int countWi = 0;  countWi < limitWi; countWi += GEMM_SIZE)
		  {
			  //Try to make this part data flow....

			  //TODO: Load inputs

			  //TODO: Initialize/Load output buffers.

			  //TODO: Do GEMM

			  //TODO: Write output buffer to output. Apply ReLu conditionally.

		  }
	  }
  }


#endif
}

