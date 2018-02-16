############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2017 Xilinx, Inc. All Rights Reserved.
############################################################
open_project dotProduct_prj
set_top dotProductWrapper
add_files ../dot_test/dotProduct.cpp
add_files -tb ../dot_test/dotProductTest.cpp
open_solution "solution2"
set_part {xcvu095-ffvc1517-2-e}
create_clock -period 10 -name default
#source "./dotProduct_prj/solution2/directives.tcl"
csim_design -clean -compiler gcc -setup
csynth_design
cosim_design
export_design -format ip_catalog
