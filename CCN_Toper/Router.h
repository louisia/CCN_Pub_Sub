/*
 * Router.h
 *
 *  Created on: Oct 28, 2015
 *      Author: njnu
 */

#ifndef ROUTER_H_
#define ROUTER_H_

#include "../CCN_Layer/DataPanel.h"
#include<set>
class Router: public DataPanel {
public:
	Router(std::string localIP);
	virtual ~Router();

protected:


	virtual bool addSTItem(Message &msg);

	virtual std::set<int> getSTFaceList(Message &msg);

	virtual std::string getRendezvousName(Message &msg);

private:
	std::vector<STItem> m_stList;

};

#endif /* ROUTER_H_ */
