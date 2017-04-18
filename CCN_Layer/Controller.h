/*
 * Controller.h
 *
 *  Created on: Sep 15, 2015
 *      Author: njnu
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "Panel.h"
#include "Topology.h"

class Controller : public Panel {
public:
    Controller( std::string localIP );
    virtual ~Controller();

    /*
     * function: the main logical handle of the Controller
     */
    void running();

private:
    /*
     * function: init the controller
     */
    void initController();

    /*
     * function: apply the node for the role
     * parameters: comAddr: the communicate information set
     * return value: the id of the role applied
     */
    int applyNode( Role role, CommunicateAddr comAddr );

    /*
     * function: handle the register
     * parameters: msg: the message of register
     */
    void handleRegist( Message& msg );



    /*
     * function: distribute all table needed to the all node
     */
    void distributeAllTable();


    /*
     * function: distribute the adjacent table
     */
    void distributeAdjacentTable();

    /*
     * function: distribute the router table
     */
    void distributeRouterTalbe();

    /*

    /*
     * function: handle the SubListUplaod
     * parameters: msg: the message of SubListUpload
     */
    void handleSubListUpload( Message& msg );

    /*
     * function: handle the SubListRequest
     * parameters: msg: the message of SubListRequest
     */
    void handleSubListRequest( Message& msg );

private:
    Topology m_topology;                                 //the topology of whole system
    std::vector< RoleID > m_registedSet;                 //the set of the node registered
    std::vector< string > m_allSubList;                  //the sub list of all publisher
};

#endif /* CONTROLLER_H_ */
