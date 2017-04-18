################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CCN_Layer/Buffer.cpp \
../CCN_Layer/Checker.cpp \
../CCN_Layer/CheckerImp.cpp \
../CCN_Layer/Communicator.cpp \
../CCN_Layer/ControlPanel.cpp \
../CCN_Layer/Controller.cpp \
../CCN_Layer/DataPanel.cpp \
../CCN_Layer/Function.cpp \
../CCN_Layer/MsgReceiver.cpp \
../CCN_Layer/MsgSender.cpp \
../CCN_Layer/Node.cpp \
../CCN_Layer/Panel.cpp \
../CCN_Layer/Reformer.cpp \
../CCN_Layer/Slicer.cpp \
../CCN_Layer/SubListDownloader.cpp \
../CCN_Layer/SubListUploader.cpp \
../CCN_Layer/Threading.cpp \
../CCN_Layer/Topology.cpp \
../CCN_Layer/TopologyNode.cpp 

OBJS += \
./CCN_Layer/Buffer.o \
./CCN_Layer/Checker.o \
./CCN_Layer/CheckerImp.o \
./CCN_Layer/Communicator.o \
./CCN_Layer/ControlPanel.o \
./CCN_Layer/Controller.o \
./CCN_Layer/DataPanel.o \
./CCN_Layer/Function.o \
./CCN_Layer/MsgReceiver.o \
./CCN_Layer/MsgSender.o \
./CCN_Layer/Node.o \
./CCN_Layer/Panel.o \
./CCN_Layer/Reformer.o \
./CCN_Layer/Slicer.o \
./CCN_Layer/SubListDownloader.o \
./CCN_Layer/SubListUploader.o \
./CCN_Layer/Threading.o \
./CCN_Layer/Topology.o \
./CCN_Layer/TopologyNode.o 

CPP_DEPS += \
./CCN_Layer/Buffer.d \
./CCN_Layer/Checker.d \
./CCN_Layer/CheckerImp.d \
./CCN_Layer/Communicator.d \
./CCN_Layer/ControlPanel.d \
./CCN_Layer/Controller.d \
./CCN_Layer/DataPanel.d \
./CCN_Layer/Function.d \
./CCN_Layer/MsgReceiver.d \
./CCN_Layer/MsgSender.d \
./CCN_Layer/Node.d \
./CCN_Layer/Panel.d \
./CCN_Layer/Reformer.d \
./CCN_Layer/Slicer.d \
./CCN_Layer/SubListDownloader.d \
./CCN_Layer/SubListUploader.d \
./CCN_Layer/Threading.d \
./CCN_Layer/Topology.d \
./CCN_Layer/TopologyNode.d 


# Each subdirectory must supply rules for building sources it contributes
CCN_Layer/%.o: ../CCN_Layer/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


