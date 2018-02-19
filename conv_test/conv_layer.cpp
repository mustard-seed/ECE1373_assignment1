#include <algorithm>
#include <assert.h>
#include "conv_layer.h"
#include <iostream>

#if defined(CONV_TILE) || defined(CONV_TILENOPIPE)
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
	//assert (od <= MAX_OUTPUT_DIMS);
	//assert (ox <= MAX_OUTPUT_WIDTH);
	//assert (oy <= MAX_OUTPUT_HEIGHT);
	//assert (id <= MAX_INPUT_DIMS);
	//assert (ix <= MAX_INPUT_WIDTH);
	//assert (iy <= MAX_INPUT_HEIGHT);
	//assert (k <= MAX_KERNEL_SIZE);
	//assert (s == 1 || s == 2);
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
              // Input X Dimension
FOR_INPUT_DIM_X:
              for (int i_x = xOffset, iix = 0; i_x < xOffset+k; i_x++, iix++)
              {
				#pragma HLS PIPELINE
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
#define GEMM_SIZE 4
//#define NBANKS 1
  /*
   * Convert 2D convolution into matrix-matrix multiplication
   * Reuse the weights!!!!
   * The left matrix is Hf by Wf, where Hf is the number of kernels (output feature maps), and Wf is the
   * number of weights in each kernel.
   * The right matrix is Hi by Wi, where Hi = Wf, and Hi is the number number of elements in the output volume
   * times the number images in the batches.
   */

  /*
   * Set counter limites and define some constants
   */
  int limitWf = k*k*id;
  int limitHf = od;
  int limitWi = b*ox*oy;
  int inputArea = ix*iy;
  int inputVolume = inputArea*id;
  int strideWidthProduct = s*ix;
  int kernelArea = k*k;
  int outputArea = ox*oy;
  int outputVolume = od*ox*oy;

  //int honStride = GEMM_SIZE*NBANKS;

  /*
   * Create the buffers that will be used.
   */
  type weightBuffer[GEMM_SIZE][GEMM_SIZE];
  type inputBuffer[GEMM_SIZE][GEMM_SIZE];
  type outputBuffer[GEMM_SIZE][GEMM_SIZE];
  #pragma HLS ARRAY_PARTITION variable=weightBuffer complete dim = 0
  #pragma HLS ARRAY_PARTITION variable=inputBuffer complete dim = 0
  #pragma HLS ARRAY_PARTITION variable=outputBuffer complete dim = 0


  //Iterate along axis of different kernels
FOR_OUTPUTMTX_HEIGHT:
  for (int countHf = 0; countHf < limitHf; countHf += GEMM_SIZE)
  {
#ifndef __SYNTHESIS__
	  std::cout <<"Hf iterator: "<<countHf<< std::endl;
#endif
FOR_OUTPUTMTX_WIDTH:
	  //Iterate along the axis of the same kernels
	  for (int countWf = 0; countWf < limitWf; countWf += GEMM_SIZE)
	  {
		  //TODO: Load weights into a GEMM_SIZE by GEMM_SIZE block
FOR_LOADWEIGHT_HEIGHT:
		  for (int i= 0; i < GEMM_SIZE; i++)
		  {
#pragma HLS PIPELINE
			  /*
			   * Vertical index coungHf + i of the weight matrix maps to output feature map index k.
			   */
			  int idxHf = countHf + i;
			  int outputFMapOffset = idxHf * id * k * k;
FOR_LOADWEIGHT_WIDTH:
			  for (int j= 0; j < GEMM_SIZE; j++)
			  {
				  /*
				   * TODO: Add comments to explain the map to weight[] later.
				   *
				   */
				  int weightIdx = outputFMapOffset + j + countWf;
				  if (idxHf < limitHf && j + countWf < limitWf )
				  {
					  weightBuffer[i][j] = (type) weights[weightIdx];
				  }
				  else
				  {
					  weightBuffer[i][j] = (type) 0.0f;
				  }
			  }
		  }
		  //Iterate along different input windows
FOR_INPUTMTX_WIDTH:
		  for (int countWi = 0;  countWi < limitWi; countWi += GEMM_SIZE)
		  {
			  //Try to make this part data flow....

			  //TODO: Load inputs
			  //TODO: Initialize/Load output buffers.
FOR_LOADINPUT_OUTPUT_HEIGHT:
			  for (int i= 0; i < GEMM_SIZE; i++)
			  {
#pragma HLS PIPELINE
				  int idxHi = countWf + i;
				  int iContribution = (idxHi/kernelArea)*inputArea
						  + (idxHi % kernelArea) / k * ix + (idxHi % kernelArea) % k;
				  int idxHf = countHf + i;
				  int outputOffset = idxHf * ox * oy;
FOR_LOADINPUT_OUTPUT_WIDTH:
				  for (int j= 0; j < GEMM_SIZE; j++)
				  {
					  /*
					   * TODO: Add comments
					   */
					  int idxWi = countWi + j;
					  int idxBatch = idxWi / outputArea;
					  int idxOy = (idxWi % outputArea) / ox;
					  int idxOx = (idxWi % outputArea) % ox;
					  int idxInput = iContribution +
					  	  	  idxBatch * inputVolume +
							  idxOy * strideWidthProduct +
							  idxOx * s;

					  if ( idxHi < limitWf && idxWi < limitWi)
					  {
						  inputBuffer[i][j] = (type) input[idxInput];
					  }
					  else
					  {
						  inputBuffer[i][j] = (type) 0.0f;
					  }

					  if ( idxHf < limitHf && idxWi < limitWi)
					  {
						  if (countWf == 0)
						  {
							  //Initilialize weight to 0
							  outputBuffer[i][j] = (type) 0.0;
						  }
						  else
						  {
							  outputBuffer[i][j] = (float) output[outputOffset + idxBatch * outputVolume + idxWi % outputArea];
						  }
					  }
					  else
					  {
						  outputBuffer[i][j] = (type) 0.0;
					  }
				  }
			  }

			  //TODO: Do GEMM
			  gemm<type, GEMM_SIZE>(weightBuffer, inputBuffer, outputBuffer);

			  //TODO: Write back to memory. Apply bias, and apply Relu if necessary.
FOR_WRITE_OUTPUT_HEIGHT:
			  for (int i= 0; i < GEMM_SIZE; i++)
			  {
#pragma HLS PIPELINE
				  int idxHf = countHf + i;
				  int outputOffset = idxHf * ox * oy;
FOR_WRITE_OUTPUT_WIDTH:
				  for (int j= 0; j < GEMM_SIZE; j++)
				  {
					  /*
					   * TODO: Add comments
					   */
					  int idxWi = countWi + j;
					  int idxBatch = idxWi / outputArea;

					  if ( idxHf < limitHf && idxWi < limitWi)
					  {
						  if (countWf + GEMM_SIZE >= limitWf)
						  {
							  //Initilialize weight to 0
							  outputBuffer[i][j] += (type) biases[idxHf];
							  output[outputOffset + idxBatch * outputVolume + idxWi % outputArea]
									 = (float) std::max(0.0f, (float) outputBuffer[i][j]);

						  }
						  else
						  {
							  output[outputOffset + idxBatch * outputVolume + idxWi % outputArea]
									 = (float) outputBuffer[i][j];
						  }
					  }
				  }
			  }
		  }
	  }
  }


#endif
#if defined(CONV_TILE) || defined(CONV_TILENOPIPE)
#if defined(CONV_TILE)
#define OFTILE_SIZE 18 //Parallelism in output feature map
#define IFTILE_SIZE 2 //Parallelism in input feature map
#define FLATTILE_SIZE 2 //2D tile size
#elif defined(CONV_TILENOPIPE)
#define OFTILE_SIZE 10 //Parallelism in output feature map
#define IFTILE_SIZE 2 //Parallelism in input feature map
#define FLATTILE_SIZE 2 //2D tile size
#endif

int inputArea = ix*iy;
int inputVolume = inputArea*id;
int strideWidthProduct = s*ix;
int kernelArea = k*k;
int kernelVolume = kernelArea * id;
int outputArea = ox*oy;
int outputVolume = od*ox*oy;
int totalOutput = 0;

type outputBuffer[OFTILE_SIZE];
#pragma HLS array_partition variable=outputBuffer complete dim=0

type inputBuffer[IFTILE_SIZE*FLATTILE_SIZE*FLATTILE_SIZE];
type weightBuffer[OFTILE_SIZE][IFTILE_SIZE*FLATTILE_SIZE*FLATTILE_SIZE];
#pragma HLS array_partition variable=inputBuffer complete dim=0
#pragma HLS array_partition variable=weightBuffer complete dim=0

// Batch
FOR_BATCH:
for (int b_=0; b_< b; b_++)
{
   int batchOffsetOutput;
   batchOffsetOutput = b_*outputVolume;
   int batchOffsetInput = b_*inputVolume;
#ifndef __SYNTHESIS__
  std::cout <<"Batch iterator: "<<b_<<std::endl;
#endif
 // Output Dimensions (Feature Maps)
FOR_OUTPUT_DIM_D:
 for (int o_d = 0; o_d < od; o_d += OFTILE_SIZE)
 {
   int outputKernelOffset;
#pragma HLS RESOURCE variable=outputKernelOffset core=fMul_nodsp
   outputKernelOffset = o_d*outputArea;
   // Output Y Dimension
FOR_OUTPUT_DIM_Y:
   for (int o_y = 0; o_y < oy; o_y ++)
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

FOR_INIT_OUTPUT_BUFFER:
	   for (int oBufferCount = 0; oBufferCount < OFTILE_SIZE; oBufferCount++)
	   {
#pragma HLS PIPELINE
		   outputBuffer[oBufferCount] = (type) 0;
	   }
	   // Weighted Sum:

	   // Input Dimensions (Feature Maps)
FOR_INPUT_DIM_D:
	   for (int i_d = 0; i_d < id; i_d += IFTILE_SIZE)
	   {
	// Input Y Dimension
FOR_INPUT_DIM_Y:
		 for (int i_y = yOffset, iiy = 0; i_y < yOffset+k; i_y += FLATTILE_SIZE, iiy += FLATTILE_SIZE)
		 {
		   // Input X Dimension
FOR_INPUT_DIM_X:
		   for (int i_x = xOffset, iix = 0; i_x < xOffset+k; i_x += FLATTILE_SIZE, iix += FLATTILE_SIZE)
		   {

			//Load input and kernels
FOR_LOAD_INPUTKERNEL_Z:
			   for (int inputCountLoadZ = 0; inputCountLoadZ < IFTILE_SIZE; inputCountLoadZ++)
			   {
#pragma HLS PIPELINE
FOR_LOAD_INPUTKERNEL_Y:
				   for (int countLoadY = 0; countLoadY < FLATTILE_SIZE; countLoadY++)
				   {
					   FOR_LOAD_INPUTKERNEL_X:
					   for (int countLoadX = 0; countLoadX < FLATTILE_SIZE; countLoadX++)
					   {
						   int inputZ = i_d + inputCountLoadZ;
						   int inputY = i_y + countLoadY;
						   int inputX = i_x + countLoadX;
						   int inputIndex = inputCountLoadZ*FLATTILE_SIZE*FLATTILE_SIZE
								   + countLoadY* FLATTILE_SIZE
								   + countLoadX;
						   if (inputZ < id && inputY < iy && inputX < ix)
						   {
							   inputBuffer[inputIndex] =
									   input[batchOffsetInput + inputZ * inputArea + inputY*ix + inputX];
						   }
						   else
						   {
							   inputBuffer[inputIndex] = 0;
						   }

						   int kernelZ = inputZ;
						   int kernelY = iiy + countLoadY;
						   int kernelX = iix + countLoadX;
						   FOR_LOAD_INPUTKERNEL_Z2:
						   for (int countLoadKernel=0; countLoadKernel < OFTILE_SIZE; countLoadKernel++)
						   {
							   int kernelZO = countLoadKernel + o_d;
							   if (kernelZO < od && kernelZ < id && kernelY < k && kernelX < k)
							   {
								   weightBuffer[countLoadKernel][inputIndex] =
										   weights[kernelZO*kernelVolume + kernelZ*kernelArea
								   					+ kernelY*k + kernelX];
							   }
							   else
							   {
								  weightBuffer[countLoadKernel][inputIndex] = 0;
							   }

						   }
					   }
				   }
			   }
			//Compute partial sums
			FOR_COMPUTE:
			for (int kernelCount = 0; kernelCount < OFTILE_SIZE; kernelCount++)
			{
#pragma HLS UNROLL skip_exit_check
//#pragma HLS UNROLL skip_exit_check
//#pragma HLS dependence array inter false
				dotProduct<type,IFTILE_SIZE*FLATTILE_SIZE*FLATTILE_SIZE>
				(inputBuffer, weightBuffer[kernelCount], &outputBuffer[kernelCount]);
			}
		   }
		 }
	   }

	   // Activaton Function
	   FOR_COMPUTE_ACTIVATION:
	   for (int outputCount=0; outputCount < OFTILE_SIZE; outputCount++)
	   {
#pragma HLS PIPELINE

		   if (outputCount + o_d < od)
		   {
			   output[batchOffsetOutput + outputKernelOffset + outputCount*outputArea + outputYOffset + o_x] =
					   std::max( (float)0.0f, (float) outputBuffer[outputCount] + biases[outputCount+o_d]);
			   totalOutput++;
		   }
	   }
	 }
   }
 }
}
//std::cout <<"Number of outputs written "<<totalOutput<<std::endl;
#endif

}

