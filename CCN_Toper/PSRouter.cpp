/*
 * PSRouter.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: njnu
 */

#include "PSRouter.h"
#include"Router.h"
#include"string.h"
#include<vector>
#include<set>
#include<algorithm>
using namespace std;

PS_Router::PS_Router(std::string localIP) :
		Router(localIP) {

}

PS_Router::~PS_Router() {

}

void PS_Router::handleMsg(Message& msg) {
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

	default:
		cerr << "handleMsg error packet type:" << type << endl;
	}
}

void PS_Router::handleSubscribe(Message& msg) {
	cout << "***************recv subscribe start************" << endl;
	printMsg(msg._buffer, msg._msgLength, Rot);
	cout << "***************recv subscribe end************" << endl;

	std::string rendezvous = getRendezvousName(msg);
	vector<int> faceList = getFaceList(rendezvous);
	if (faceList.size() == 0) {
		msg.release();
		cout << "********msg is local released***********" << endl;
		return;
	}

	if (addSTItem(msg)) {
		msg._faceID = faceList[0];
		cout << "***************send subscribe start************" << endl;
		printMsg(msg._buffer, msg._msgLength, Rot);
		cout << "***************send subscribe end************" << endl;
		sendMsg(msg, msg._faceID);

	} else {
		msg.release();
		cout << "********msg is local released***********" << endl;
	}

}

void PS_Router::handlePublish(Message& msg) {

	cout << "***************recv publish start************" << endl;
	printMsg(msg._buffer, msg._bufferLength, Rot);
	cout << "***************recv publish end************" << endl;

	set<int> faceList = getSTFaceList(msg);

	if (faceList.size() != 0) {
		for (set<int>::iterator iter = faceList.begin(); iter != faceList.end();
				iter++) {
			Message *tmp = msg.dup();
			tmp->_faceID = *iter;

			cout << "***************send publish start************" << endl;
			printMsg(msg._buffer, msg._bufferLength, Rot);
			cout << "***************send publish end************" << endl;

			sendMsg(*tmp, tmp->_faceID);
		}
	}
	msg.release();

}

void PS_Router::handleRPublish(Message& msg) {

	cout << "**********recv rpublish start************" << endl;
	printMsg(msg._buffer, msg._msgLength, Rot);
	cout << "**********recv rpublish end ************" << endl;

	string rendezvous = getRendezvousName(msg);

	vector<int> faceList = getFaceList(rendezvous);
	if (faceList.size() == 0) {
		msg.release();
		cout << "********msg is local released***********" << endl;
		return;
	}
	msg._faceID = faceList[0];

	cout << "**********send rpublish start************" << endl;
	printMsg(msg._buffer, msg._msgLength, Rot);
	cout << "**********send rpublish end ************" << endl;

	sendMsg(msg, msg._faceID);

}

