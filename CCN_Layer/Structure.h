/*
 * Structure.h
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include <vector>
#include <map>
#include <iterator>
#include <list>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>
#include <string.h>
#include<bitset>
using namespace std;

/********************************the structure of communication*********************************************/
#define SOCK_TIMEOUT 20
typedef int Time_t;

#define PORTMIN 9000                //the min port number
#define PORTMAX 9120                //the max port number

#define SENDPORT 9000
#define RECEVPORT 9001

#define CBFLENGTH 15

#define GROUPIPPREFIX "ff0E"
typedef int FaceID;



enum MessageType {
	UDP,
	IGMP
};
#define GROUPPORT 9500

typedef struct _Message {
	FaceID _faceID;                 //the id of face to send or received

	int _bufferLength;              //the length of the buffer
	char* _buffer; //the pointer to the buffer, and the pointer should not be a constant pointer

	int _msgLength; //the length of the message, which smaller than the length of the buffer
	MessageType _type;
	std::string _multicastIp;

	_Message* dup()                  //create the copy of current message
	{
		_Message* cp = new _Message();

		//make the copy of msg--modify after 2016.06.17
		cp->_faceID = _faceID;
		cp->_bufferLength = _bufferLength;
		cp->_msgLength = _msgLength;
		cp->_buffer = new char[_bufferLength];
		cp->_type=_type;
		cp->_multicastIp=_multicastIp;
		memcpy(cp->_buffer, _buffer, _bufferLength);
		return cp;
	}

	void release() {
		if (_buffer != NULL)
			delete[] _buffer;

		_buffer = NULL;
		_bufferLength = 0;
		_msgLength = -1;
		_faceID = 0;
		_type=UDP;
		_multicastIp="";
	}
} Message;

typedef struct _PacketID {
	char _id[17];                 //store the md5 and extra zero, md5(16) | 0(1)
} PacketID;

typedef unsigned int SliceID;

#define SLICER_SIZE 512
//the structure to store the slice data
typedef struct _Slice {
	PacketID _pid;                  //the id of the message packet
	SliceID _sid;                   //the id of the current slice
	bool _isLast;                   //mark the last slice of the message packet
	int _sliceSize;                 //the size of data
	char* _sliceData;               //the pointer of slice data
} Slice;

typedef struct _SliceBuffer {
	PacketID _id;                    //the id of the message packet
	std::vector<Slice*> _sliceSet; //the pointer set of slice that belongs to the same message
	bool _isUsed;                    //to mark the buffer whether used
	bool _isCompleted;              //to mark whether the buffer complete or not
	FaceID _recevFaceID;             //the faceID that received the message
	Time_t _time;                    //the recent receive time of the slice
} SliceBuffer;

/********************************the structure of reformer*********************************************/
#define REQUEST_TIME_OUT SOCK_TIMEOUT*10*5
#define REFORMER_TIME_OUT REQUEST_TIME_OUT

/********************************the structure of topology*********************************************/
typedef int Port;

enum Role {
	Ctlr = 0, Rot = 1, Sub = 2, RRot = 3, Pub = 4,ERot=5
};

typedef struct _RoleID {
	Role _role;                      //the role of the node
	int _id;                         //the id of the role
} RoleID;

typedef struct _CommunicateAddr
{
    in6_addr  _ip;                  //the ip of the node

    Port _csPort;                    //the port to the controller for sending
    Port _crPort;                    //the port to the controller for receving
    Port _nsPort;                    //the port to the node for sending
    Port _nrPort;                    //the port to the node for receving
} CommunicateAddr;

//the item of the router table
typedef struct _RouterItem {
	RoleID _des;                     //the roleID of the destination node
	std::vector<RoleID> _nextSet; //the set of the next hop to route to the destination
} RouterItem;

#define MAXHOP 100

/********************************the structure of Controller*********************************************/

#define CTLIP "10.0.0.130"

enum PacketType {
	Regist = 0,
	RouterTable = 1,
	AdjacentTable = 2,
	IDAssigned ,
	SubListUpload,
	SubListDownload ,
	SubListRequest ,
	Subscribe ,
	Publish ,
	RPublish ,
	GroupIpRequest,
	GroupIpResponse
};


/********************************the structure of ControlPanel*********************************************/
typedef struct _AdjacentItem {
	RoleID _roleID;                      //the roleID of the adjacent node
	CommunicateAddr _comAddr;   //the communication address of the adjacent node
} AdjacentItem;

/********************************the structure of DataPanel************************************************/
typedef struct _FIBItem {
	std::string _contentName;            //the name of the content
	std::vector<int> _faceList;        //the face list to translate
} FIBItem;

bool operator<(const FIBItem& item1, const FIBItem& item2);

typedef unsigned long LinkTag; //the link tag which use the number of the message send

Time_t getCurrentTime();                 //get the current time
Time_t translateTime(string t); //translate the time with string format into the time with system format

/********************************the structure of router******************************************/
typedef struct _IFTItem {
	FaceID _incomeFaceID;                //the face id of the message arriving
	LinkTag _incomeLinkTag;              //the link tag of the message received

	FaceID _outgoingFaceID;              //the face id of the message forwarding
	LinkTag _outgoingLinkTag;           //the link tag of the message forwarding

	Time_t _sendTime;               //the sending time of the message forwarding
} IFTItem;

/********************************the structure of checker******************************************/
#define CHECHER_TIME_OUT 20



bool operator<(const RoleID& a, const RoleID& b);

/********************************the structure of STItem************************************************/

typedef struct _STItem {
	int _faceID;
	std::vector<std::string> _cdsList;
	int _bitmap[CBFLENGTH]; //bitset数组
} STItem;

typedef struct _CDGroupIPRelation{
	string _contentName;
	string _multicastIp;
	int _membercount;
}CDGroupIPRelation;

bool isInBF(STItem &st, string &str);
void setBF(STItem &st, string &str);
void resetBF(STItem &st, string &str);

unsigned long getMD5ID(string name) ;
std::string hRW(std::string contentName,
			map<string, unsigned long> rRotSet);
std::vector<std::string> getPrexfixs(std::string str);

void printMsg(const char* msg, int len, Role r);

#endif /* STRUCTURE_H_ */
