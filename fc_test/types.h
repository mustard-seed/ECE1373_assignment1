/*
 * Declaration of types for assignment 1
 */
#ifndef TYPES
#define TYPES
#include "ap_int.h"
#include "ap_fixed.h"

//#define SHORT  //Used for short solutions only!!!!

#if !defined(SHORT)
typedef float t_input;
typedef float t_output;
typedef float t_weight;
typedef float t_bias;
#else
typedef ap_fixed<16,8, AP_RND_CONV, AP_SAT> t_input;
typedef ap_fixed<16,8, AP_RND_CONV, AP_SAT> t_output;
typedef ap_fixed<16,8, AP_RND_CONV, AP_SAT> t_weight;
typedef ap_fixed<16,8, AP_RND_CONV, AP_SAT> t_bias;
#endif


#endif
