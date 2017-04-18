################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CCN_Toper/EPSRouter.cpp \
../CCN_Toper/Main.cpp \
../CCN_Toper/MulticastThreading.cpp \
../CCN_Toper/PSRouter.cpp \
../CCN_Toper/PublishThreading.cpp \
../CCN_Toper/Publisher.cpp \
../CCN_Toper/RPSRouter.cpp \
../CCN_Toper/Router.cpp \
../CCN_Toper/SubscribeThreading.cpp \
../CCN_Toper/Subscriber.cpp 

OBJS += \
./CCN_Toper/EPSRouter.o \
./CCN_Toper/Main.o \
./CCN_Toper/MulticastThreading.o \
./CCN_Toper/PSRouter.o \
./CCN_Toper/PublishThreading.o \
./CCN_Toper/Publisher.o \
./CCN_Toper/RPSRouter.o \
./CCN_Toper/Router.o \
./CCN_Toper/SubscribeThreading.o \
./CCN_Toper/Subscriber.o 

CPP_DEPS += \
./CCN_Toper/EPSRouter.d \
./CCN_Toper/Main.d \
./CCN_Toper/MulticastThreading.d \
./CCN_Toper/PSRouter.d \
./CCN_Toper/PublishThreading.d \
./CCN_Toper/Publisher.d \
./CCN_Toper/RPSRouter.d \
./CCN_Toper/Router.d \
./CCN_Toper/SubscribeThreading.d \
./CCN_Toper/Subscriber.d 


# Each subdirectory must supply rules for building sources it contributes
CCN_Toper/%.o: ../CCN_Toper/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


