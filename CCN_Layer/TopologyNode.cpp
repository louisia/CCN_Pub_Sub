/*
 * Node.cpp
 *
 *  Created on: Sep 14, 2015
 *      Author: njnu
 */

#include "TopologyNode.h"
#include<arpa/inet.h>

TopologyNode::TopologyNode() {
    // TODO Auto-generated constructor stub

    m_isAssigned = false;
}

TopologyNode::~TopologyNode() {
    // TODO Auto-generated destructor stub
}

RoleID TopologyNode::getRoleID()
{
    return m_roleID;
}

void TopologyNode::setRoleID( RoleID roleID )
{
    m_roleID = roleID;
}

void TopologyNode::addAdjacent( RoleID roleID )
{
    for( std::vector< RoleID >::size_type i = 0; i != m_adjacent.size(); ++i )
    {
        if( m_adjacent[ i ]._role == roleID._role && m_adjacent[ i ]._id == roleID._id )
        {
            std::cerr<<"the role: "<<m_roleID._role<<" id: "<<m_roleID._id<<" "
                    <<"addAdjacent( RoleID roleID ): the role: "<<roleID._role<<" id: "<<roleID._id<<"is already exist..."
                    <<std::endl;
            return ;
        }
    }

    m_adjacent.push_back( roleID );
}

std::vector< RoleID >& TopologyNode::getAdjacent()
{
    return m_adjacent;
}

CommunicateAddr TopologyNode::getCommunicateAddr()
{
    return m_addr;
}

void TopologyNode::setCommunicateAddr( CommunicateAddr comaddr )
{
    m_addr = comaddr;
}

bool TopologyNode::getAssigned()
{
    return m_isAssigned;
}

void TopologyNode::setAssigned( bool isAssigned )
{
    m_isAssigned = isAssigned;
}

std::ostream& operator<<( std::ostream& ofs, const TopologyNode& node )
{
    char* nodeTypes[] = { "ctl", "rot", "sub", "rrot", "pub","erot" };

    std::cout<<"------------Node------------"<<std::endl;
    std::cout<<"Role: "<<nodeTypes[ node.m_roleID._role ]<<" Id: "<<node.m_roleID._id<<std::endl;

    std::cout<<"Adjacent: "<<std::endl;
    for( std::vector< RoleID >::size_type i = 0; i != node.m_adjacent.size(); ++i )
    {
        std::cout<<"Role: "<<node.m_adjacent[ i ]._role<<" Id: "<<node.m_adjacent[ i ]._id<<std::endl;
    }

    std::cout<<"CommunicateAddr:"<<std::endl;

    char tmp[128];
    inet_ntop(AF_INET6,&node.m_addr._ip,tmp,sizeof(tmp));

    std::cout<<"IP: "<<tmp<<std::endl;
    std::cout<<"crPort: "<<node.m_addr._crPort<<std::endl;
    std::cout<<"csPort: "<<node.m_addr._csPort<<std::endl;
    std::cout<<"nrPort: "<<node.m_addr._nrPort<<std::endl;
    std::cout<<"nsPort: "<<node.m_addr._nsPort<<std::endl;

    std::cout<<"----------------------------"<<std::endl;

    return ofs;
}
