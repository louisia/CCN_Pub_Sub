/*
 * Publisher.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: njnu
 */

#include "Publisher.h"

#include <iostream>
#include <fstream>
#include<openssl/md5.h>
#include<set>
#include<algorithm>
using namespace std;

Publisher::Publisher(std::string localIP) :
		DataPanel(localIP), m_publishThreading(this) {

	m_sequenceNumber = 0;
	m_publishThreading.create(&m_publishThreading);
	m_isSubListNew = true;
}

Publisher::~Publisher() {
}

/*
 * function: publish the content according the time
 */
void Publisher::publish() {


	if (m_subList.size() == 0)
			return;

		if (m_isSubListNew) {
			m_isSubListNew = false;
			clog << endl << "------choose the following CD------" << endl;
			for (int i = 0; i < m_subList.size(); i++) {
				clog << m_subList[i] << endl;
			}

		}

	string cd, content;
	clog << "please input the CD:";
	cin >> cd;
	clog << "please input the content:";
	cin >> content;
	publishContent(content, cd);
}

/*
 * function: read the content to publish from the file
 * parameters: the name of the file which including the content to publish
 */
void Publisher::readContent(string fileName) {
	ifstream is;
	is.open(fileName.c_str(), ios::binary);

	Json::Reader reader;
	if (!reader.parse(is, m_contentSet)) {
		cerr << "read the content to publish failed..." << endl;
		exit(0);
	}
	is.close();
}

/*
 * function:send message
 * parameter: the description of the content
 *						the RRot to send the content
 */
void Publisher::publishSendMsg(std::string contentName, std::string content,
		std::string rendezvous) {
	/*
	 * PacketType | Role | ID | RRotLength | RRot|
	 * CDLength|CD|ContentLength|Content
	 */
	Message msg;
	PacketType type = RPublish;
	msg._bufferLength = sizeof(PacketType) + sizeof(Role) + sizeof(int)
			+ sizeof(int) + rendezvous.length() + sizeof(int)
			+ contentName.length() + sizeof(int) + content.length()
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

	len = content.length();
	memcpy(msg._buffer + pos, &len, sizeof(int));
	pos += sizeof(int);

	memcpy(msg._buffer + pos, content.c_str(), len);
	pos += len;

	m_sequenceNumber++;
	memcpy(msg._buffer + pos, &m_sequenceNumber, sizeof(int));
	pos += len;

	vector<int> faceList = getFaceList(rendezvous);

	if (faceList.size() == 0) {
		msg.release();
		return;
	}

	msg._faceID = faceList[0];
	msg._msgLength = pos;

	cout << "**********send rpublish start*************" << endl;
	printMsg(msg._buffer, msg._msgLength, Pub);
	cout << "**********send rpublish end*************" << endl;
	clog << "**********send publish successful*************" << endl;

	//send message
	sendMsg(msg, msg._faceID);

}

/*
 * function: publish the content with its content description
 * parameters: content : the content to send
 *             cds : the content description of the content to publish
 */
void Publisher::publishContent(string content, string contentName) {
	//getRRots from the datapanel
	map<string, unsigned long> rRotMap = getRRotSet();
	if (rRotMap.size() == 0)
		return;

	set<string> rRotSet;
	vector<string> cds = getPrexfixs(contentName);
	for (vector<string>::iterator iter = cds.begin(); iter != cds.end();
			iter++) {
		string rendezvous = hRW(*iter, rRotMap);
		rRotSet.insert(rendezvous);
	}

	//send the content to pulish
	for (set<string>::iterator iter = rRotSet.begin(); iter != rRotSet.end();
			iter++) {
		publishSendMsg(contentName, content, *iter);
	}
}

