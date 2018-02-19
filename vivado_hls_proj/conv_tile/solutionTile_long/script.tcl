############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
open_project conv_tile
set_top conv_layer
add_files ../dot_test/dotProduct.cpp
add_files ../conv_test/conv_layer.cpp -cflags "-DCONV_TILE -I../. -I../dot_test/."
add_files -tb ../nn_params
add_files -tb ../util/shared.h
add_files -tb ../util/shared.cpp
add_files -tb ../conv_test/conv_layer_test.cpp -cflags "-I../. -I../dot_test"
open_solution "solutionTile_long"
set_part {xcvu095-ffvc1517-2-e} -tool vivado
create_clock -period 10 -name default
source "./conv_tile/solutionTile_long/directives.tcl"
csim_design -clean -compiler gcc -setup
csynth_design
cosim_design -O -trace_level port
export_design -format ip_catalog
