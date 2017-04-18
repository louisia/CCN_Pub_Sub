/*
 * MsgReceiver.h
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#ifndef MSGRECEIVER_H_
#define MSGRECEIVER_H_

#include "Buffer.h"
#include "Threading.h"
#include "Communicator.h"
#include "Reformer.h"

#include <string>
#include <vector>

class MsgReceiver: public Threading {
public:
	MsgReceiver(Buffer& buffer, std::string localIP);
	virtual ~MsgReceiver();

	/*
	 * function: the main function of the Receiver
	 */
	void running();

	/*
	 * function: set the transmission face table
	 * parameters: transFaceTable: the set of socket address of adjacent
	 */
	void setTransFaceTable(std::vector<sockaddr_in6>& transFaceTable);

	/*
	 * function: get the transmission face table
	 * return value: the transmission face table of the receiver
	 */
	std::vector<sockaddr_in6> getTransFaceTable();

	/*
	 * function: get the local ip
	 */
	std::string getLocalIP();

	void setPort(int port);

	int getPort();
	/*
	 * function: get the faceID by the socket address
	 * parameters: addr: the address to find
	 * return value: return the faceID of the addr if found, otherwise return -1
	 */
	FaceID getFaceIDByAddr(sockaddr_in6 addr);



private:
	/*
	 * function: initialize the socket to communicate and the transmission face table
	 */
	void initMsgReceiver();

private:
	std::string m_localIP;                                    //the local ip
	Buffer& m_msgBuffer;                                 //the buffer of message

	std::vector<sockaddr_in6> m_transmissionFaceTable; //the transmission face table

	Communicator m_communicator;  //the communicator to send and receive message
};

#endif /* MSGRECEIVER_H_ */
