################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/video_decoder_jni.cpp 

OBJS += \
./src/video_decoder_jni.o 

CPP_DEPS += \
./src/video_decoder_jni.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/lib/jvm/default-java/include -I/usr/lib/jvm/default-java/include/linux -I"/ssd/workspace/ISEE/LaS-VPE Platform/Video-Decoder/inc" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


