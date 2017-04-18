/*
 * Node.h
 *
 *  Created on: Sep 20, 2015
 *      Author: njnu
 */

#ifndef NODE_H_
#define NODE_H_

#include "ControlPanel.h"
#include "DataPanel.h"

class Node {
public:
    Node( Role role );
    virtual ~Node();

    /*
     * function: the main logical handle of current node
     */
    void running();

private:
    ControlPanel m_controlPanel;                             //the control panel of the current node
    DataPanel m_dataPanel;                                   //the data panel of the current node

    Role m_role;                                             //the role of current node
};

#endif /* NODE_H_ */
