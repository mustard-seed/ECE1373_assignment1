############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
open_project conv_proj
set_top conv_layer
add_files ../conv_test/conv_layer.cpp -cflags "-I ../.  -DCONV_DEFAULT"
add_files -tb ../nn_params
add_files -tb ../util/shared.cpp
add_files -tb ../conv_test/conv_layer_test.cpp -cflags "-I../."
open_solution "solution_short"
set_part {xcvu095-ffvc1517-2-e}
create_clock -period 10 -name default
#source "./conv_proj/solution_short/directives.tcl"
csim_design -clean -compiler gcc
csynth_design
cosim_design
export_design -format ip_catalog
