/*
 * MsgReceiver.cpp
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#include "MsgReceiver.h"
#include "Reformer.h"
#include "Slicer.h"

#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>

MsgReceiver::MsgReceiver(Buffer& buffer, std::string localIP) :
		m_msgBuffer(buffer), m_communicator(localIP), m_localIP(localIP){
	// TODO Auto-generated constructor stub

	initMsgReceiver();
}

MsgReceiver::~MsgReceiver() {
	// TODO Auto-generated destructor stub
}

/*
 * function: initialize the socket to communicate and the transmission face table
 */
void MsgReceiver::initMsgReceiver() {

}

/*
 * function: set the transmission face table
 * parameters: transFaceTable: the set of socket address of adjacent
 */
void MsgReceiver::setTransFaceTable(std::vector<sockaddr_in6>& transFaceTable) {
	m_transmissionFaceTable = transFaceTable;
}

/*
 * function: get the transmission face table
 * return value: the transmission face table of the receiver
 */
std::vector<sockaddr_in6> MsgReceiver::getTransFaceTable() {
	return m_transmissionFaceTable;
}

/*
 * function: get the local ip
 */
std::string MsgReceiver::getLocalIP() {
	return m_localIP;
}

void MsgReceiver::setPort(int port) {
	m_communicator.setSocketPort(port);
}

int MsgReceiver::getPort() {
	return m_communicator.getSocketPort();
}

/*
 * function: get the faceID by the socket address
 * parameters: addr: the address to find
 * return value: return the faceID of the addr if found, otherwise return -1
 */
FaceID MsgReceiver::getFaceIDByAddr(sockaddr_in6 addr) {

	for (std::vector<sockaddr_in6>::size_type i = 0;
			i != m_transmissionFaceTable.size(); ++i) {
		if (memcmp((char*) &addr, (char*) &m_transmissionFaceTable[i],
				sizeof(sockaddr_in6)) == 0) {
			return i;
		}
	}

	return -1;
}

void MsgReceiver::running() {

	Reformer reformer(m_msgBuffer);
	Slice slice;

	//the address of sender
	sockaddr_in6 senderAddr;
	socklen_t addrLen = 0;

	int result = 0;

	//the buffer of message received
	char recevBuffer[ SLICER_SIZE + sizeof(Slice) - sizeof(char*)];

	int i = 0;

	while (true) {
		//receive the message
		addrLen = sizeof(sockaddr_in6);
		memset(&senderAddr, 0, sizeof(sockaddr_in6));

		result = m_communicator.recvFrom(recevBuffer, sizeof(recevBuffer),
				senderAddr, addrLen);

		if (-1 == result) {
			std::cerr << "recev failed..." << strerror( errno) << " errno: "
					<< errno << std::endl;

			continue;
		}

		//recovery the slice
		memcpy(&slice, recevBuffer, sizeof(Slice) - sizeof(char*));
		slice._sliceData = recevBuffer + sizeof(Slice) - sizeof(char*);

		//colloct the slice
		FaceID faceID = getFaceIDByAddr(senderAddr);

		reformer.putSlice(slice, faceID);


	}
}
