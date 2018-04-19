/*
 * Controller.cpp
 *
 *  Created on: Sep 15, 2015
 *      Author: njnu
 */

#include "Controller.h"

#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fstream>

Controller::Controller(std::string localIP) :
		Panel(localIP) {
	// TODO Auto-generated constructor stub
	initController();
}

Controller::~Controller() {
	// TODO Auto-generated destructor stub
}

/*
 * function: init the controller
 */
void Controller::initController() {
	m_registedSet.resize(0);

	//init the topology
	m_topology.initFromFile();

//	std::cout << m_topology << std::endl << endl;

//init the communication addr
	setSenderPort( SENDPORT);

	setReceiverPort( RECEVPORT);

}

/*
 * function: apply the node for the role
 * parameters: comAddr: the communicate information set
 * return value: the id of the role applied
 */
int Controller::applyNode(Role role, CommunicateAddr comAddr) {
	return m_topology.applyNode(role, comAddr);
}

/*
 * function: handle the register
 * parameters: msg: the message of register
 */
void Controller::handleRegist(Message& msg) {
	//create the communicate address
	char* buffer = msg._buffer;

	/*
	 * the format of the regist message
	 * PacketType | Role | IP | CSPort | CRPort | NSPort | NRPort
	 */

	int pos = sizeof(PacketType);

	in6_addr addr;

	Role role;

	Port csPort;
	Port crPort;

	Port nsPort;
	Port nrPort;

	memcpy(&role, buffer + pos, sizeof(Role));
	pos += sizeof(Role);

	memcpy(&addr, buffer + pos, sizeof(in6_addr));
	pos += sizeof(in6_addr);

	memcpy(&csPort, buffer + pos, sizeof(Port));
	pos += sizeof(Port);

	memcpy(&crPort, buffer + pos, sizeof(Port));
	pos += sizeof(Port);

	memcpy(&nsPort, buffer + pos, sizeof(Port));
	pos += sizeof(Port);

	memcpy(&nrPort, buffer + pos, sizeof(Port));
	pos += sizeof(Port);

	CommunicateAddr comAddr;
	comAddr._ip = addr;
	comAddr._csPort = csPort;
	comAddr._crPort = crPort;
	comAddr._nsPort = nsPort;
	comAddr._nrPort = nrPort;

	int id = applyNode(role, comAddr);

	if (-1 != id) {
		RoleID roleID;
		roleID._role = role;
		roleID._id = id;

		m_registedSet.push_back(roleID);
	}

	//return the id assigned to the node
	sockaddr_in6 nodeAddr;
	memset(&nodeAddr, 0, sizeof(sockaddr_in6));
	nodeAddr.sin6_family = AF_INET6;
	nodeAddr.sin6_addr = comAddr._ip;

	//set the transfacetable of sender
	nodeAddr.sin6_port = htons(comAddr._crPort);
	std::vector<sockaddr_in6> sendTransFaceTable = getSendTransFaceTable();
	sendTransFaceTable.push_back(nodeAddr);
	FaceID sendFaceID = sendTransFaceTable.size() - 1;
	setSendTransFaceTable(sendTransFaceTable);

	//set the transfacetable of receiver
	nodeAddr.sin6_port = htons(comAddr._csPort);
	std::vector<sockaddr_in6> recevTransFaceTable = getRecevTransFaceTable();
	recevTransFaceTable.push_back(nodeAddr);
	setRecevTransFaceTable(recevTransFaceTable);

	/*
	 * the format of the assigned message
	 * PacketType | ID
	 */
	Message msgReturn;
	PacketType type = IDAssigned;
	pos = 0;

	msgReturn._bufferLength = sizeof(PacketType) + sizeof(id);
	msgReturn._buffer = new char[msgReturn._bufferLength];

	memcpy(msgReturn._buffer + pos, &type, sizeof(PacketType));
	pos += sizeof(type);

	memcpy(msgReturn._buffer + pos, &id, sizeof(int));
	pos += sizeof(int);

	msgReturn._msgLength = pos;

	msgReturn._faceID = sendFaceID;
	msgReturn._type=UDP;

	std::cout << "*********Control::handleRegist() start********" << std::endl;
	printMsg(msgReturn._buffer, msgReturn._msgLength, Ctlr);
	std::cout << "*********Control::handleRegist() end**********" << std::endl;

	sendMsg(msgReturn, msgReturn._faceID);

	std::cout << "the number of registered..." << m_registedSet.size()
			<< std::endl;

	//distrubition all kinds table
	if (m_registedSet.size() == m_topology.getNodeNum()) {
		std::cout << "controller distribute tables to nodes..." << std::endl;
		distributeAllTable();
	}

}

/*
 * function: distribute all table needed to the all node
 */
void Controller::distributeAllTable() {

	//distribute the adjacent table
	distributeAdjacentTable();

	//distribute the router table
	distributeRouterTalbe();
}

/*
 * function: distribute the adjacent table
 */
void Controller::distributeAdjacentTable() {
	/*
	 * the format of message of adjacent table
	 * PacketType | CurrentRole | CurrentID | NumOfAdjacent | AdjacentSet
	 *
	 * the format of adjacent set
	 * Role | ID | CommunicateAddr, the order mean the index of interface linked
	 *
	 * the format of CommunicateAddr
	 * IP | CSPort | CRPort | NSPort | NRPort
	 */

	Message msgAdja;

	int numOfNodeRegisted = m_registedSet.size();

	RoleID roleID;
	int numOfAdjacent = -1;

	PacketType type = AdjacentTable;
	int pos = 0;

	CommunicateAddr comAddr;

	for (int i = 0; i < numOfNodeRegisted; ++i) {
		std::vector<RoleID> adjacentSet = m_topology.getNodeByIndex(
				m_topology.findNode(m_registedSet[i])).getAdjacent();

		msgAdja._bufferLength = sizeof(PacketType) + sizeof(Role) + sizeof(int)
				+ sizeof(int)
				+ (sizeof(Role) + sizeof(int) + sizeof(CommunicateAddr))
						* adjacentSet.size();
		msgAdja._buffer = new char[msgAdja._bufferLength];

		pos = 0;

		//write the head of the message
		memcpy(msgAdja._buffer + pos, &type, sizeof(PacketType));
		pos += sizeof(PacketType);

		roleID = m_registedSet[i];

		memcpy(msgAdja._buffer + pos, &roleID._role, sizeof(Role));
		pos += sizeof(Role);

		memcpy(msgAdja._buffer + pos, &roleID._id, sizeof(int));
		pos += sizeof(int);

		numOfAdjacent = adjacentSet.size();

		memcpy(msgAdja._buffer + pos, &numOfAdjacent, sizeof(int));
		pos += sizeof(int);

		//write the adjacent information
		for (std::vector<RoleID>::size_type j = 0; j != adjacentSet.size();
				++j) {
			TopologyNode nodeAdja = m_topology.getNodeByIndex(
					m_topology.findNode(adjacentSet[j]));

			roleID = nodeAdja.getRoleID();

			memcpy(msgAdja._buffer + pos, &roleID._role, sizeof(Role));
			pos += sizeof(Role);

			memcpy(msgAdja._buffer + pos, &roleID._id, sizeof(int));
			pos += sizeof(int);

			//write the ip information
			comAddr = nodeAdja.getCommunicateAddr();

			memcpy(msgAdja._buffer + pos, &comAddr, sizeof(CommunicateAddr));
			pos += sizeof(CommunicateAddr);
		}

		msgAdja._msgLength = pos;
		msgAdja._faceID = i;
		msgAdja._type=UDP;

		sleep(1);
		printMsg(msgAdja._buffer, msgAdja._msgLength, Ctlr);

		//send the message
		sendMsg(msgAdja, msgAdja._faceID);
	}
}

/*
 * function: distribute the router table
 */
void Controller::distributeRouterTalbe() {
	/*
	 * the format of the message of router table
	 * PacketType | NumOfItem | RouterItem
	 *
	 * the format of RouterItem
	 * DestinationRoleID | NumberOfNextHop | NextHopSet
	 *
	 * the format of NextHopSet
	 * Role | ID
	 */

	Message msgRouter;
	int numOfNextHop = 0;
	int pos = 0;
	int numOfItem = 0;
	RoleID roleID;

	PacketType type = RouterTable;

	for (std::vector<RoleID>::size_type i = 0; i != m_registedSet.size(); ++i) {
		std::vector<RouterItem> routerTable = m_topology.getRouterTable(
				m_registedSet[i]);

		//calculate the all number of the next hop in the router table
		numOfNextHop = 0;
		for (std::vector<RouterItem>::size_type j = 0; j != routerTable.size();
				++j) {
			numOfNextHop += routerTable[j]._nextSet.size();
		}

		msgRouter._bufferLength = sizeof(PacketType) + sizeof(int)
				+ (sizeof(RoleID) + sizeof(int)) * routerTable.size()
				+ sizeof(RoleID) * numOfNextHop;
		msgRouter._buffer = new char[msgRouter._bufferLength];

		pos = 0;

		//write the message header
		memcpy(msgRouter._buffer + pos, &type, sizeof(PacketType));
		pos += sizeof(PacketType);

		numOfItem = routerTable.size();
		memcpy(msgRouter._buffer + pos, &numOfItem, sizeof(int));
		pos += sizeof(int);

		for (std::vector<RouterItem>::size_type j = 0; j != routerTable.size();
				++j) {
			RouterItem item = routerTable[j];
			numOfNextHop = item._nextSet.size();

			roleID = item._des;
			memcpy(msgRouter._buffer + pos, &roleID, sizeof(RoleID));
			pos += sizeof(RoleID);

			memcpy(msgRouter._buffer + pos, &numOfNextHop, sizeof(int));
			pos += sizeof(int);

			for (std::vector<RoleID>::size_type k = 0;
					k != item._nextSet.size(); ++k) {
				roleID = item._nextSet[k];
				memcpy(msgRouter._buffer + pos, &roleID, sizeof(RoleID));
				pos += sizeof(RoleID);
			}
		}
		msgRouter._msgLength = pos;
		msgRouter._faceID = i;
		msgRouter._type=UDP;

		sleep(1);
		cout << "************send msgRouter msg start******************"<<endl;
		printMsg(msgRouter._buffer, msgRouter._msgLength, Ctlr);
		cout << "************send msgRouter msg end******************"<<endl;;

		//send the message
		sendMsg(msgRouter, msgRouter._faceID);
	}
}

/*
 * function: handle the SubListUplaod
 * parameters: msg: the message of SubListUpload
 */
void Controller::handleSubListUpload(Message& msg) {
	/*
	 * the format of the upload message
	 * PacketType | Role | ID | ElementNumber | ElementSet
	 *
	 * the format of the ElementSet
	 * ElementLength | Element
	 */

	int elementNum = 0;
	vector<string> subList;
	int elementLength = 0;

	RoleID roleID;

	int pos = sizeof(PacketType);

	memcpy(&roleID._role, msg._buffer + pos, sizeof(Role));
	pos += sizeof(Role);

	memcpy(&roleID._id, msg._buffer + pos, sizeof(int));
	pos += sizeof(int);

	memcpy(&elementNum, msg._buffer + pos, sizeof(int));
	pos += sizeof(int);

	for (int i = 0; i < elementNum; ++i) {
		//read the length of the element
		memcpy(&elementLength, msg._buffer + pos, sizeof(int));
		pos += sizeof(int);

		//read the content of the element
		string str(msg._buffer + pos, elementLength);
		pos += elementLength;

		subList.push_back(str);
	}

	//union the sublist with the allsublist
	//order the sublist
	sort(subList.begin(), subList.end());

	vector<string> subListUnion(subList.size() + m_allSubList.size());
	vector<string>::iterator it = set_union(subList.begin(), subList.end(),
			m_allSubList.begin(), m_allSubList.end(), subListUnion.begin());

	m_allSubList.assign(subListUnion.begin(), it);

}

/*
 * function: handle the SubListDownload
 * parameters: msg: the message of SubListDownload
 */
void Controller::handleSubListRequest(Message& msg) {
	//release the memory of the msg
	delete[] msg._buffer;
	msg._buffer = NULL;

	//download
	/*
	 * the format of the upload message
	 * PacketType | Role | ID | ElementNumber | ElementSet
	 *
	 * the format of the ElementSet
	 * ElementLength | Element
	 */
	PacketType type = SubListDownload;

	long elementAllLength = 0;
	for (vector<string>::size_type i = 0; i != m_allSubList.size(); ++i) {
		elementAllLength += m_allSubList[i].size();
	}

	msg._bufferLength = sizeof(PacketType) + sizeof(Role) + sizeof(int)
			+ sizeof(int) + sizeof(int) * m_allSubList.size()
			+ elementAllLength;
	msg._buffer = new char[msg._bufferLength];
	memset(msg._buffer, 0, msg._bufferLength);

	int pos = 0;

	memcpy(msg._buffer + pos, &type, sizeof(PacketType));
	pos += sizeof(PacketType);

	RoleID roleID;
	roleID._role = Ctlr;
	roleID._id = 0;

	memcpy(msg._buffer + pos, &roleID._role, sizeof(Role));
	pos += sizeof(Role);

	memcpy(msg._buffer + pos, &roleID._id, sizeof(int));
	pos += sizeof(int);

	int elementNum = m_allSubList.size();
	memcpy(msg._buffer + pos, &elementNum, sizeof(int));
	pos += sizeof(int);

	int elementLength = 0;
	for (vector<string>::size_type i = 0; i != m_allSubList.size(); ++i) {
		//write the length of the element
		elementLength = m_allSubList[i].size();
		memcpy(msg._buffer + pos, &elementLength, sizeof(int));
		pos += sizeof(int);

		//write the content of the element
		memcpy(msg._buffer + pos, m_allSubList[i].c_str(), elementLength);
		pos += elementLength;
	}

	msg._msgLength = pos;
	msg._type=UDP;

	cout << "************send handleSubListRequest start******************"<<endl;
	printMsg(msg._buffer, msg._bufferLength, Ctlr);
	cout << "************send handleSubListRequest end******************"<<endl;

	//send the msg to the requester
	sendMsg(msg, msg._faceID);
}

/*
 * function: the main logical handle of the Controller
 */
void Controller::running() {
	//start the sender and receiver
	start();

	Message msg;

	while (true) {
		//read the message
		msg = recevMsg();

		cout << "***********controller start recv message**********************"
				<< endl;
		printMsg(msg._buffer, msg._msgLength, Ctlr);

		//analysis the type of the message
		PacketType type;
		memcpy(&type, msg._buffer, sizeof(PacketType));

		switch (type) {
		case Regist:
			handleRegist(msg);
			break;

		case SubListUpload:
			handleSubListUpload(msg);
			break;

		case SubListRequest:
			handleSubListRequest(msg);
			break;

		default:
			std::cerr << "error packet type:" << type << std::endl;
		}
		sleep(5);
	}
}
