/*
 * Declaration of types for assignment 1
 */
#ifndef TYPES
#define TYPES
#include "ap_int.h"
#include "ap_fixed.h"

#define SHORT  //Used for short solutions only!!!!

#if !defined(SHORT)
typedef float type;
#else
typedef ap_fixed<24,10, AP_RND_CONV, AP_SAT> type;
#endif


#endif
