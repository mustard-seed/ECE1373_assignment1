#include <algorithm>
#include <cassert>
#include "fc_layer.h"
#include "types.h"
/*
 * ECE1373 Code begins
 */
#define PRAGMA_SUB(x) _Pragma (#x)
#define PRAGMA_HLS(x) PRAGMA_SUB(x)
/*
 * ECE1373 Code ends
 */

void fc_layer(t_weight weights[MAX_INPUT_SIZE*MAX_OUTPUT_SIZE],
              t_bias biases[MAX_OUTPUT_SIZE],
              t_input input[MAX_INPUT_SIZE*MAX_BATCH],
              t_output output[MAX_OUTPUT_SIZE*MAX_BATCH],
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

        output[b*num_outputs+o] += input[b*num_inputs+i]*weights[o*num_inputs+i];
      }

      // Compute activation
      output[b*num_outputs+o] = std::max(0.0f, output[b*num_outputs+o]);
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

//Set buffer for wegiths
      t_weight *pbuff_weights = &weights[o*num_inputs];

FOR_INPUT:
      // Accumulate weighted sum
      for (int i = 0; i < num_inputs; i++) {

        output[b*num_outputs+o] += input[b*num_inputs+i]*pbuff_weights[i];
      }
      // Compute activation
      output[b*num_outputs+o] = std::max(0.0f, output[b*num_outputs+o]);
    }
  }
#endif
}
