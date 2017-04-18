/*
 * Communicator.cpp
 *
 *  Created on: Sep 13, 2015
 *      Author: njnu
 */

#include "Communicator.h"

#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <stdio.h>
#include <openssl/md5.h>

Communicator::Communicator(std::string localIP) {
	m_socket = -1;
	m_port = -1;
	m_ip = localIP;

	initCommunicator();
}

Communicator::~Communicator() {
	// TODO Auto-generated destructor stub
}

/*
 * function: get the port of current socket
 * return value: the port number
 */
int Communicator::getSocketPort() {
	return m_port;
}

/*
 * function: set the port of the current socket
 * parameters: port: the port number to set
 */
void Communicator::setSocketPort(int port) {
	m_port = port;

	//reset the socket
	initCommunicator();
}

/*
 * function: receive the message and return the address of the sender
 * parameters: buffer: the buffer to store the message received
 *             sizeOfBuffer: the size of the buffer
 *             addr: the address of the sender
 *             sizeOfAddr: the size of the addr
 * return value: return the size of the message received if success, otherwise return -1
 */
int Communicator::recvFrom(char* buffer, int sizeOfBuffer, sockaddr_in6& addr,
		socklen_t& sizeOfAddr) {

	int res = 0;

	while (-1
			== (res = recvfrom(m_socket, buffer, sizeOfBuffer, 0,
					(sockaddr*) &addr, &sizeOfAddr))) {
		std::cerr << m_ip << ":" << m_port << " " << strerror( errno)
				<< " errno: " << errno << std::endl;
	}

	if (res != -1) {

		PacketID sliceIdAck;
		MD5((unsigned char*) buffer, sizeOfBuffer,
				(unsigned char*) &sliceIdAck);

		//send back the ack
		int result = sendto(m_socket, sliceIdAck._id, sizeof(PacketID), 0,
				(sockaddr*) &addr, sizeOfAddr);
		if (-1 == result) {
			std::cerr << m_ip << ":" << m_port
					<< " recevFrom send ack message failed..."
					<< strerror( errno) << " errno: " << errno << std::endl;

			return -1;
		}
	}

	return res;
}

/*
 * function: send the message to the destination address
 * parameters: buffer: the buffer of message to send
 *             sizeOfBuffer: the size of the message
 *             addr: the address of receiver
 *             sizeOfAddr: the size of addr
 * return value: return the size of message sent if success, otherwise return -1
 */
int Communicator::sendTo(char* buffer, int sizeOfBuffer, sockaddr_in6 addr,
		socklen_t sizeOfAddr, MessageType type) {

	int tryNum = 10;

	PacketID sliceId, sliceIdAck;
	MD5((unsigned char*) buffer, sizeOfBuffer, (unsigned char*) &sliceId);

	while (tryNum > 0) {

		int result = sendto(m_socket, buffer, sizeOfBuffer, 0,
				(sockaddr*) &addr, sizeOfAddr);
		if (-1 != result) {
			//wait the ack from the receiver and compare the source of the ack by comparing the address
			sockaddr_in6 addrbak;
			socklen_t sizeOfAddrbak = sizeof(sockaddr_in6);

			memset(&addrbak, 0, sizeof(sockaddr_in6));

			while (true) {
				int res = recvfrom(m_socket, sliceIdAck._id, sizeof(PacketID),
						0, (sockaddr*) &addrbak, &sizeOfAddrbak);

				if(ntohs(addr.sin6_port)==9500){
					cout<<"好好"<<endl;
				}


				if (type == UDP) {
					if (res != -1
							&& memcmp(sliceId._id, sliceIdAck._id,
									sizeof(PacketID) - 1) == 0
							&& memcmp(&addrbak, &addr, sizeof(addrbak)) == 0) {
						char temp[128];
						bzero(temp,0);
						inet_ntop(AF_INET6, &addr.sin6_addr, temp,
								sizeof(temp));
						std::cout << m_ip << ":" << m_port
								<< " sendTo: recev ack succeed..."
								//							<< strerror( errno) << " errno: " << errno << " "
								<< temp << ": " << ntohs(addr.sin6_port)
								<< std::endl;
						return result;
					}
				} else if (type == IGMP) { //addr is group ip,while addrbak is host ip
					if (res != -1
							&& memcmp(sliceId._id, sliceIdAck._id,
									sizeof(PacketID) - 1) == 0) {

						return result;
					}
				}

				if (res == -1) {
					char temp[128];
					bzero(temp,0);
					inet_ntop(AF_INET6, &addr.sin6_addr, temp, sizeof(temp));
					std::cerr << m_ip << ":" << m_port
							<< " sendTo: recev ack failed..."
//							<< strerror( errno) << " errno: " << errno << " "
							<< temp << ": " << ntohs(addr.sin6_port)
							<< std::endl;
					break;
				}
			}
		}

		std::cerr << "sendTofailed..." << strerror( errno) << " errno: "
				<< errno << std::endl;
		char temp[128];
		bzero(temp,0);
		inet_ntop(AF_INET6, &addr.sin6_addr, temp, sizeof(temp));
		std::cerr << temp << ":" << ntohs(addr.sin6_port) << "," << m_ip << ":"
				<< m_port << std::endl;
		//here can sleep to wait some time to reduce the congestion of the network
		std::cerr << " sleep and try again: " << tryNum << std::endl;
		sleep(10 * SOCK_TIMEOUT / tryNum);
		tryNum--;
	}
	return -1;
}

/*
 * function: set the socket timeout
 * parameters: time : the second of the time to wait
 */
void Communicator::setSocketTimeout(int time) {
	if (-1 == m_socket) {
		std::cerr << "the socket does not initialized..." << std::endl;
		return;
	}

	timeval tv;
	tv.tv_sec = time;
	tv.tv_usec = 0;

	//set the option of the socket
	if (-1 == setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))) {
		std::cerr << "the socket option set error..." << strerror( errno)
				<< " errno: " << errno << std::endl;
	}
}

/*
 * function: get the local port usable
 */
void Communicator::getPortUsable() {
	if (-1 == m_port)
		m_port = PORTMIN;

	//the local address
	sockaddr_in6 localAddr;

	localAddr.sin6_family = AF_INET6;
	inet_pton(AF_INET6, m_ip.c_str(), &localAddr.sin6_addr);

	int result = 0;

	while (m_port <= PORTMAX) {
		localAddr.sin6_port = htons(m_port);

		//try to bind the address with the port
		result = bind(m_socket, (sockaddr*) &localAddr, sizeof(localAddr));
		if (-1 == result) {
//			std::cerr << "bind with port:" << m_port << " failed..."
//					<< strerror( errno) << " errno: " << errno << std::endl;
			m_port++;
		} else {
//			std::cout << "bind success..." << m_port << std::endl;
			break;
		}
	}
}

/*
 * function: initialize the socket
 */
void Communicator::initCommunicator() {
	if (-1 != m_socket) {
		close(m_socket);

		m_socket = -1;
	}

	//create the socket
	m_socket = socket( AF_INET6, SOCK_DGRAM, 0);
	if (-1 == m_socket) {
		std::cerr << "socket create failed..." << strerror( errno) << " errno: "
				<< errno << std::endl;
		exit(1);
	} else {
//		std::cout << "socket create succeed..." << std::endl;
	}
	//get the local Port usable
	getPortUsable();
	//set the time out option of the socket
	setSocketTimeout( SOCK_TIMEOUT);
}

