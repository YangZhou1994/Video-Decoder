################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/decoderjni.cpp \
../src/vpe_decoder.cpp 

OBJS += \
./src/decoderjni.o \
./src/vpe_decoder.o 

CPP_DEPS += \
./src/decoderjni.d \
./src/vpe_decoder.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/lib/jvm/default-java/include/linux -I/usr/lib/jvm/default-java/include -I"/home/zy/workspace/mem-decoding/lib" -I"/home/zy/workspace/mem-decoding/inc" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


