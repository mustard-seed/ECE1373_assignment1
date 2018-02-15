############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
open_project fc_proj
set_top fc_layer
add_files ../fc_test/fc_layer.cpp -cflags "-DFC_DEFAULT"
add_files -tb ../nn_params
add_files -tb ../util/shared.cpp
add_files -tb ../fc_test/fc_layer_test.cpp -cflags "-I../"
open_solution "solution_boundAndPipeline"
set_part {xcvu095-ffvc1517-2-e}
create_clock -period 10 -name default
#source "./fc_proj/solution_boundAndPipeline/directives.tcl"
csim_design -compiler gcc -setup
csynth_design
cosim_design
export_design -format ip_catalog
