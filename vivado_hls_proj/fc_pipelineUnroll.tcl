open_project fc_pipelineUnroll
set_top fc_layer
add_files ../fc_test/fc_layer.cpp -cflags "-DFC_PIPELINEUNROLL"
add_files -tb ../fc_test/fc_layer_test.cpp -cflags "-I ../"
add_files -tb ../util/shared.cpp
add_files -tb ../nn_params
open_solution "solution1"
set_part {xcvu095-ffvc1517-2-e} -tool vivado
create_clock -period 10 -name default
csim_design -compiler gcc
csynth_design
#cosim_design
#export_design -format ip_catalog
