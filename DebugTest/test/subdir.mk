################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/video_decoder_test.cpp 

OBJS += \
./test/video_decoder_test.o 

CPP_DEPS += \
./test/video_decoder_test.d 


# Each subdirectory must supply rules for building sources it contributes
test/%.o: ../test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/lib/jvm/default-java/include -I/usr/lib/jvm/default-java/include/linux -I"/ssd/workspace/ISEE/LaS-VPE Platform/Video-Decoder/inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


