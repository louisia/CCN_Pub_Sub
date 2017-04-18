/*
 * Node.h
 *
 *  Created on: Sep 14, 2015
 *      Author: njnu
 */

#ifndef NODE_H_
#define NODE_H_

#include "Structure.h"

#include <vector>
#include <string>
#include <iostream>

class TopologyNode {
public:
    TopologyNode();
    virtual ~TopologyNode();

    RoleID getRoleID();
    void setRoleID( RoleID roleID );

    void addAdjacent( RoleID roleID );
    std::vector< RoleID >& getAdjacent();

    CommunicateAddr getCommunicateAddr();
    void setCommunicateAddr( CommunicateAddr comaddr );

    bool getAssigned();
    void setAssigned( bool isAssigned );

    friend std::ostream& operator<<( std::ostream& ofs, const TopologyNode& node );

private:
    RoleID m_roleID;                                //the roleID of current node
    std::vector< RoleID > m_adjacent;               //the set of the adjacent of the current node

    CommunicateAddr m_addr;                         //the communicate address of the current node

    bool m_isAssigned;                              //mark the current node whether assigned or not
};

#endif /* NODE_H_ */
