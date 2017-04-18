/*
 * ControlPanel.h
 *
 *  Created on: Sep 20, 2015
 *      Author: njnu
 */

#ifndef CONTROLPANEL_H_
#define CONTROLPANEL_H_

#include "Panel.h"
#include "DataPanel.h"

class ControlPanel: public Panel {
public:
	ControlPanel(DataPanel* pDataPanel, std::string localIP,
			std::string controllerIP);
	virtual ~ControlPanel();

	/*
	 * function: the main logical handle function fo the control panel
	 */
	void running();
	/*
	 * function: regist to the controller
	 * parameters: role: the role to register
	 * return value: the id of the role applied
	 */
	int regist(Role role);
	/*
	 * function: set the role of current node
	 */
	void setRole(Role role);

private:

	/*
	 * function: init the control panel
	 * parameters: controllerIP : the ip of the controller
	 */
	void initControlPanel(std::string controllerIP);

	/*
	 * function: translate the message to adjacent node table
	 * parameters: msg: the message to translate
	 */
	void handleAdjacentTable(Message& msg);

	/*
	 * function: translate the message to router table
	 * parameters: msg: the message to translate
	 */
	void handleRouterTable(Message& msg);

	/*
	 * function: set the pointer of the data panel
	 * parameters: pDataPanel: the pointer of the data panel to set
	 */

	/*
	 * function: inject the transmission face table into the data panel
	 */
	void injectTransFaceTableToDataPanel();

	/*
	 * function: inject the FIB to the data panel
	 */
	void injectFIBToDataPanel();

private:

	/*
	 * function: start the sub list loader according the role type of current node
	 */
	void startSubListLoader();

private:
	DataPanel* m_pDataPanel;                     //the pointer of the data panel

	std::vector<AdjacentItem> m_adjacentTable;     //the address of the adjacent
	std::vector<RouterItem> m_routerTable;                 //the router table

	RoleID m_roleID;                      //the role of current node to register

	Checker* m_subListloader; //the loader of the subscription list including upload or download
};

#endif /* CONTROLPANEL_H_ */
