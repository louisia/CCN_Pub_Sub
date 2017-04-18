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


void createSub(string localip,string controllerip, string infilename,
		string outfilename) {

	ofstream ofs(outfilename.c_str());
	std::cout.rdbuf(ofs.rdbuf());
	std::cerr.rdbuf(ofs.rdbuf());

	Subscriber subscirber(localip, infilename);
	ControlPanel controlPanel(&subscirber, localip, controllerip);
	controlPanel.setRole(Sub);
	controlPanel.running();
}
void createPub( string localip, string controllerip,string infilename,
		string outfilename) {
	ofstream ofs(outfilename.c_str());
	std::cout.rdbuf(ofs.rdbuf());
	std::cerr.rdbuf(ofs.rdbuf());

	Publisher pubisher(localip);
	ControlPanel controlPanel(&pubisher, localip, controllerip);
	controlPanel.setRole(Pub);
	controlPanel.running();
}
void createPS(string localip, string controllerip, string infilename,
		string outfilename) {
	ofstream ofs(outfilename.c_str());
	std::cout.rdbuf(ofs.rdbuf());
	std::cerr.rdbuf(ofs.rdbuf());

	PS_Router psrouter(localip);
	ControlPanel controlPanel(&psrouter, localip, controllerip);
	controlPanel.setRole(Rot);
	controlPanel.running();

}
void createEPS(string localip, string controllerip, string infilename,
		string outfilename) {
	ofstream ofs(outfilename.c_str());
	std::cout.rdbuf(ofs.rdbuf());
	std::cerr.rdbuf(ofs.rdbuf());

	EPS_Router epsrouter(localip);
	ControlPanel controlPanel(&epsrouter, localip, controllerip);
	controlPanel.setRole(ERot);
	controlPanel.running();

}
void createRPS(string localip, string controllerip, string infilename,
		string outfilename) {
	ofstream ofs(outfilename.c_str());
	std::cout.rdbuf(ofs.rdbuf());
	std::cerr.rdbuf(ofs.rdbuf());

	RPS_Router rpsrouter(localip);
	ControlPanel controlPanel(&rpsrouter, localip, controllerip);
	controlPanel.setRole(RRot);
	controlPanel.running();

}

void testRunning1(string localip,string controllerip) {
	int id = fork();
	if (id != 0) {
		ofstream ofs("Logs/controller_log");
		std::cout.rdbuf(ofs.rdbuf());
		std::cerr.rdbuf(ofs.rdbuf());
		Controller controller(controllerip);
		controller.running();

	} else {
		sleep(1);

		int id = fork();

		if (id != 0) {
			createSub(localip, controllerip, "Files/subscribecontent.txt",
					"Logs/subscirber_log");

		} else {
			sleep(1);

			int id = fork();

			if (id != 0) {

				createPub(localip, controllerip, "", "Logs/publisher_log");

			} else {
				sleep(1);

				int id = fork();

				if (id != 0) {

					createPS(localip, controllerip, "", "Logs/psrouter_log");

				} else {
					sleep(1);

					int id = fork();

					if (id != 0) {

						createEPS(localip, controllerip, "", "Logs/epsrouter_log");

					} else {
						sleep(1);

						int id = fork();

						if (id != 0) {

							createRPS(localip, controllerip, "", "Logs/rpsrouter_log");

						}

					}

				}
			}
		}
	}
}

void testRunning23(string localip,string controllerip) {
	int id = fork();
	if (id != 0) {
		ofstream ofs("Logs/controller_log");
		std::cout.rdbuf(ofs.rdbuf());
		std::cerr.rdbuf(ofs.rdbuf());
		Controller controller(controllerip);
		controller.running();

	} else {
		sleep(1);

		int id = fork();

		if (id != 0) {
			createSub(localip, controllerip, "Files/subscribecontent.txt",
					"Logs/subscirber_log");

		} else {
			sleep(1);

			int id = fork();

			if (id != 0) {

				createPub(localip, controllerip, "", "Logs/publisher_log");

			} else {
				sleep(1);

				int id = fork();

				if (id != 0) {

					createPS(localip, controllerip, "", "Logs/psrouter_log");

				} else {
					sleep(1);

					int id = fork();

					if (id != 0) {

						createEPS(localip, controllerip, "", "Logs/epsrouter_log");

					} else {
						sleep(1);

						int id = fork();

						if (id != 0) {

							createRPS(localip, controllerip, "", "Logs/rpsrouter_log");

						} else {
							sleep(1);

							int id = fork();

							if (id != 0) {

								createSub(localip, controllerip,
										"Files/subscribecontent1.txt",
										"Logs/subscirber_log");
							}
						}

					}

				}
			}
		}
	}
}

void testRunning4(string localip,string controllerip) {
	int id = fork();
	if (id != 0) {
		ofstream ofs("Logs/controller_log");
		std::cout.rdbuf(ofs.rdbuf());
		std::cerr.rdbuf(ofs.rdbuf());
		Controller controller(controllerip);
		controller.running();

	} else {
		sleep(1);

		int id = fork();

		if (id != 0) {
			createSub(localip, controllerip, "Files/subscribecontent.txt",
					"Logs/subscirber_log");

		} else {
			sleep(1);

			int id = fork();

			if (id != 0) {

				createPub(localip, controllerip, "", "Logs/publisher_log");

			} else {
				sleep(1);

				int id = fork();

				if (id != 0) {

					createPS(localip, controllerip, "", "Logs/psrouter_log");

				} else {
					sleep(1);

					int id = fork();

					if (id != 0) {

						createEPS(localip, controllerip, "", "Logs/epsrouter_log");

					} else {
						sleep(1);

						int id = fork();

						if (id != 0) {

							createRPS(localip, controllerip, "", "Logs/rpsrouter_log");

						} else {
							sleep(1);

							int id = fork();

							if (id != 0) {
								createSub(localip, controllerip,
										"Files/subscribecontent1.txt",
										"Logs/subscirber1_log");

							} else {
								sleep(1);

								int id = fork();

								if (id != 0) {
									createSub(localip, controllerip,
											"Files/subscribecontent2.txt",
											"Logs/subscirber2_log");

								} else {
									sleep(1);

									int id = fork();

									if (id != 0) {
										createEPS(localip, controllerip, "",
												"Logs/epsrouter1_log");

									}
								}
							}
						}

					}

				}
			}
		}
	}
}

void testRunning5(string localip,string controllerip) {
	int id = fork();
	if (id != 0) {
		ofstream ofs("Logs/controller_log");
		std::cout.rdbuf(ofs.rdbuf());
		std::cerr.rdbuf(ofs.rdbuf());
		Controller controller(controllerip);
		controller.running();

	} else {
		sleep(1);

		int id = fork();

		if (id != 0) {
			createSub(localip, controllerip, "Files/subscribecontent.txt",
					"Logs/subscirber_log");

		} else {
			sleep(1);

			int id = fork();

			if (id != 0) {

				createPub(localip, controllerip, "", "Logs/publisher_log");

			} else {
				sleep(1);

				int id = fork();

				if (id != 0) {

					createPS(localip, controllerip, "", "Logs/psrouter_log");

				} else {
					sleep(1);

					int id = fork();

					if (id != 0) {

						createEPS(localip, controllerip, "", "Logs/epsrouter_log");

					} else {
						sleep(1);

						int id = fork();

						if (id != 0) {

							createRPS(localip, controllerip, "", "Logs/rpsrouter_log");

						} else {
							sleep(1);

							int id = fork();

							if (id != 0) {
								createSub(localip, controllerip,
										"Files/subscribecontent1.txt",
										"Logs/subscirber1_log");

							}
						}

					}

				}
			}
		}
	}
}

int main(int argc, char** argv) {

	string localip="::1";
	string controllerip="::1";
	testRunning4(localip,controllerip);

	return 0;
}
