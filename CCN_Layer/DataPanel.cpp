/*
 * DataPanel.cpp
 *
 *  Created on: Sep 20, 2015
 *      Author: njnu
 */

#include "DataPanel.h"

#include <string.h>

//class Checker;

DataPanel::DataPanel(std::string localIP) :
		Panel(localIP), m_checker(this) {
	// TODO Auto-generated constructor stub

	m_checker.create(&m_checker);

	pthread_mutex_init(&m_subListMutex, NULL);
}

DataPanel::~DataPanel() {
	// TODO Auto-generated destructor stub
}

/*
 * function: get the face list to translate to get the content
 * parameters: contentName : the name of the content to get
 * return value: the face list to translate
 */
std::vector<int> DataPanel::getFaceList(std::string contentName) {
	//using the stl algorithm to find the contentName--binary search
	FIBItem target;
	target._contentName = contentName;
	std::pair<std::vector<FIBItem>::iterator, std::vector<FIBItem>::iterator> res =
			equal_range(m_fib.begin(), m_fib.end(), target);

	if (res.first != res.second)
		return res.first->_faceList;

	return std::vector<int>();
}

/*
 * function: set the fib
 * parameters: fib : the fib table to setS
 */
void DataPanel::setFIB(const std::vector<FIBItem>& fib) {
	m_fib = fib;

	sleep(1);
	std::cout << "-----setFIB---FIB----------------" << std::endl;
	for (int i = 0; i < m_fib.size(); ++i) {
		std::cout << "Name: " << m_fib[i]._contentName << " FaceList: ";
		for (int j = 0; j < m_fib[i]._faceList.size(); ++j)
			std::cout << m_fib[i]._faceList[j] << " ";
		std::cout << endl;
	}
	std::cout << "-----------------------------------" << std::endl;
}

/*
 * function: set the roelID of current node
 * parameters: roleID : the roleID to set
 */
void DataPanel::setRoleID(RoleID roleID) {
	m_roleID = roleID;
}

/*
 * function: get the roleID of current node
 */
RoleID DataPanel::getRoleID() {
	return m_roleID;
}

/*
 * function: set the subscription list
 * parameters: subList: the sublist to be set
 */
void DataPanel::setSubList(const std::vector<std::string>& subList) {
	//here may need synchronous operation

	pthread_mutex_lock(&m_subListMutex);
	m_subList = subList;
	pthread_mutex_unlock(&m_subListMutex);
}

/*
 * function: get the subscription list
 * return value: subList: the current sublist
 */
std::vector<std::string> DataPanel::getSubList() {
	pthread_mutex_lock(&m_subListMutex);
	std::vector<std::string> subList = m_subList;
	pthread_mutex_unlock(&m_subListMutex);

	return subList;
}

/*
 *function: init the m_rRotSet
 */
map<string, unsigned long> DataPanel::getRRotSet() {
	map<string, unsigned long> rRotSet;
	if (m_fib.size() == 0)
		return rRotSet;

	//get all the RRots
	for (vector<FIBItem>::iterator iter = m_fib.begin(); iter != m_fib.end();
			iter++) {
		string name = iter->_contentName;
		if (name.find("RRot-") != string::npos) {
			rRotSet.insert(pair<string, unsigned long>(name, 0));
		}
	}
	//get the MD5 ID of each RRot
	for (map<string, unsigned long>::iterator iter = rRotSet.begin();
			iter != rRotSet.end(); iter++) {
		unsigned long md5ID = getMD5ID(iter->first);
		rRotSet[iter->first] = md5ID;
	}
	return rRotSet;
}
/*
 * function: the main logical handle function fo the control panel
 */
void DataPanel::running() {
	Message msg;

	while (isRunning()) {

		msg = recevMsg();

		handleMsg(msg);
	}
}
