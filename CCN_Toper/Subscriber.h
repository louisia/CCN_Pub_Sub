/*
 * Subscriber.h
 *
 *  Created on: Mar 20, 2016
 *      Author: njnu
 */

#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include "../CCN_Layer/DataPanel.h"
#include "SubscribeThreading.h"

class Subscriber: public DataPanel {
public:
	Subscriber(std::string localIP);
	virtual ~Subscriber();

	void subscribe();

protected:

	virtual void handleMsg(Message& msg);

private:

	void handlePublish(Message& msg);

	std::string getContentName(std::vector<std::string> subList);

	void subscribeSendMsg(std::string contentName, std::string rendezvous);

private:
	SubscribeThreading m_subscribeThreading; //the threading for sending subscription message
	int m_sequenceNumber;
	map<string,bool>m_subscribeCdList;
	bool m_isSubListNew;
};

#endif /* SUBSCRIBER_H_ */
