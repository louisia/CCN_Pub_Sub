/*
 * MsgSender.cpp
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#include "MsgSender.h"
#include "Slicer.h"

#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>

MsgSender::MsgSender(Buffer& buffer, std::string localIP) :
		m_msgBuffer(buffer), m_communicator(localIP), m_localIP(localIP) {
	// TODO Auto-generated constructor stub

	initMsgSender();
}

MsgSender::~MsgSender() {
	// TODO Auto-generated destructor stub
}

/*
 * function: initialize the socket to communicate and the transmission face table
 */
void MsgSender::initMsgSender() {

}

/*
 * function: set the transmission face table
 * parameters: transFaceTable: the set of socket address of adjacent
 */
void MsgSender::setTransFaceTable(std::vector<sockaddr_in6>& transFaceTable) {
	m_transmissionFaceTable = transFaceTable;
}

/*
 * function: get the transmission face table
 * return value: the transmission face table of the sender
 */
std::vector<sockaddr_in6> MsgSender::getTransFaceTable() {
	return m_transmissionFaceTable;
}

/*
 * function: get the ip of local
 */
std::string MsgSender::getLacalIP() {
	return m_localIP;
}

void MsgSender::setPort(int port) {
	m_communicator.setSocketPort(port);
}

int MsgSender::getPort() {
	return m_communicator.getSocketPort();
}

/*
 * function: the main function of the Sender
 */
void MsgSender::running() {

	Message msg;

	Slicer slicer;

	//the address of message to send
	sockaddr_in6 receiverAddr;

	int result = 0;

	//the buffer of message to send
	char sendBuffer[ SLICER_SIZE + sizeof(Slice) - sizeof(char*)];

	while (true) {
		msg = m_msgBuffer.getNextMsg();

		if (msg._faceID >= m_transmissionFaceTable.size()) {
			std::cerr << "send faceID error..." << msg._faceID << std::endl;
			return;
		}

		slicer.setBuffer(msg._buffer, msg._msgLength);

		while (!slicer.isSliceFinished()) {
			Slice slice = slicer.getNextSlice();

			//copy the message to send
			memcpy(sendBuffer, &slice, sizeof(Slice) - sizeof(char*));
			memcpy(sendBuffer + sizeof(Slice) - sizeof(char*), slice._sliceData,
					slice._sliceSize);

			//send the message
			if (msg._type == IGMP) {
				memset(&receiverAddr, 0, sizeof(receiverAddr));
				receiverAddr.sin6_family = AF_INET6;
				inet_pton(AF_INET6, msg._multicastIp.c_str(),
						&receiverAddr.sin6_addr);
				receiverAddr.sin6_port = htons(GROUPPORT);
				result = m_communicator.sendTo(sendBuffer, sizeof(sendBuffer),
						receiverAddr, sizeof(receiverAddr), IGMP);
			} else {
				receiverAddr = m_transmissionFaceTable[msg._faceID];
				result = m_communicator.sendTo(sendBuffer, sizeof(sendBuffer),
						receiverAddr, sizeof(receiverAddr), UDP);
			}

			if (-1 == result) {
				std::cerr << "send failed..." << strerror( errno) << " errno: "
						<< errno << std::endl;
				break;
			}

		}

	}
}
