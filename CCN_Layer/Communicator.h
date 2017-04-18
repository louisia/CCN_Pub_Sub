/*
 * Communicator.h
 *
 *  Created on: Sep 13, 2015
 *      Author: njnu
 */

#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include "Structure.h"

#include <string>
#include <fstream>

class Communicator {
public:
	Communicator(std::string localIP);
	virtual ~Communicator();

	/*
	 * function: get the port of current socket
	 * return value: the port number
	 */
	int getSocketPort();

	/*
	 * function: set the port of the current socket
	 * parameters: port: the port number to set
	 */
	void setSocketPort(int port);


	/*
	 * function: receive the message and return the address of the sender
	 * parameters: buffer: the buffer to store the message received
	 *             sizeOfBuffer: the size of the buffer
	 *             addr: the address of the sender
	 *             sizeOfAddr: the size of the addr
	 * return value: return the size of the message received if success, otherwise return -1
	 */
	int recvFrom(char* buffer, int sizeOfBuffer, sockaddr_in6& addr,
			socklen_t& sizeOfAddr);

	/*
	 * function: send the message to the destination address
	 * parameters: buffer: the buffer of message to send
	 *             sizeOfBuffer: the size of the message
	 *             addr: the address of receiver
	 *             sizeOfAddr: the size of addr
	 * return value: return the size of message sent if success, otherwise return -1
	 */
	int sendTo(char* buffer, int sizeOfBuffer, sockaddr_in6 addr,
			socklen_t sizeOfAddr,MessageType type);


private:
	/*
	 * function: set the socket timeout
	 * parameters: time : the second of the time to wait
	 */
	void setSocketTimeout(int time);
	/*
	 * function: initialize the socket
	 */
	void initCommunicator();

	/*
	 * function: get the local port usable
	 */
	void getPortUsable();

protected:
	int m_socket;                                    //the socket to communicate
	int m_port;                                         //the port of the socket
	std::string m_ip;                                     //the IP of the socket

};

#endif /* COMMUNICATOR_H_ */
