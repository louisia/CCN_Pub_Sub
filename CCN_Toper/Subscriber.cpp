/*
 * Subscriber.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: njnu
 */

#include "Subscriber.h"
#include"openssl/md5.h"
Subscriber::Subscriber(std::string localIP) :
		DataPanel(localIP), m_subscribeThreading(this) {
	m_sequenceNumber = 0;
	m_subscribeThreading.create(&m_subscribeThreading);
	m_isSubListNew = true;
}

Subscriber::~Subscriber() {


}

void Subscriber::subscribe() {
	//get the rendezvous routers Set in the System
	map<string, unsigned long> rRotSet = getRRotSet();
	if (rRotSet.size() == 0)
		return;

	//get the content name subList in the System
	if (m_subList.size() == 0)
		return;

	if (m_isSubListNew) {
		m_isSubListNew = false;
		clog << endl << "------choose the following CD------" << endl;
		for (int i = 0; i < m_subList.size(); i++) {
			clog << m_subList[i] << endl;
		}

	}

	//TODO pick a random contentName from subList
	string contentName = getContentName(m_subList);
	if (contentName == "")
		return;
	string rendezvous = hRW(contentName, rRotSet);

	//send message
	subscribeSendMsg(contentName, rendezvous);
}

void Subscriber::handleMsg(Message& msg) {
	PacketType type;
	memcpy(&type, msg._buffer, sizeof(PacketType));
	switch (type) {
	case Publish:
		handlePublish(msg);
		break;
	default:
		cerr << "handleMsg error packet type:" << type << endl;
	}
}

void Subscriber::handlePublish(Message& msg) {
	cout << "**********handlePublish() start*************" << endl;
	printMsg(msg._buffer, msg._msgLength, Sub);
	cout << "**********handlePublish() end*************" << endl;
	msg.release();
}

string Subscriber::getContentName(vector<string> subList) {
	string contentName;
	clog << "please input your interest cd: ";
	cin >> contentName;
	if (find(subList.begin(), subList.end(), contentName) == subList.end()) {
		clog << "invaild content name" << endl;
		contentName = "";
	}
	return contentName;
}

/*
 * PacketType | Role | ID | RRotLength | RRot|CDLength|CD
 */
void Subscriber::subscribeSendMsg(std::string contentName,
		std::string rendezvous) {
	Message msg;
	PacketType type = Subscribe;
	msg._bufferLength = sizeof(PacketType) + sizeof(RoleID) + sizeof(int)
			+ rendezvous.length() + sizeof(int) + contentName.length()
			+ sizeof(int);
	msg._buffer = new char[msg._bufferLength];

	int pos = 0;

	memcpy(msg._buffer + pos, &type, sizeof(PacketType));
	pos += sizeof(PacketType);

	RoleID roleID = getRoleID();

	memcpy(msg._buffer + pos, &roleID._role, sizeof(Role));
	pos += sizeof(Role);

	memcpy(msg._buffer + pos, &roleID._id, sizeof(int));
	pos += sizeof(int);

	int len = rendezvous.length();
	memcpy(msg._buffer + pos, &len, sizeof(int));
	pos += sizeof(int);

	memcpy(msg._buffer + pos, rendezvous.c_str(), len);
	pos += len;

	len = contentName.length();
	memcpy(msg._buffer + pos, &len, sizeof(int));
	pos += sizeof(int);

	memcpy(msg._buffer + pos, contentName.c_str(), len);
	pos += len;

	memcpy(msg._buffer + pos, &m_sequenceNumber, sizeof(int));
	pos += sizeof(int);
	m_sequenceNumber++;

	vector<int> faceList = getFaceList(rendezvous);

	if (faceList.size() == 0) {
		msg.release();
		return;
	}
	msg._faceID = faceList[0];
	msg._msgLength = pos;
	msg._type = UDP;

	cout << "**********subscribeSendMsg() start*************" << endl;
	printMsg(msg._buffer, msg._msgLength, Sub);
	cout << "**********subscribeSendMsg() end*************" << endl;
	clog << "**********send subscribe successful*************" << endl;

	//send message
	sendMsg(msg, msg._faceID);

}

