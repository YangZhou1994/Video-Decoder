################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/VideoDecoderJni.cpp \
../src/VpeVideoDecoder.cpp \
../src/decoderjni.cpp 

OBJS += \
./src/VideoDecoderJni.o \
./src/VpeVideoDecoder.o \
./src/decoderjni.o 

CPP_DEPS += \
./src/VideoDecoderJni.d \
./src/VpeVideoDecoder.d \
./src/decoderjni.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/usr/lib/jvm/default-java/include/linux -I/usr/lib/jvm/default-java/include -I"/home/zy/workspace/mem-decoding/lib" -I"/home/zy/workspace/mem-decoding/inc" -O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


