/*
 * PSRouter.h
 *
 *  Created on: Mar 20, 2016
 *      Author: njnu
 */

#ifndef PSROUTER_H_
#define PSROUTER_H_

#include "Router.h"
#include "MulticastThreading.h"
#include<set>

class PS_Router: public Router {
public:
	PS_Router(std::string localIP);
	virtual ~PS_Router();

protected:

	virtual void handleMsg(Message& msg);

private:

	virtual void handleSubscribe(Message& msg);


	virtual void handlePublish(Message& msg);

	virtual void handleRPublish(Message& msg);

};

#endif /* PSROUTER_H_ */
