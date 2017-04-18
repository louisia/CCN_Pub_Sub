/*
 * Router.cpp
 *
 *  Created on: Oct 28, 2015
 *      Author: njnu
 */

#include "Router.h"
#include <set>
using namespace std;
Router::Router(std::string localIP) :
		DataPanel(localIP) {
	// TODO Auto-generated constructor stub
}

Router::~Router() {
	// TODO Auto-generated destructor stub
}

bool Router::addSTItem(Message &msg) {

	bool forwardflag = true;
	bool addflag = true;

	int pos = sizeof(PacketType) + sizeof(Role) + sizeof(int) + sizeof(int)
			+ getRendezvousName(msg).length();

	int len = 0;
	memcpy(&len, msg._buffer + pos, sizeof(int));
	pos += sizeof(int);
	std::string contentName = std::string(msg._buffer + pos, len);

	//检查路由器所有接口的CDList中是否有其超条目
	vector<string> prefixs = getPrexfixs(contentName);
	for (vector<STItem>::size_type pos = 0; pos < m_stList.size(); pos++) {
		for (vector<string>::iterator iter = m_stList[pos]._cdsList.begin();
				iter != m_stList[pos]._cdsList.end(); iter++) {
			if (find(prefixs.begin(), prefixs.end(), *iter) != prefixs.end()) {
				forwardflag = false;
				break;
			}

		}
	}

	vector<STItem>::iterator iter;
	for (iter = m_stList.begin(); iter != m_stList.end(); iter++) {
		if ((*iter)._faceID == msg._faceID) {
			break;
		}
	}
	//ST表中之前没有记录该接口
	if (iter == m_stList.end()) {
		STItem st;
		st._faceID = msg._faceID;
		st._cdsList.push_back(contentName);
		for (int i = 0; i < CBFLENGTH; i++) {
			st._bitmap[i] = 0;
		}
		setBF(st, contentName);
		m_stList.push_back(st);
	} else {
		for (vector<string>::iterator iterCD = (*iter)._cdsList.begin();
				iterCD != (*iter)._cdsList.end();) {
			if ((*iterCD).length() > contentName.length()) { //检查当前端口中是否有其子条目
				prefixs = getPrexfixs((*iterCD));
				if (find(prefixs.begin(), prefixs.end(), contentName)
						!= prefixs.end()) {
					resetBF(*iter, *iterCD);
					(*iter)._cdsList.erase(iterCD, iterCD + 1);
				} else {
					iterCD++;
				}
			} else { //检查是有其超条目
				prefixs = getPrexfixs(contentName);
				if (find(prefixs.begin(), prefixs.end(), *iterCD)
						!= prefixs.end()) {
					addflag = false;
					break;
				}
				iterCD++;

			}
		}
		if (addflag) {
			(*iter)._cdsList.push_back(contentName);
			setBF(*iter, contentName);
		}
	}

//	cout << "***************ST construct start 测试************" << endl;
//	for (vector<STItem>::iterator iter = m_stList.begin();
//			iter != m_stList.end(); iter++) {
//		cout << (*iter)._faceID << "	";
//		for (vector<string>::iterator iter1 = (*iter)._cdsList.begin();
//				iter1 != (*iter)._cdsList.end(); iter1++) {
//			cout << (*iter1) << "	";
//		}
//		for (int i = 0; i < CBFLENGTH; i++) {
//			cout << (*iter)._bitmap[i] << "	";
//		}
//		cout << endl;
//	}
//	cout << "***************ST construct end 测试************" << endl;
	return forwardflag;

}

std::set<int> Router::getSTFaceList(Message &msg) {
	string contentName = getRendezvousName(msg);
	vector<string> prefixs = getPrexfixs(contentName);

	set<int> faceSet;
	for (vector<STItem>::iterator iter = m_stList.begin();
			iter != m_stList.end(); iter++) {
		for (vector<string>::iterator iter2 = prefixs.begin();
				iter2 != prefixs.end(); iter2++) {
			if (isInBF(*iter, *iter2)) {
				faceSet.insert(iter->_faceID);
				break;
			}
		}
	}

//	cout << "***************contentNameList start 测试************" << endl;
//	for (vector<string>::iterator iter = prefixs.begin(); iter != prefixs.end();
//			iter++) {
//		cout << *iter << endl;
//	}
//	cout << "***************contentNameList end 测试************" << endl;
//
//	cout << "***************STList start 测试************" << endl;
//	for (vector<STItem>::iterator iter = m_stList.begin();
//			iter != m_stList.end(); iter++) {
//		cout << (*iter)._faceID << "	";
//		for (vector<string>::iterator iter1 = (*iter)._cdsList.begin();
//				iter1 != (*iter)._cdsList.end(); iter1++) {
//			cout << (*iter1) << "	";
//		}
//		for (int i = 0; i < CBFLENGTH; i++) {
//			cout << (*iter)._bitmap[i] << "	";
//		}
//		cout << endl;
//	}
//	cout << "***************STList end 测试************" << endl;
//
//	cout << "***************faceIDList start 测试************" << endl;
//	for (set<int>::iterator iter = faceSet.begin(); iter != faceSet.end();
//			iter++) {
//		cout << *iter << endl;
//	}
//	cout << "***************faceIDList end 测试************" << endl;
	return faceSet;
}

string Router::getRendezvousName(Message &msg) {
	int pos = sizeof(PacketType) + sizeof(Role) + sizeof(int);
	int len;
	memcpy(&len, msg._buffer + pos, sizeof(int));
	pos += sizeof(int);
	string rendezvous = std::string(msg._buffer + pos, len);
	return rendezvous;
}
