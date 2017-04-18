/*
 * EPSRouter.h
 *
 *  Created on: 2017年3月14日
 *      Author: louisia
 */


#ifndef CCN_TOPER_EPSROUTER_H_
#define CCN_TOPER_EPSROUTER_H_
#include "Router.h"
#include"MulticastThreading.h"

class EPS_Router: public Router {
public:
	EPS_Router(std::string localIp);
	virtual ~EPS_Router();
	void multicast();
protected:
	virtual void handleMsg(Message& msg);
private:
	virtual void handleSubscribe(Message& msg);
	virtual void handlePublish(Message& msg);
	virtual void handleRPublish(Message& msg);
	virtual void handleGroupIpResponse(Message& msg);
	void setSocketOpt();
	void joinMulticastGroup(string groupIP);
	void leaveMulticastGroups(vector<string> groupIPs);

	void initMulticastSocket();

private:
	MulticastThreading m_multicastThreading;
	int m_multicastSocket;
};

#endif /* CCN_TOPER_EPSROUTER_H_ */
