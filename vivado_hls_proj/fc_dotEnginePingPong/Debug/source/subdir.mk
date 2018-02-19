################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/jamesliu/ECE1373/ECE1373_assignment1/dot_test/dotProduct.cpp \
/home/jamesliu/ECE1373/ECE1373_assignment1/fc_test/fc_layer.cpp 

OBJS += \
./source/dotProduct.o \
./source/fc_layer.o 

CPP_DEPS += \
./source/dotProduct.d \
./source/fc_layer.d 


# Each subdirectory must supply rules for building sources it contributes
source/dotProduct.o: /home/jamesliu/ECE1373/ECE1373_assignment1/dot_test/dotProduct.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DAESL_TB -D__llvm__ -D__llvm__ -I/opt/Xilinx/Vivado_HLS/2017.2/lnx64/tools/auto_cc/include -I/opt/Xilinx/Vivado_HLS/2017.2/lnx64/tools/systemc/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -I/home/jamesliu/ECE1373/ECE1373_assignment1/. -I/home/jamesliu/ECE1373/ECE1373_assignment1/vivado_hls_proj -I/opt/Xilinx/Vivado_HLS/2017.2/include/etc -I/opt/Xilinx/Vivado_HLS/2017.2/include/ap_sysc -I/home/jamesliu/ECE1373/ECE1373_assignment1/dot_test -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

source/fc_layer.o: /home/jamesliu/ECE1373/ECE1373_assignment1/fc_test/fc_layer.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DFC_DOTENGPINGPONG -I../dot_test/. -I/opt/Xilinx/Vivado_HLS/2017.2/lnx64/tools/auto_cc/include -I/opt/Xilinx/Vivado_HLS/2017.2/lnx64/tools/systemc/include -I/opt/Xilinx/Vivado_HLS/2017.2/include -I/home/jamesliu/ECE1373/ECE1373_assignment1/. -I/home/jamesliu/ECE1373/ECE1373_assignment1/vivado_hls_proj -I/opt/Xilinx/Vivado_HLS/2017.2/include/etc -I/opt/Xilinx/Vivado_HLS/2017.2/include/ap_sysc -I/home/jamesliu/ECE1373/ECE1373_assignment1/dot_test -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"source/fc_layer.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


