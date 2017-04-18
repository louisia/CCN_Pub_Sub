#include "Structure.h"

#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include<openssl/md5.h>

int arr[] = { 11, 22, 33, 44, 55, 66, 77 };
std::vector<int> hashCode(arr, arr + sizeof(arr) / sizeof(int));

bool operator<(const FIBItem& item1, const FIBItem& item2) {
	return item1._contentName < item2._contentName;
}

bool operator<(const RoleID& a, const RoleID& b) {
	return a._role == b._role ? a._id < b._id : a._role < b._role;
}

Time_t getCurrentTime() {
	Time_t curTime = time((time_t*) NULL);

	return curTime;
}

Time_t translateTime(string t) {
	struct tm tm_time;
	strptime(t.c_str(), "%Y-%m-%d %H:%M:%S", &tm_time);

	return mktime(&tm_time);
}

vector<string> getPrexfixs(string str) {
	string tmp = str.substr(1, str.length());

	vector<string> result;

	int prepos = 0;
	string::size_type pos = 0;
	while ((pos = tmp.find("/", prepos)) != string::npos) {
		result.push_back(str.substr(0, pos + 1));
		prepos = pos + 1;
	}

	result.push_back(str);
	return result;

}
/*
 *function: get the MD5 value of a string
 *parameters: the description of the content or the RRot
 */
unsigned long getMD5ID(string name) {
	unsigned char md[16];
	//md5返回16个十进制数,放在md[]中
	MD5((const unsigned char*) name.c_str(), name.length(), md);
	unsigned long result = 0;
	for (int i = 0; i < 16; i++) {
		unsigned short temp = (unsigned short) md[i];
		result = result * 10 + temp;
	}
	return result;
}
/*
 *function: use hrw to get the RRot with the highest weight
 *parameters: the describe of the content
 *						the set of RRots
 */
string hRW(string contentName, map<string, unsigned long> rRotSet) {
	unsigned long a = 1103515245;
	unsigned long b = 12345;
	unsigned long contentNameID = getMD5ID(contentName);
	unsigned long max = 0;
	string result = "";
	for (map<string, unsigned long>::iterator iter = rRotSet.begin();
			iter != rRotSet.end(); iter++) {
		unsigned long weight =
				(a * ((a * iter->second + b) ^ contentNameID) + b);
		cout << "**********************weight start测试****************" << endl;
		cout << "contentName:" << contentName << ",RRot:" << iter->first
				<< ",weight:" << weight << endl;
		cout << "**********************weight end测试****************" << endl;
		if (weight > max) {
			max = weight;
			result = iter->first;
		}
	}

	return result;
}
/*
 *
 *function:set the bitmap of each STItem
 *parameter:st,str
 */
void setBF(STItem &st, string &str) {
	unsigned long result = getMD5ID(str);
	for (vector<int>::size_type i = 0; i < hashCode.size(); i++) {
		unsigned long index = hashCode[i] * result;
		st._bitmap[index % CBFLENGTH] += 1;
	}
}
void resetBF(STItem &st, string &str) {
	unsigned long result = getMD5ID(str);
	for (vector<int>::size_type i = 0; i < hashCode.size(); i++) {
		unsigned long index = hashCode[i] * result;
		if (st._bitmap[index % CBFLENGTH] > 0)
			st._bitmap[index % CBFLENGTH] -= 1;
	}
}
/*
 *function:judge whether the str is in  the st BF
 *parameter: st,str
 *return: true:in,false:not in
 *
 */
bool isInBF(STItem &st, string &str) {
	unsigned long result = getMD5ID(str);
	for (vector<int>::size_type j = 0; j < hashCode.size(); j++) {
		unsigned long index = hashCode[j] * result;
		if (st._bitmap[index % CBFLENGTH] == 0)
			return false;
	}
	return true;
}

void printMsg(const char* msg, int len) {
	std::cout << "--------------Message Print--------------" << std::endl;

	if (len <= 0) {
		cout << "len <= 0... " << len << endl;
		return;
	}

	//analysis the msg type
	PacketType type;

	int pos = 0;

	//--------------------Regist--------------------
	in6_addr addr;

	Role role;

	Port csPort;
	Port crPort;

	Port nsPort;
	Port nrPort;

	//--------------------IDAssigned-----------------
	int id = -1;

	RoleID roleID;

	//--------------------AdjacentTable----------------
	int numOfNode = 0;
	int numOfAdjacent = -1;
	CommunicateAddr comAddr;

	//--------------------RouterTable----------------
	int numOfNextHop = 0;
	int numOfItem = 0;
	std::vector<RouterItem> routerTable;

	//---------------------SubListUpload-------------------
	int elementNum = 0;
	vector<string> subList;
	int elementLength = 0;

	//---------------------SubListDownload-------------------

	std::string rendezvous;
	std::string contentName;
	std::string content;
	std::string groupIp;

	int size;

	int sequenceNumber;

	memcpy(&type, msg + pos, sizeof(PacketType));
	pos += sizeof(PacketType);

	switch (type) {
	case Regist:
		std::cout << "packet type: Regist" << std::endl;

		memcpy(&role, msg + pos, sizeof(Role));
		pos += sizeof(Role);

		char tmp[128];
		memcpy(&addr, msg + pos, sizeof(in6_addr));
		pos += sizeof(in6_addr);
		inet_ntop(AF_INET6, (void *) &addr, tmp, sizeof(tmp));

		std::cout << "addr: " << tmp << std::endl;

		memcpy(&csPort, msg + pos, sizeof(Port));
		pos += sizeof(Port);

		memcpy(&crPort, msg + pos, sizeof(Port));
		pos += sizeof(Port);

		memcpy(&nsPort, msg + pos, sizeof(Port));
		pos += sizeof(Port);

		memcpy(&nrPort, msg + pos, sizeof(Port));
		pos += sizeof(Port);

		std::cout << "csPort: " << csPort << std::endl;
		std::cout << "crPort: " << crPort << std::endl;
		std::cout << "nsPort: " << nsPort << std::endl;
		std::cout << "nrPort: " << nrPort << std::endl;

		break;

	case IDAssigned:
		std::cout << "packet type: IDAssigned" << std::endl;

		memcpy(&id, msg + pos, sizeof(int));
		pos += sizeof(Port);

		std::cout << "IDAssigned: " << id << std::endl;

		break;

	case AdjacentTable:
		/*
		 * the format of message of adjacent table
		 * PacketType | CurrentRole | CurrentID | NumOfAdjacent | AdjacentSet
		 *
		 * the format of adjacent set
		 * Role | ID | CommunicateAddr, the order mean the index of interface linked
		 *
		 * the format of CommunicateAddr
		 * IP | CSPort | CRPort | NSPort | NRPort
		 */
		std::cout << "packet type: AdjacentTable" << std::endl;

		//read the head of the message
		memcpy(&roleID._role, msg + pos, sizeof(Role));
		pos += sizeof(Role);

		memcpy(&roleID._id, msg + pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&numOfAdjacent, msg + pos, sizeof(int));
		pos += sizeof(int);

		std::cout << "current roleID._role: " << roleID._role
				<< " current roleID._id: " << roleID._id << std::endl;
		std::cout << "numOfAdjacent: " << numOfAdjacent << std::endl;

		//write the adjacent information
		for (int j = 0; j < numOfAdjacent; ++j) {
			memcpy(&roleID._role, msg + pos, sizeof(Role));
			pos += sizeof(Role);

			memcpy(&roleID._id, msg + pos, sizeof(int));
			pos += sizeof(int);

			//read the ip information
			memcpy(&comAddr, msg + pos, sizeof(CommunicateAddr));
			pos += sizeof(CommunicateAddr);

			char tmp[128];

			inet_ntop(AF_INET6, (void *) &(comAddr._ip), tmp, sizeof(tmp));

			std::cout << "addr: " << tmp << std::endl;
			std::cout << "csPort: " << comAddr._csPort << std::endl;
			std::cout << "crPort: " << comAddr._crPort << std::endl;
			std::cout << "nsPort: " << comAddr._nsPort << std::endl;
			std::cout << "nrPort: " << comAddr._nrPort << std::endl;
		}

		break;

	case RouterTable:
		/*
		 * the format of the message of router table
		 * PacketType | NumOfItem | RouterItem
		 *
		 * the format of RouterItem
		 * DestinationRoleID | NumberOfNextHop | NextHopSet
		 *
		 * the format of NextHopSet
		 * Role | ID
		 */
		std::cout << "packet type: RouterTable" << std::endl;

		memcpy(&numOfItem, msg + pos, sizeof(int));
		pos += sizeof(int);

		for (int j = 0; j < numOfItem; ++j) {
			RouterItem item;

			memcpy(&roleID, msg + pos, sizeof(RoleID));
			pos += sizeof(RoleID);

			item._des = roleID;

			memcpy(&numOfNextHop, msg + pos, sizeof(int));
			pos += sizeof(int);

			for (int k = 0; k < numOfNextHop; ++k) {
				memcpy(&roleID, msg + pos, sizeof(RoleID));
				pos += sizeof(RoleID);

				item._nextSet.push_back(roleID);
			}

			routerTable.push_back(item);
		}

		for (std::vector<int>::size_type j = 0; j != routerTable.size(); ++j) {
			RoleID roleId = routerTable[j]._des;

			std::cout << "_role: " << roleId._role << "_id: " << roleId._id
					<< std::endl;

			std::vector<RoleID> nextSet = routerTable[j]._nextSet;

			for (std::vector<int>::size_type j = 0; j != nextSet.size(); ++j) {
				std::cout << "nextHop._role: " << nextSet[j]._role
						<< " nextHop._id: " << nextSet[j]._id << " ";
			}
			std::cout << std::endl;
		}

		break;

	case SubListUpload:
		//upload
		/*
		 * the format of the upload message
		 * PacketType | Role | ID | ElementNumber | ElementSet
		 *
		 * the format of the ElementSet
		 * ElementLength | Element
		 */

		memcpy(&roleID._role, msg + pos, sizeof(Role));
		pos += sizeof(Role);

		memcpy(&roleID._id, msg + pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&elementNum, msg + pos, sizeof(int));
		pos += sizeof(int);

		for (int i = 0; i < elementNum; ++i) {
			//read the length of the element
			memcpy(&elementLength, msg + pos, sizeof(int));
			pos += sizeof(int);

			//read the content of the element
			string str(msg + pos, elementLength);
			pos += elementLength;

			subList.push_back(str);
		}

		for (vector<string>::size_type i = 0; i != subList.size(); ++i) {
			cout << subList[i] << endl;
		}
		break;

	case SubListDownload:

		//download
		/*
		 * the format of the upload message
		 * PacketType | Role | ID | ElementNumber | ElementSet
		 *
		 * the format of the ElementSet
		 * ElementLength | Element
		 */

		memcpy(&roleID._role, msg + pos, sizeof(Role));
		pos += sizeof(Role);

		memcpy(&roleID._id, msg + pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&elementNum, msg + pos, sizeof(int));
		pos += sizeof(int);

		for (int i = 0; i < elementNum; ++i) {
			//read the length of the element
			memcpy(&elementLength, msg + pos, sizeof(int));
			pos += sizeof(int);

			//read the content of the element
			string str(msg + pos, elementLength);
			pos += elementLength;

			subList.push_back(str);
		}

		for (vector<string>::size_type i = 0; i != subList.size(); ++i) {
			cout << subList[i] << endl;
		}

		break;

	case SubListRequest:
		memcpy(&type, msg + pos, sizeof(PacketType));
		pos += sizeof(PacketType);

		memcpy(&roleID._role, msg + pos, sizeof(Role));
		pos += sizeof(Role);

		memcpy(&roleID._id, msg + pos, sizeof(int));
		pos += sizeof(int);

		break;

	case Subscribe:
	case GroupIpRequest:
		cout << "PacketType:	" << type << endl;

		memcpy(&roleID._role, msg + pos, sizeof(Role));
		pos += sizeof(Role);
		cout << "role		" << roleID._role << endl;

		memcpy(&roleID._id, msg + pos, sizeof(int));
		pos += sizeof(int);
		cout << "id		" << roleID._id << endl;

		memcpy(&len, msg + pos, sizeof(int));
		pos += sizeof(int);

		rendezvous = std::string(msg + pos, len);
		pos += len;
		cout << "rendezvous		" << rendezvous << endl;

		memcpy(&len, msg + pos, sizeof(int));
		pos += sizeof(int);

		contentName = std::string(msg + pos, len);
		pos += len;
		cout << "contentID		" << contentName << endl;

		memcpy(&sequenceNumber, msg + pos, sizeof(int));
		pos += sizeof(int);
		cout << "sequenceNumber		" << sequenceNumber << endl;

		break;

	case RPublish:
		cout << "PacketType:	" << type << endl;

		memcpy(&roleID._role, msg + pos, sizeof(Role));
		pos += sizeof(Role);
		cout << "role		" << roleID._role << endl;

		memcpy(&roleID._id, msg + pos, sizeof(int));
		pos += sizeof(int);
		cout << "id		" << roleID._id << endl;

		int rlen;
		memcpy(&rlen, msg + pos, sizeof(int));
		pos += sizeof(int);

		rendezvous = std::string(msg + pos, rlen);
		pos += rlen;
		cout << "rendezvous		" << rendezvous << endl;

		memcpy(&len, msg + pos, sizeof(int));
		pos += sizeof(int);

		contentName = std::string(msg + pos, len);
		pos += len;
		cout << "contentName		" << contentName << endl;

		memcpy(&len, msg + pos, sizeof(int));
		pos += sizeof(int);

		content = std::string(msg + pos, len);
		pos += len;
		cout << "content		" << content << endl;

		memcpy(&sequenceNumber, msg + pos, sizeof(int));
		pos += sizeof(int);
		cout << "sequenceNumber		" << sequenceNumber << endl;

		break;

	case Publish:
		cout << "PacketType:	" << type << endl;

		memcpy(&roleID._role, msg + pos, sizeof(Role));
		pos += sizeof(Role);
		cout << "role		" << roleID._role << endl;

		memcpy(&roleID._id, msg + pos, sizeof(int));
		pos += sizeof(int);
		cout << "id		" << roleID._id << endl;

		memcpy(&len, msg + pos, sizeof(int));
		pos += sizeof(int);

		contentName = std::string(msg + pos, len);
		pos += len;

		cout << "contentName		" << contentName << endl;

		memcpy(&len, msg + pos, sizeof(int));
		pos += sizeof(int);

		content = std::string(msg + pos, len);
		pos += len;
		cout << "content		" << content << endl;

		memcpy(&sequenceNumber, msg + pos, sizeof(int));
		pos += sizeof(int);
		cout << "sequenceNumber		" << sequenceNumber << endl;

		break;
	case GroupIpResponse:
		cout << "PacketType:	" << type << endl;
		memcpy(&len, msg + pos, sizeof(int));
		pos += sizeof(int);

		contentName = std::string(msg + pos, len);
		pos += len;
		cout << "contentname		" << contentName << endl;

		memcpy(&len, msg + pos, sizeof(int));
		pos += sizeof(int);

		groupIp = std::string(msg + pos, len);
		pos += len;
		cout << "joinGroupIp		" << groupIp << endl;

		memcpy(&size, msg + pos, sizeof(int));
		pos += sizeof(int);

		for (int i = 0; i < size; i++) {
			memcpy(&len, msg + pos, sizeof(int));
			pos += sizeof(int);

			contentName = std::string(msg + pos, len);
			pos += len;
			cout << "quitGroupCDs		" << contentName << endl;

			memcpy(&len, msg + pos, sizeof(int));
			pos += sizeof(int);

			groupIp = std::string(msg + pos, len);
			pos += len;
			cout << "quitGroupIPs		" << groupIp << endl;

		}

		break;

	default:
		std::cerr << "error packet type..." << type << std::endl;
	}

	std::cout << "-----------------------------------------" << std::endl;
}
