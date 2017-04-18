/*
 * MsgSender.h
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#ifndef MSGSENDER_H_
#define MSGSENDER_H_

#include "Threading.h"
#include "Buffer.h"
#include "Communicator.h"

#include <vector>
#include <sys/types.h>

class MsgSender: public Threading {
public:
	MsgSender(Buffer& buffer, std::string localIP);
	virtual ~MsgSender();

	/*
	 * function: the main function of the Sender
	 */
	void running();

	/*
	 * function: set the transmission face table
	 * parameters: transFaceTable: the set of socket address of adjacent
	 */
	void setTransFaceTable(std::vector<sockaddr_in6>& transFaceTable);

	/*
	 * function: get the transmission face table
	 * return value: the transmission face table of the sender
	 */
	std::vector<sockaddr_in6> getTransFaceTable();

	/*
	 * function: get the ip of local
	 */
	std::string getLacalIP();

	void setPort(int port);

	int getPort();
private:
	/*
	 * function: initialize the socket to communicate and the transmission face table
	 */
	void initMsgSender();

private:
	std::string m_localIP;                                //the local ip

	Buffer& m_msgBuffer;                             //the buffer of the message

	std::vector<sockaddr_in6> m_transmissionFaceTable; //the transmission face table

	Communicator m_communicator;  //the communicator to send and receive message
};

#endif /* MSGSENDER_H_ */
