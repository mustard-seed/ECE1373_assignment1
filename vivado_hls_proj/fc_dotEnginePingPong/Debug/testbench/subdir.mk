################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/jamesliu/ECE1373/ECE1373_assignment1/fc_test/fc_layer_test.cpp \
/home/jamesliu/ECE1373/ECE1373_assignment1/util/shared.cpp 

OBJS += \
./testbench/fc_layer_test.o \
./testbench/shared.o 

CPP_DEPS += \
./testbench/fc_layer_test.d \
./testbench/shared.d 


# Each subdirectory must supply rules for building sources it contributes
testbench/fc_layer_test.o: /home/jamesliu/ECE1373/ECE1373_assignment1/fc_test/fc_layer_test.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../../../. -I../../../dot_test -I/opt/Xilinx/Vivado_HLS/2017.2/lnx64/tools/auto_cc/include -I/opt/Xilinx/Vivado_HLS/2017.2/lnx64/tools/systemc/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -I/home/jamesliu/ECE1373/ECE1373_assignment1/. -I/home/jamesliu/ECE1373/ECE1373_assignment1/vivado_hls_proj -I/opt/Xilinx/Vivado_HLS/2017.2/include/etc -I/opt/Xilinx/Vivado_HLS/2017.2/include/ap_sysc -I/home/jamesliu/ECE1373/ECE1373_assignment1/dot_test -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"testbench/fc_layer_test.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

testbench/shared.o: /home/jamesliu/ECE1373/ECE1373_assignment1/util/shared.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DAESL_TB -D__llvm__ -D__llvm__ -I/opt/Xilinx/Vivado_HLS/2017.2/lnx64/tools/auto_cc/include -I/opt/Xilinx/Vivado_HLS/2017.2/lnx64/tools/systemc/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -I/home/jamesliu/ECE1373/ECE1373_assignment1/. -I/home/jamesliu/ECE1373/ECE1373_assignment1/vivado_hls_proj -I/opt/Xilinx/Vivado_HLS/2017.2/include/etc -I/opt/Xilinx/Vivado_HLS/2017.2/include/ap_sysc -I/home/jamesliu/ECE1373/ECE1373_assignment1/dot_test -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


