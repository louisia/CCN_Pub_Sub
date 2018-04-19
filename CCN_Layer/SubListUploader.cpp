/*
 * SubListUpload.cpp
 *
 *  Created on: Apr 10, 2016
 *      Author: njnu
 */

#include "SubListUploader.h"
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <iostream>
#include <fstream>
#include <algorithm>

SubListUploader::SubListUploader() {
	// TODO Auto-generated constructor stub
}

SubListUploader::~SubListUploader() {
	// TODO Auto-generated destructor stub
}

/*
 * function: set the datapanel controlled by current controlpanel,
 *          and it should be called before calling the check function
 */
void SubListUploader::setDataPanel(DataPanel* pDataPanel) {
	m_pDataPanel = pDataPanel;
}

/*
 * function: set the current controlpanel, and it should be called before calling the check function
 */
void SubListUploader::setControlPanel(ControlPanel* pControlPanel) {
	m_pControlPanel = pControlPanel;
}

/*
 * function: generate the subscription list from the file named by the file named fileName
 * parameters: fileName : the name of the file which contain the content to publish
 * return value: return the subscription list of the content to publish
 */
std::vector<std::string> SubListUploader::readSubList(std::string fileName) {
	vector<string> subList;

	//read the content from the file
	ifstream is;
	is.open(fileName.c_str(), std::ios::binary);

	if (is) {
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		while (!is.eof()) {
			is.getline(buffer, sizeof(buffer));
			string s(buffer);
			vector<string> prefixs = getPrexfixs(s);

			for (vector<string>::size_type i = 0; i < prefixs.size(); i++) {
				if (find(subList.begin(), subList.end(), prefixs[i])
						== subList.end())
					subList.push_back(prefixs[i]);

			}
		}
	}
	//close the file
	is.close();

	return subList;
}

/*
 * function: the implementation of the interface of the checker
 */
void SubListUploader::check() {

	//upload the list of subscription

	//read the subscription from the special file
	vector<string> subList = readSubList("../Files/CDList.txt");

	long elementAllLength = 0;
	for (vector<string>::size_type i = 0; i != subList.size(); ++i) {
		elementAllLength += subList[i].size();
	}

	//upload
	/*
	 * the format of the upload message
	 * PacketType | Role | ID | ElementNumber | ElementSet
	 *
	 * the format of the ElementSet
	 * ElementLength | Element
	 */
	Message msg;
	PacketType type = SubListUpload;

	msg._bufferLength = sizeof(PacketType) + sizeof(Role) + sizeof(int)
			+ sizeof(int) + sizeof(int) * subList.size() + elementAllLength;
	msg._buffer = new char[msg._bufferLength];
	memset(msg._buffer, 0, msg._bufferLength);

	int pos = 0;

	memcpy(msg._buffer + pos, &type, sizeof(PacketType));
	pos += sizeof(PacketType);

	if (m_pDataPanel == NULL) {
		cerr << "the pointer of datapanel in the sublist uploader is null..."
				<< endl;
		return;
	}
	RoleID roleID = m_pDataPanel->getRoleID();

	memcpy(msg._buffer + pos, &roleID._role, sizeof(Role));
	pos += sizeof(Role);

	memcpy(msg._buffer + pos, &roleID._id, sizeof(int));
	pos += sizeof(int);

	int elementNum = subList.size();
	memcpy(msg._buffer + pos, &elementNum, sizeof(int));
	pos += sizeof(int);

	int elementLength = 0;
	for (vector<string>::size_type i = 0; i != subList.size(); ++i) {
		//write the length of the element
		elementLength = subList[i].size();
		memcpy(msg._buffer + pos, &elementLength, sizeof(int));
		pos += sizeof(int);

		//write the content of the element
		memcpy(msg._buffer + pos, subList[i].c_str(), elementLength);
		pos += elementLength;
	}

	msg._msgLength = pos;

	msg._faceID = 0;

	//send the message
	if (m_pControlPanel == NULL) {
		cerr << "the pointer of controlpanel in the sublist uploader is null..."
				<< endl;
		return;
	}

	printMsg(msg._buffer, msg._bufferLength, Ctlr);
	m_pDataPanel->setSubList(subList);

	m_pControlPanel->sendMsg(msg, msg._faceID);



	//sleep
	sleep(5);
}
