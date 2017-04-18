/*
 * Subscriber.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: njnu
 */

#include "Subscriber.h"
#include"openssl/md5.h"
Subscriber::Subscriber(std::string localIP, string filename) :
		DataPanel(localIP), m_subscribeThreading(this) {
	m_filename = filename;
	m_sequenceNumber = 0;
	m_subscribeThreading.create(&m_subscribeThreading);
}

Subscriber::~Subscriber() {

}

void Subscriber::subscribe() {
	//get the rendezvous routers Set in the System
	map<string, unsigned long> rRotSet = getRRotSet();
	if (rRotSet.size() == 0)
		return;

	//get the content name subList in the System
	vector<string> subList = getSubList();
	if (subList.size() == 0)
		return;
	//TODO pick a random contentName from subList
	string contentName = getContentName(subList);
	if(contentName=="")
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
	printMsg(msg._buffer, msg._msgLength);
	cout << "**********handlePublish() end*************" << endl;
	msg.release();
}

string Subscriber::getContentName(vector<string> subList) {
	ifstream infile(m_filename.c_str());
	if (infile) {
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		while (!infile.eof()) {
			infile.getline(buffer, sizeof(buffer));
			string s(buffer);
			//TODO 这边subscriber提供的CD与publisher提供的CD之间的关系需要进一步优化
			if (find(subList.begin(), subList.end(), s) != subList.end()) {
				if (m_subscribeCdList.count(s) == 0) {
					m_subscribeCdList[s] = false;
				}
			}
		}
	} else {
		cerr << "open " << m_filename << " failed" << endl;
	}

	string contentName = "";
	for (map<string, bool>::iterator iter = m_subscribeCdList.begin();
			iter != m_subscribeCdList.end(); iter++) {
		if ((*iter).second == false) {
			contentName = (*iter).first;
			(*iter).second = true;
			break;
		}
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
	printMsg(msg._buffer, msg._msgLength);
	cout << "**********subscribeSendMsg() end*************" << endl;

	//send message
	sendMsg(msg, msg._faceID);
}

