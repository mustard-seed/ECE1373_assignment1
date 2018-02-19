############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
open_project conv_gemm
set_top conv_layer
add_files ../conv_test/conv_layer.cpp -cflags "-DCONV_GEMM -I../dot_test/."
add_files ../dot_test/dotProduct.cpp
add_files -tb ../conv_test/conv_layer_test.cpp -cflags "-I../."
add_files -tb ../util/shared.cpp
add_files -tb ../util/shared.h
add_files -tb ../nn_params
open_solution "solution_gemm"
set_part {xcvu095-ffvc1517-2-e} -tool vivado
create_clock -period 10 -name default
source "./conv_gemm/solution_gemm/directives.tcl"
csim_design -compiler gcc
csynth_design
cosim_design -O -trace_level port
export_design -format ip_catalog
