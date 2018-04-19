/*
 * RPSRouter.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: njnu
 */

#include "RPSRouter.h"
#include<sstream>
#include<set>
#include<algorithm>
#include<vector>
#include"openssl/md5.h"
using namespace std;

RPS_Router::RPS_Router(std::string localIP) :
		Router(localIP) {

}

RPS_Router::~RPS_Router() {
}

void RPS_Router::handleMsg(Message& msg) {

	PacketType type;
	memcpy(&type, msg._buffer, sizeof(PacketType));

	switch (type) {
	case Subscribe:
		handleSubscribe(msg);
		break;
	case RPublish:
		handleRPublish(msg);
		break;
	case Publish:
		handlePublish(msg);
		break;
	case GroupIpRequest:
		handleGroupIpRequest(msg);
		break;

	default:
		cerr << "handleMsg error packet type:" << type << endl;
	}
}

void RPS_Router::handleSubscribe(Message& msg) {
	cout << "**********recv subscribe start*************" << endl;
	printMsg(msg._buffer, msg._msgLength, RRot);
	cout << "**********recv subscribe end*************" << endl;

	//判断是否存在FIB表用于转发该消息，注意：这个判断必须在ST更新之前
	std::string rendezvous = getRendezvousName(msg);
	vector<int> faceList = getFaceList(rendezvous);

	RoleID role = getRoleID();
	if (atoi(rendezvous.substr(5, rendezvous.length()).c_str()) == role._id) {
		/**
		 * 更新ST表，并抑制该消息，与其他节点的处理逻辑略有不同，
		 * 由于本节点为该订阅报文对应的汇聚节点，所以函数的返回值是否为false，都需要抑制这个消息
		 */
		addSTItem(msg);
		cout << "handleSubscribe is the destination" << endl;
		msg.release();
	} else {
		cout << "handleSubscribe is not the destination" << endl;
		if (addSTItem(msg)) {
			msg._faceID = faceList[0];
			cout << "**********send subscribe start*************" << endl;
			sendMsg(msg, msg._faceID);
			cout << "**********send subscribe end*************" << endl;
		} else {
			msg.release();
		}

	}
}

void RPS_Router::handlePublish(Message &msg) {

	cout << "***************recv Publish start************" << endl;
	printMsg(msg._buffer, msg._bufferLength, RRot);
	cout << "***************recv Publish end************" << endl;

	set<int> faceList = getSTFaceList(msg);

	if (faceList.size() != 0) {
		for (set<int>::iterator iter = faceList.begin(); iter != faceList.end();
				iter++) {
			Message *tmp = msg.dup();
			tmp->_faceID = *iter;
			sendMsg(*tmp, tmp->_faceID);
		}
	}
	msg.release();
}

void RPS_Router::handleRPublish(Message& msg) {

	cout << "***************recv rpublish start************" << endl;
	printMsg(msg._buffer, msg._msgLength, RRot);
	cout << "***************recv rpublish end************" << endl;
	std::string rendezvous = getRendezvousName(msg);
	RoleID role = getRoleID();
	if (atoi(rendezvous.substr(5, 6).c_str()) == role._id) {
		cout << "handleRPublish is the destination" << endl;
		PacketType type = Publish;
		memcpy(msg._buffer, &type, sizeof(PacketType));

		int pos = sizeof(PacketType) + sizeof(Role) + sizeof(int);
		memcpy(msg._buffer + pos,
				msg._buffer + pos + sizeof(int) + rendezvous.length(),
				msg._msgLength - pos - sizeof(int) - rendezvous.length());
		msg._msgLength = msg._msgLength - sizeof(int) - rendezvous.length();
		msg._bufferLength = msg._bufferLength - sizeof(int)
				- rendezvous.length();

		//search m_stList to get the forwarding faceID
		set<int> faceList = getSTFaceList(msg);

		if (faceList.size() == 0) {
			msg.release();
			cout << "**********rpublish is released,no ST Item*******" << endl;
			return;
		}

		for (set<int>::iterator iter = faceList.begin(); iter != faceList.end();
				iter++) {
			if (m_faceGroupIps.count(*iter) == 0) {
				msg._faceID = *iter;
				msg._type = UDP;
				Message *tmp = msg.dup();
				cout << "*******send publish udp start to port " << *iter
						<< "**********" << endl;
				printMsg((*tmp)._buffer, (*tmp)._msgLength, RRot);
				cout << "*******send publish udp end to port " << *iter
						<< "**********" << endl;
				sendMsg((*tmp), (*tmp)._faceID);

			}
		}
		string contentName = getRendezvousName(msg);
		vector<string> prefixs = getPrexfixs(contentName);
		for (vector<string>::iterator iter = prefixs.begin();
				iter != prefixs.end(); iter++) {
			for (vector<CDGroupIPRelation>::iterator iter2 =
					m_cdGroupIPRelation.begin();
					iter2 != m_cdGroupIPRelation.end(); iter2++) {
				if ((*iter) == (*iter2)._contentName
						&& (*iter2)._membercount > 0) {
					msg._type = IGMP;
					msg._multicastIp = (*iter2)._multicastIp;
					msg._faceID = 0;

					Message *tmp = msg.dup();
					cout << "**********send publish multicast start to "
							<< msg._multicastIp << endl << "*************";
					printMsg((*tmp)._buffer, (*tmp)._msgLength, RRot);
					cout << "**********send publish multicast end to "
							<< msg._multicastIp << endl << "*************";
					sendMsg((*tmp), (*tmp)._faceID);
					break;
				}
			}
		}
		msg.release();
	} else {
		cout << "handleRPublish is not the destination" << endl;
		vector<int> faceList = getFaceList(rendezvous);
		if (faceList.size() == 0) {
			msg.release();
			return;
		}
		msg._faceID = faceList[0];
		//send message
		sendMsg(msg, msg._faceID);
	}
}

void RPS_Router::handleGroupIpRequest(Message &msg) {
	/*
	 * 因为一个请求到达报文，只会经过一个汇聚节点，即这个汇聚节点收到后，必须将其释放
	 */
	cout << "**********recv groupIpRequest() start*************" << endl;
	printMsg(msg._buffer, msg._msgLength, RRot);
	cout << "**********recv groupIpRequest() end*************" << endl;

	addSTItem(msg);

	int pos = sizeof(PacketType) + sizeof(Role) + sizeof(int) + sizeof(int)
			+ getRendezvousName(msg).length();
	int len;
	memcpy(&len, msg._buffer + pos, sizeof(int));
	pos += sizeof(int);
	string contentName = std::string(msg._buffer + pos, len);

	vector<string> quitGroupCDs;

	for (vector<string>::iterator iter = m_faceGroupIps[msg._faceID].begin();
			iter != m_faceGroupIps[msg._faceID].end();) {
		vector<string> prefixs = getPrexfixs(*iter);
		if (find(prefixs.begin(), prefixs.end(), contentName)
				!= prefixs.end()) {
			quitGroupCDs.push_back(*iter);
			m_faceGroupIps[msg._faceID].erase(iter, iter + 1);
		} else {
			iter++;
		}
	}

	m_faceGroupIps[msg._faceID].push_back(contentName);

	vector<string> quitGroupIPs;
	for (vector<CDGroupIPRelation>::iterator iter = m_cdGroupIPRelation.begin();
			iter != m_cdGroupIPRelation.end(); iter++) {
		if (find(quitGroupCDs.begin(), quitGroupCDs.end(), (*iter)._contentName)
				!= quitGroupCDs.end()) {
			quitGroupIPs.push_back(iter->_multicastIp);
			iter->_membercount--;
		}
	}

	string multicastIp = "";
	for (vector<CDGroupIPRelation>::iterator iter = m_cdGroupIPRelation.begin();
			iter != m_cdGroupIPRelation.end(); iter++) {
		if (iter->_contentName == contentName) {
			multicastIp = iter->_multicastIp;
			iter->_membercount++;
			break;
		}
	}
	if (multicastIp == "") {
		unsigned char *md = MD5((const unsigned char*) contentName.c_str(),
				(contentName).size(), NULL);
		multicastIp = hexToGroupIP(hexstr(md));
		CDGroupIPRelation item;
		item._contentName = contentName;
		item._membercount = 1;
		item._multicastIp = multicastIp;
		m_cdGroupIPRelation.push_back(item);
	}

	Message GroupIPResponseMsg;
	PacketType type = GroupIpResponse;
	GroupIPResponseMsg._bufferLength = sizeof(PacketType) + sizeof(int)
			+ contentName.length() + sizeof(int) + multicastIp.length()
			+ sizeof(int);
	for (vector<string>::size_type i = 0; i < quitGroupCDs.size(); i++) {
		GroupIPResponseMsg._bufferLength += sizeof(int);
		GroupIPResponseMsg._bufferLength += quitGroupCDs[i].length();
		GroupIPResponseMsg._bufferLength += sizeof(int);
		GroupIPResponseMsg._bufferLength += quitGroupIPs[i].length();
	}
	GroupIPResponseMsg._buffer = new char[GroupIPResponseMsg._bufferLength];

	pos = 0;

	memcpy(GroupIPResponseMsg._buffer + pos, &type, sizeof(PacketType));
	pos += sizeof(PacketType);

	len = contentName.length();
	memcpy(GroupIPResponseMsg._buffer + pos, &len, sizeof(int));
	pos += sizeof(int);

	memcpy(GroupIPResponseMsg._buffer + pos, contentName.c_str(), len);
	pos += len;

	len = multicastIp.length();
	memcpy(GroupIPResponseMsg._buffer + pos, &len, sizeof(int));
	pos += sizeof(int);

	memcpy(GroupIPResponseMsg._buffer + pos, multicastIp.c_str(), len);
	pos += len;

	int size = quitGroupIPs.size();
	memcpy(GroupIPResponseMsg._buffer + pos, &size, sizeof(int));
	pos += sizeof(int);

	for (int i = 0; i < size; i++) {
		len = quitGroupCDs[i].length();
		memcpy(GroupIPResponseMsg._buffer + pos, &len, sizeof(int));
		pos += sizeof(int);

		memcpy(GroupIPResponseMsg._buffer + pos, quitGroupCDs[i].c_str(), len);
		pos += len;

		len = quitGroupIPs[i].length();
		memcpy(GroupIPResponseMsg._buffer + pos, &len, sizeof(int));
		pos += sizeof(int);

		memcpy(GroupIPResponseMsg._buffer + pos, quitGroupIPs[i].c_str(), len);
		pos += len;

	}

	GroupIPResponseMsg._faceID = msg._faceID;
	GroupIPResponseMsg._msgLength = pos;
	GroupIPResponseMsg._type = UDP;

	cout << "**********send groupIpResponse() start*************" << endl;
	printMsg(GroupIPResponseMsg._buffer, GroupIPResponseMsg._msgLength, RRot);
	cout << "**********send groupIpResponse() end*************" << endl;

	sendMsg(GroupIPResponseMsg, GroupIPResponseMsg._faceID);

	msg.release();

}

string RPS_Router::hexstr(unsigned char *buf) {
	const char *set = "0123456789abcdef";
	static char str[32], *tmp;
	unsigned char *end;
	end = buf + MD5_DIGEST_LENGTH;
	tmp = &str[0];
	while (buf < end) {
		*tmp++ = set[(*buf) >> 4];
		*tmp++ = set[(*buf) & 0xF];
		buf++;
	}
	string result(str);
	return result;
}
string RPS_Router::hexToGroupIP(string hexStr) {
	string ip = GROUPIPPREFIX;
	hexStr = hexStr.erase(0, 4);
	for (string::size_type i = 0; i < hexStr.size(); i += 5) {
		hexStr.insert(i, ":");
	}
	ip += hexStr;
	return ip;

}

