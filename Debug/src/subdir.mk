################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/VideoDecoderJni.cpp \
../src/VpeVideoDecoder.cpp 

OBJS += \
./src/VideoDecoderJni.o \
./src/VpeVideoDecoder.o 

CPP_DEPS += \
./src/VideoDecoderJni.d \
./src/VpeVideoDecoder.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/lib/jvm/default-java/include -I/usr/lib/jvm/default-java/include/linux -I"/ssd/workspace/ISEE/LaS-VPE-Platform/src/native/Video-Decoder/inc" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


