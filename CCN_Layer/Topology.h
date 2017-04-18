/*
 * Topology.h
 *
 *  Created on: Sep 14, 2015
 *      Author: njnu
 */

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include "Structure.h"
#include "TopologyNode.h"

#include <vector>
#include <iostream>

class Topology {
public:
    Topology();
    virtual ~Topology();

    /*
     * function: init the topology from the file
     */
    void initFromFile( );

    /*
     * function: add a link into the topology
     * parameters:rid1: the roleID of the node of the link to add
     *            rid2: the roleID of the another node of the link to add
     */
    void addLink( RoleID rid1, RoleID rid2 );

    /*
     * function: add a node into the topology
     * parameters: roleID: the roleID of the node to add
     * return value: the index of the node added
     */
    int addNode( RoleID roleID );

    /*
     * function: find the named node with the roldID
     * parameters: roleID: the roleID of the node to find
     * return value: return the index of the node if found, otherwise return -1
     */
    int findNode( RoleID roleID );

    /*
     * function: translate the buffer into the roleID
     * parameters: buffer: the information of the roleID
     *             len: the length of the buffer
     * return value: the roleID translated
     */
    RoleID translateToRoleID( char* buffer, int len );

    /*
     * function: get the router table of the node with roleID
     * parameters: roleID: the roleID of the node to get router table
     * return value: the router table of the node with roleID
     */
    std::vector< RouterItem > getRouterTable( RoleID roleID );

    /*
     * function: using the dijkstra to find the multiple shortest path
     * parameters: index: the index of the source node in the nodeSet
     * return value: the pre-node of each destination
     */
    std::vector< std::vector< int > > multiPathDijkstra( int index );

    /*
     * function: translate the pre-node into the next-node table
     * parameters: index: the index of the preTable to translate
     *             preTable: the pre-node table to translate
     *             source: the source of the path
     * return value: the next-node table translate
     */
    std::vector< int > translatePreToNextTable( int index, std::vector< std::vector< int > > preTable, int source );

    /*
     * function: apply the node for the role
     * parameters: comAddr: the communicate information set
     * return value: the id of the role applied
     */
    int applyNode( Role role, CommunicateAddr comAddr );

    /*
     * function: get the number of node in the topology
     * return value: the number of node
     */
    int getNodeNum( );

    /*
     * function: get node by index
     * parameters: index: the index of the node to find
     * return value: the node found
     */
    TopologyNode getNodeByIndex( int index );

    friend std::ostream& operator<<( std::ostream& ofs, Topology& top );

private:
    std::vector< TopologyNode > m_nodeSet;
};

#endif /* TOPOLOGY_H_ */
