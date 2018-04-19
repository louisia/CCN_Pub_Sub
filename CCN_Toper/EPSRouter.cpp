/*
 * EPSRouter.cpp
 *
 *  Created on: 2017年3月14日
 *      Author: louisia
 */

#include "MulticastThreading.h"
#include"../CCN_Layer/Structure.h"
#include"../CCN_Layer/Slicer.h"
#include"../CCN_Layer/Reformer.h"
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <stdio.h>
#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<iostream>
using namespace std;

EPS_Router::EPS_Router(string localIp) :
		Router(localIp), m_multicastThreading(this) {
	m_multicastSocket = -1;
	initMulticastSocket();
	m_multicastThreading.create(&m_multicastThreading);

}

EPS_Router::~EPS_Router() {
}

void EPS_Router::initMulticastSocket() {

	if ((m_multicastSocket = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		cerr << "multisocket create failed" << endl;
		exit(1);
	}

	setSocketOpt();

	struct sockaddr_in6 addr6;
	/* set up destination address */
	memset(&addr6, 0, sizeof(addr6));
	addr6.sin6_family = AF_INET6;
	addr6.sin6_addr = in6addr_any;
	addr6.sin6_port = htons(GROUPPORT);

	/* bind to receive address */
	if (bind(m_multicastSocket, (struct sockaddr *) &addr6, sizeof(addr6))
			== -1) {
		cout << "multicast bind failed" << strerror(errno) << " errno: "
				<< errno << endl;
		exit(1);
	}

}

void EPS_Router::multicast() {
	Reformer reformer(getReceiverBuffer());
	Slice slice;

	struct sockaddr_in6 addr6;
	socklen_t addrlen = sizeof(addr6);
	memset(&addr6, 0, sizeof(sockaddr_in6));

	char recevBuffer[SLICER_SIZE + sizeof(Slice) - sizeof(char*)];
	int num;

	while (true) {

		if ((num = recvfrom(m_multicastSocket, recevBuffer, sizeof(recevBuffer),
				0, (struct sockaddr *) &addr6, &addrlen)) < 0) {
			std::cerr << "multicast：" << strerror( errno) << " errno: " << errno
					<< std::endl;
		} else {
			PacketID sliceIdAck;
			MD5((unsigned char*) recevBuffer, sizeof(recevBuffer),
					(unsigned char*) &sliceIdAck);
			//send back the ack
			int result = sendto(m_multicastSocket, sliceIdAck._id,
					sizeof(PacketID), 0, (struct sockaddr*) &addr6,
					sizeof(addr6));

			if (-1 == result) {
				std::cerr << " multicast send ack message failed..."
						<< strerror(errno) << " errno: " << errno << std::endl;
				return;
			}

			memcpy(&slice, recevBuffer, sizeof(Slice) - sizeof(char*));
			slice._sliceData = recevBuffer + sizeof(Slice) - sizeof(char*);
			FaceID faceID = getMsgReceiver().getFaceIDByAddr(addr6);
			reformer.putSlice(slice, faceID);

		}
	}
}
void EPS_Router::setSocketOpt() {
	//设置定时器
	timeval tv;
	tv.tv_sec = SOCK_TIMEOUT;
	tv.tv_usec = 0;
	if (setsockopt(m_multicastSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))
			!= 0) {
		cerr << "setSocketOpt SO_RCVTIMEO error" << endl;
	}

	int addressReuseFlag = 1; //允许地址重用
	if (setsockopt(m_multicastSocket, SOL_SOCKET, SO_REUSEADDR,
			&addressReuseFlag, sizeof(addressReuseFlag)) != 0) {
		cerr << "setSocketOpt SO_REUSEADDR error" << endl;
	}

	//设置跳数
	int hops = 255;
	if (setsockopt(m_multicastSocket, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops,
			sizeof(hops)) != 0) {
		cerr << "setSocketOpt IPV6_MULTICAST_HOPS error" << endl;
	}



}

void EPS_Router::joinMulticastGroup(string groupIP) {
	struct sockaddr_in6 maddr;
	struct ipv6_mreq mreq6;
	memset(&maddr, 0, sizeof(maddr));
	inet_pton(AF_INET6, groupIP.c_str(), &maddr.sin6_addr);

	memcpy(&mreq6.ipv6mr_multiaddr, &maddr.sin6_addr,
			sizeof(mreq6.ipv6mr_multiaddr));
	inet_pton(AF_INET6, groupIP.c_str(), &(mreq6.ipv6mr_multiaddr));
	mreq6.ipv6mr_interface = 0;
	/* set socket operation to join the multicast group in IPV6 way */
	if (setsockopt(m_multicastSocket, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq6,
			sizeof(mreq6)) != 0) {
		cerr << "join group failed " << groupIP << endl;
	} else {
		cout << "join group succeed " << groupIP << endl;

	}
}
void EPS_Router::leaveMulticastGroups(vector<string> groupIPs) {
	for (vector<string>::iterator iter = groupIPs.begin();
			iter != groupIPs.end(); iter++) {
		struct ipv6_mreq mreq6;
		inet_pton(AF_INET6, iter->c_str(), &(mreq6.ipv6mr_multiaddr));
		mreq6.ipv6mr_interface = htons(INADDR_ANY);

		/* set socket operation to join the multicast group in IPV6 way */
		if (setsockopt(m_multicastSocket, IPPROTO_IPV6, IPV6_LEAVE_GROUP,
				&mreq6, sizeof(mreq6)) != 0) {
			cerr << "leave group failed " << *iter << endl;
		} else {
			cout << "leave group succeed " << *iter << endl;

		}
	}
}

void EPS_Router::handleMsg(Message &msg) {
	PacketType type;
	memcpy(&type, msg._buffer, sizeof(PacketType));
	switch (type) {
	case Subscribe:
		handleSubscribe(msg);
		break;
	case Publish:
		handlePublish(msg);
		break;
	case RPublish:
		handleRPublish(msg);
		break;
	case GroupIpResponse:
		handleGroupIpResponse(msg);
		break;
	default:
		cerr << "handleMsg error packet type:" << type << endl;
	}
}
/**
 * function：Change subscribe message to group ip request message and send to RRot
 * subscribe message format:
 * 			PacketType(Subscribe) | Role | ID | RRotLength | RRot|CDLength|CD
 * group ip request message format:
 * 			PacketType(GroupIpRequest) | Role | ID | RRotLength | RRot|CDLength|CD
 * input parameter: subscribe message
 *
 * return parameter:
 */
void EPS_Router::handleSubscribe(Message &msg) {
	cout << "**********recv subscribe start***********" << endl;
	printMsg(msg._buffer, msg._msgLength,ERot);
	cout << "**********recv subscribe end*************" << endl;

	//判断是否存在FIB表用于转发该消息，注意：这个判断必须在ST更新之前
	string rendezvous = getRendezvousName(msg);
	vector<int> faceList = getFaceList(rendezvous);
	if (faceList.size() == 0) {
		msg.release();
		cout << "********msg is local released***********" << endl;
		return;
	}

	if (addSTItem(msg)) { //转发
		PacketType type = GroupIpRequest;
		memcpy(msg._buffer, &type, sizeof(type));
		msg._faceID = faceList[0];

		cout << "**********send groupIpRequest start***********" << endl;
		printMsg(msg._buffer, msg._msgLength,ERot);
		cout << "**********send groupIpRequest end***********" << endl;

		sendMsg(msg, msg._faceID);
	} else {
		cout << "********msg is local released***********" << endl;
		msg.release(); //抑制转发
	}

}
/**
 * function：query the subscribe table to send the publish message to subscribers
 * publish message format:
 * 		PacketType | Role | ID
 * 		CDNum|CD1Length|CD1……
 * 		ContentLength|Content
 * input parameter: publish message
 *
 * return parameter:
 */
void EPS_Router::handlePublish(Message &msg) {

	cout << "*********recv publish start********" << endl;
	printMsg(msg._buffer, msg._bufferLength, ERot);
	cout << "*********recv publish end**********" << endl;
	//get the forwarding ports of the publish message
	set<int> faceList = getSTFaceList(msg);

	if (faceList.size() != 0) {
		for (set<int>::iterator iter = faceList.begin(); iter != faceList.end();
				iter++) {
			Message* tmp = msg.dup();
			tmp->_faceID = *iter;
			tmp->_type = UDP;
			tmp->_multicastIp = "";

			cout << "*********send publish start********" << endl;
			printMsg((*tmp)._buffer, (*tmp)._bufferLength,ERot);
			cout << "*********send publish end**********" << endl;

			sendMsg(*tmp, tmp->_faceID);
		}
	}
	msg.release();

}
/**
 * function：send the rendezvous publish message to the rendezvous point
 * rendezvous publish message format:
 * 		PacketType | Role | ID
 * 		RRotLength | RRot
 * 		CDNum|CD1Length|CD1……
 * 		ContentLength|Content
 * input parameter: rendezvous publish message
 *
 * return parameter:
 */
void EPS_Router::handleRPublish(Message &msg) {

	cout << "*******recv rpublish start*****" << endl;
	printMsg(msg._buffer, msg._msgLength,ERot);
	cout << "*******recv rpublish end******" << endl;

	string rendezvous = getRendezvousName(msg);
	vector<int> faceList = getFaceList(rendezvous);
	if (faceList.size() == 0) {
		msg.release();
		cout << "*************msg is local released************" << endl;
		return;
	}
	msg._faceID = faceList[0];
	cout << "*******send rpublish start*****" << endl;
	printMsg(msg._buffer, msg._msgLength,ERot);
	cout << "*******send rpublish end******" << endl;
	sendMsg(msg, msg._faceID);
}
/**
 * function：get the group ip from group ip response message
 * group ip response message format:
 * 		PacketType | CDLength | CD|GroupIpLen|GroupIp
 * input parameter: group ip response message
 *
 * return parameter:
 */
void EPS_Router::handleGroupIpResponse(Message &msg) {

	cout << "*********recv groupIpresponse start********" << endl;
	printMsg(msg._buffer, msg._bufferLength,ERot);
	cout << "*********recv groupIpresponse end**********" << endl;

	int pos = sizeof(PacketType);
	int len;
	memcpy(&len, msg._buffer + pos, sizeof(int));
	pos += sizeof(int);
	string contentName = std::string(msg._buffer + pos, len);
	pos += len;
	memcpy(&len, msg._buffer + pos, sizeof(int));
	pos += sizeof(int);
	string joinGroupIp = std::string(msg._buffer + pos, len);
	pos += joinGroupIp.size();

	int size;
	vector<string> quitGroupIPs;
	memcpy(&size, msg._buffer + pos, sizeof(int));
	pos += sizeof(int);

	for (int i = 0; i < size; i++) {
		memcpy(&len, msg._buffer + pos, sizeof(int));
		pos += sizeof(int);

		contentName = std::string(msg._buffer + pos, len);
		pos += len;

		memcpy(&len, msg._buffer + pos, sizeof(int));
		pos += sizeof(int);

		string quitGroupIp = std::string(msg._buffer + pos, len);
		pos += len;
		quitGroupIPs.push_back(quitGroupIp);

	}

	joinMulticastGroup(joinGroupIp);
//	leaveMulticastGroups(quitGroupIPs);

	msg.release();

}

