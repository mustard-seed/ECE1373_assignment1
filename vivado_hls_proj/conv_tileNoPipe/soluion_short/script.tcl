############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
open_project conv_tileNoPipe
set_top conv_layer
add_files ../dot_test/dotProduct.cpp
add_files ../conv_test/conv_layer.cpp -cflags "-I../. -I../dot_test/. -DCONV_TILENOPIPE"
add_files -tb ../nn_params
add_files -tb ../util/shared.h
add_files -tb ../util/shared.cpp
add_files -tb ../conv_test/conv_layer_test.cpp -cflags "-I../. -I../dot_test"
open_solution "soluion_short"
set_part {xcvu095-ffvc1517-2-e}
create_clock -period 5 -name default
#source "./conv_tileNoPipe/soluion_short/directives.tcl"
csim_design -compiler gcc
csynth_design
cosim_design -O -trace_level port
export_design -format ip_catalog
