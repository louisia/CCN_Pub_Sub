cmake_minimum_required(VERSION 3.5)

project(PS_RN1)

SET(CCN_Toper	../CCN_Toper/Main.cpp
		../CCN_Toper/EPSRouter.cpp ../CCN_Toper/EPSRouter.h
		../CCN_Toper/MulticastThreading.cpp ../CCN_Toper/MulticastThreading.h		
		../CCN_Toper/PSRouter.cpp ../CCN_Toper/PSRouter.h		
		../CCN_Toper/Publisher.cpp ../CCN_Toper/Publisher.h		
		../CCN_Toper/PublishThreading.cpp ../CCN_Toper/PublishThreading.h
		../CCN_Toper/Router.cpp ../CCN_Toper/Router.h
		../CCN_Toper/RPSRouter.cpp ../CCN_Toper/RPSRouter.h		
		../CCN_Toper/Subscriber.cpp ../CCN_Toper/Subscriber.h		
		../CCN_Toper/SubscribeThreading.cpp ../CCN_Toper/SubscribeThreading.h			     	
		)
SET(CCN_Layer 	../CCN_Layer/Buffer.cpp ../CCN_Layer/Buffer.h
	      	../CCN_Layer/Checker.cpp ../CCN_Layer/Checker.h
              	../CCN_Layer/CheckerImp.cpp ../CCN_Layer/CheckerImp.h
              	../CCN_Layer/Communicator.cpp ../CCN_Layer/Communicator.h
              	../CCN_Layer/Controller.cpp ../CCN_Layer/Controller.h
              	../CCN_Layer/ControlPanel.cpp ../CCN_Layer/ControlPanel.h
              	../CCN_Layer/DataPanel.cpp ../CCN_Layer/DataPanel.h
              	../CCN_Layer/Function.cpp 
              	../CCN_Layer/MsgReceiver.cpp ../CCN_Layer/MsgReceiver.h
              	../CCN_Layer/MsgSender.cpp ../CCN_Layer/MsgSender.h
              	../CCN_Layer/Node.cpp ../CCN_Layer/Node.h
              	../CCN_Layer/Panel.cpp ../CCN_Layer/Panel.h              
	      	../CCN_Layer/Reformer.cpp ../CCN_Layer/Reformer.h
              	../CCN_Layer/Slicer.cpp ../CCN_Layer/Slicer.h
              	../CCN_Layer/Structure.h 
              	../CCN_Layer/SubListDownloader.cpp ../CCN_Layer/SubListDownloader.h
              		../CCN_Layer/SubListUploader.cpp ../CCN_Layer/SubListUploader.h
              	../CCN_Layer/Threading.cpp ../CCN_Layer/Threading.h
              	../CCN_Layer/Topology.cpp ../CCN_Layer/Topology.h
              	../CCN_Layer/TopologyNode.cpp ../CCN_Layer/TopologyNode.h
)

add_executable(PS_RN1 ${CCN_Toper} ${CCN_Layer})

TARGET_LINK_LIBRARIES(PS_RN1 json)
TARGET_LINK_LIBRARIES(PS_RN1 pthread)
TARGET_LINK_LIBRARIES(PS_RN1 crypto)
