/*
 * Node.cpp
 *
 *  Created on: Sep 20, 2015
 *      Author: njnu
 */

#include "Node.h"

Node::Node( Role role ): m_controlPanel(0, "", ""), m_dataPanel( "" ) {
    // TODO Auto-generated constructor stub

    m_role = role;
}

Node::~Node() {
    // TODO Auto-generated destructor stub
}

/*
 * function: the main logical handle of current node
 */
void Node::running()
{
    m_controlPanel.regist( m_role );
}
