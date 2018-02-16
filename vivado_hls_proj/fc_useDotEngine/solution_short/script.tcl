############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
open_project fc_useDotEngine
set_top fc_layer
add_files ../fc_test/fc_layer.cpp -cflags "-I../dot_test/. ../. -DFC_DOTENG"
add_files ../dot_test/dotProduct.cpp
add_files -tb ../nn_params
add_files -tb ../fc_test/types.h
add_files -tb ../util/shared.h
add_files -tb ../util/shared.cpp
add_files -tb ../fc_test/fc_layer_test.cpp -cflags "-I../. -I../dot_test"
open_solution "solution_short"
set_part {xcvu095-ffvc1517-2-e}
create_clock -period 5 -name default
#source "./fc_useDotEngine/solution_short/directives.tcl"
csim_design -clean -compiler gcc -setup
csynth_design
cosim_design -O -trace_level all -tool xsim
export_design -format ip_catalog
