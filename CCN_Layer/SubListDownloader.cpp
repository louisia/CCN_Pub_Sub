/*
 * SubListDownload.cpp
 *
 *  Created on: Apr 10, 2016
 *      Author: njnu
 */

#include "SubListDownloader.h"

SubListDownloader::SubListDownloader() {
	// TODO Auto-generated constructor stub
}

SubListDownloader::~SubListDownloader() {
	// TODO Auto-generated destructor stub
}

/*
 * function: set the datapanel controlled by current controlpanel,
 *          and it should be called before calling the check function
 */
void SubListDownloader::setDataPanel(DataPanel* pDataPanel) {
	m_pDataPanel = pDataPanel;
}

/*
 * function: set the current controlpanel, and it should be called before calling the check function
 */
void SubListDownloader::setControlPanel(ControlPanel* pControlPanel) {
	m_pControlPanel = pControlPanel;
}

/*
 * function: the implementation of the interface of the checker
 */
void SubListDownloader::check() {

	//download the list of subscription

	//send the request to the controller
	/*
	 * the format of the request
	 * PacketType | Role | Id
	 */
	Message msg;
	PacketType type = SubListRequest;
	msg._bufferLength = sizeof(PacketType) + sizeof(Role) + sizeof(int);
	msg._buffer = new char[msg._bufferLength];

	memset(msg._buffer, 0, msg._bufferLength);

	int pos = 0;
	RoleID roleID = m_pDataPanel->getRoleID();

	memcpy(msg._buffer + pos, &type, sizeof(PacketType));
	pos += sizeof(PacketType);

	memcpy(msg._buffer + pos, &roleID._role, sizeof(Role));
	pos += sizeof(Role);

	memcpy(msg._buffer + pos, &roleID._id, sizeof(int));
	pos += sizeof(int);

	msg._msgLength = pos;

	msg._faceID = 0;

	//*/debug

	printMsg(msg._buffer, msg._bufferLength, Ctlr);

	//*/

	//send the msg
	if (m_pControlPanel == NULL) {
		cerr
				<< "the pointer of controlpanel in the sublist downloader is null..."
				<< endl;
		return;
	}
	m_pControlPanel->sendMsg(msg, msg._faceID);

	delete[] msg._buffer;
	msg._buffer = NULL;

	//receive the response
	msg = m_pControlPanel->recevMsg();

	memcpy(&type, msg._buffer, sizeof(PacketType));

	if (type != SubListDownload) {
		cout
				<< "@@@@@@@@@@@error message type of the sublistdownloader@@@@@@@@@"
				<< type << endl;
		printMsg(msg._buffer, msg._bufferLength, Ctlr);
		cout << "@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#" << endl;
	}

	//analysis the response
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

	pos = sizeof(PacketType);

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

	//update the sublist of the data panel
	printMsg(msg._buffer, msg._bufferLength, Ctlr);
	m_pDataPanel->setSubList(subList);

	//sleep
	sleep(5);
}
