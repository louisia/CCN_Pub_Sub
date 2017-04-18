/*
 * RPSRouter.h
 *
 *  Created on: Mar 20, 2016
 *      Author: njnu
 */

#ifndef RPSROUTER_H_
#define RPSROUTER_H_

#include "Router.h"
#include<set>
class RPS_Router: public Router {
public:
	RPS_Router(std::string localIP);
	virtual ~RPS_Router();

protected:

	virtual void handleMsg(Message& msg);

private:

	virtual void handleSubscribe(Message& msg);

	virtual void handlePublish(Message& msg);

	virtual void handleRPublish(Message& msg);

	virtual void handleGroupIpRequest(Message &msg);

	string hexstr(unsigned char *buf);

	string hexToGroupIP(string hexStr);

private:
	std::vector<CDGroupIPRelation> m_cdGroupIPRelation;
	std::map<int, vector<string> > m_faceGroupIps;

};

#endif /* RPSROUTER_H_ */
