/*
 * test.cpp
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#include "../CCN_Layer/Buffer.h"
#include "../CCN_Layer/Panel.h"

#include "../CCN_Layer/Slicer.h"
#include "../CCN_Layer/Reformer.h"

#include <iostream>
#include <fstream>
#include <exception>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <linux/if.h>

#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <errno.h>

#include "../CCN_Layer/Topology.h"

#include "../CCN_Layer/Controller.h"
#include "../CCN_Layer/Node.h"
#include "../CCN_Layer/ControlPanel.h"
#include "../CCN_Layer/DataPanel.h"

#include "PSRouter.h"
#include "Publisher.h"
#include "RPSRouter.h"
#include "Subscriber.h"

#include <list>
#include <set>
#include <time.h>
#include <vector>
using namespace std;

void createSub(string localip, string controllerip, string outfilename) {
	ofstream ofs(outfilename.c_str());
	std::cout.rdbuf(ofs.rdbuf());
	std::cerr.rdbuf(ofs.rdbuf());
	Subscriber subscirber(localip);
	ControlPanel controlPanel(&subscirber, localip, controllerip);
	controlPanel.setRole(Sub);
	controlPanel.running();
}

void createPub(string localip, string controllerip, string outfilename) {
	ofstream ofs(outfilename.c_str());
	std::cout.rdbuf(ofs.rdbuf());
	std::cerr.rdbuf(ofs.rdbuf());

	Publisher pubisher(localip);
	ControlPanel controlPanel(&pubisher, localip, controllerip);
	controlPanel.setRole(Pub);
	controlPanel.running();
}

void createEPS(string localip, string controllerip, string outfilename) {
	ofstream ofs(outfilename.c_str());
	std::cout.rdbuf(ofs.rdbuf());
	std::cerr.rdbuf(ofs.rdbuf());

	EPS_Router epsrouter(localip);
	ControlPanel controlPanel(&epsrouter, localip, controllerip);
	controlPanel.setRole(ERot);
	controlPanel.running();

}

void createPS(string localip, string controllerip, string outfilename) {
	ofstream ofs(outfilename.c_str());
	std::cout.rdbuf(ofs.rdbuf());
	std::cerr.rdbuf(ofs.rdbuf());

	PS_Router psrouter(localip);
	ControlPanel controlPanel(&psrouter, localip, controllerip);
	controlPanel.setRole(Rot);
	controlPanel.running();

}

void createRPS(string localip, string controllerip, string outfilename) {
	ofstream ofs(outfilename.c_str());
	std::cout.rdbuf(ofs.rdbuf());
	std::cerr.rdbuf(ofs.rdbuf());

	RPS_Router rpsrouter(localip);
	ControlPanel controlPanel(&rpsrouter, localip, controllerip);
	controlPanel.setRole(RRot);
	controlPanel.running();

}

int main(int argc, char** argv) {

	if (argc == 2) {
		string rolename(argv[0]);
		if (rolename.find("CN") != string::npos) {
			ofstream ofs("../Logs/CN_log.txt");
			std::cout.rdbuf(ofs.rdbuf());
			std::cerr.rdbuf(ofs.rdbuf());
			Controller controller(argv[1]);
			controller.running();
		} else {
			cout << "Usage Example: ./CN loaclip" << endl;
			return 1;
		}

	} else if (argc == 3) {
		string rolename(argv[0]);
		if (rolename.find("RN") != string::npos) {
			string outfilename = "../Logs/" + rolename + "_log.txt";
			createRPS(argv[1], argv[2], outfilename);
		} else if (rolename.find("Sub") != string::npos) {
			string outfilename = "../Logs/" + rolename + "_log.txt";
			createSub(argv[1], argv[2], outfilename);
		} else if (rolename.find("Pub") != string::npos) {
			string outfilename = "../Logs/" + rolename + "_log.txt";
			createPub(argv[1], argv[2], outfilename);
		} else if (rolename.find("EN") != string::npos) {
			string outfilename = "../Logs/" + rolename + "_log.txt";
			createEPS(argv[1], argv[2], outfilename);
		} else if (rolename.find("PSN") != string::npos) {
			string outfilename = "../Logs/" + rolename + "_log.txt";
			createPS(argv[1], argv[2], outfilename);
		} else {
			cout << "argv[0] error :CN RN Sub Pub EN PSN" << endl;
		}

	} else {
		cout << "Usage Example: ./roleName loaclip controllerip" << endl;
		return 1;
	}

	return 0;
}

