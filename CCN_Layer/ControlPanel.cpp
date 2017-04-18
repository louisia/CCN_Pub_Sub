/*
 * ControlPanel.cpp
 *
 *  Created on: Sep 20, 2015
 *      Author: njnu
 */

#include "ControlPanel.h"

#include "SubListUploader.h"
#include "SubListDownloader.h"

#include <arpa/inet.h>
#include <string.h>
#include <sstream>

ControlPanel::ControlPanel( DataPanel* pDataPanel, std::string localIP, std::string controllerIP ): Panel( localIP ) {
    // TODO Auto-generated constructor stub

    m_pDataPanel = pDataPanel;

    m_subListloader = NULL;

    initControlPanel( controllerIP );
}

ControlPanel::~ControlPanel( ) {
    // TODO Auto-generated destructor stub

    delete m_subListloader;
    m_subListloader = NULL;
}


/*
 * function: inject the transmission face table into the data panel
 */
void ControlPanel::injectTransFaceTableToDataPanel()
{
    //generate the send transmission face table and the receive transmission face table
    std::vector< sockaddr_in6 > sendTransFaceTable, recevTransFaceTable;
    sockaddr_in6 addr;
    CommunicateAddr comAddr;
    memset( &addr, 0, sizeof( sockaddr_in6 ) );

    addr.sin6_family = AF_INET6;

    for( std::vector< AdjacentItem >::size_type i = 0; i != m_adjacentTable.size(); ++i )
    {
        comAddr = m_adjacentTable[ i ]._comAddr;

        addr.sin6_addr = comAddr._ip;
        addr.sin6_port = htons( comAddr._nrPort );//the recv port of the adjacent node

        sendTransFaceTable.push_back( addr );    //put into self send port vector

        addr.sin6_port = htons( comAddr._nsPort );//the send port of the adjacent node

        recevTransFaceTable.push_back( addr );//put into self recv port vector
    }

    m_pDataPanel->setSendTransFaceTable( sendTransFaceTable );
    m_pDataPanel->setRecevTransFaceTable( recevTransFaceTable );
}

/*
 * function: inject the FIB to the data panel
 */
void ControlPanel::injectFIBToDataPanel()
{
    //generate the FIB from the file distribution and router table and the adjacent table
    std::vector< FIBItem > fib;

    //creat the fib information of the rendezvous node according the router table and the adjacent table
    for( vector< RouterItem >::size_type i = 0; i != m_routerTable.size(); ++i )
    {
        if( m_routerTable[ i ]._des._role != RRot )
            continue;

        //create the fib item for the RRot type node
        FIBItem item;

        string id;
        stringstream ss;
        ss<<m_routerTable[ i ]._des._id;
        ss>>id;

        item._contentName = "RRot-" + id;

        for( vector< RoleID >::size_type j = 0; j != m_routerTable[ i ]._nextSet.size(); ++j )
        {
            int faceID = 0;
            while( faceID < m_adjacentTable.size() )
            {
                if( m_routerTable[ i ]._nextSet[ j ]._role != m_adjacentTable[ faceID ]._roleID._role ||
                    m_routerTable[ i ]._nextSet[ j ]._id != m_adjacentTable[ faceID ]._roleID._id )
                {
                    faceID++;
                    continue ;
                }

                item._faceList.push_back( faceID++ );
            }
        }

        fib.push_back( item );
    }

    //sort the fib to facility the search action
    sort( fib.begin(), fib.end() );

    //set the fib of the data panel
    m_pDataPanel->setFIB( fib );
}


/*
 * function: regist to the controller
 * parameters: role: the role to register
 * return value: the id of the role applied
 */
int ControlPanel::regist( Role role )
{
    //send regist message to the controller
    Message msg;

    PacketType type = Regist;
    //Role role = role;


    in6_addr addr ;
	inet_pton(AF_INET6,m_pDataPanel->getPanelIP().c_str(),&addr);

    Port csPort = getSenderPort();
    Port crPort = getReceiverPort();

    Port nsPort = m_pDataPanel->getSenderPort();

    Port nrPort = m_pDataPanel->getReceiverPort();


    /*
     * the format of the regist message
     * PacketType | Role | IP | CSPort | CRPort | NSPort | NRPort
     */
    msg._bufferLength = sizeof( PacketType ) + sizeof( Role ) + sizeof( in6_addr ) + sizeof( Port ) * 4 + 10;
    msg._buffer = new char[ msg._bufferLength ];

    int pos = 0;

    memcpy( msg._buffer + pos, &type, sizeof( PacketType ) );
    pos += sizeof( PacketType );

    memcpy( msg._buffer + pos, &role, sizeof( Role ) );
    pos += sizeof( Role );

    memcpy( msg._buffer + pos, &addr, sizeof( in6_addr) );
    pos += sizeof( in6_addr);

    memcpy( msg._buffer + pos, &csPort, sizeof( Port ) );
    pos += sizeof( Port );

    memcpy( msg._buffer + pos, &crPort, sizeof( Port ) );
    pos += sizeof( Port );

    memcpy( msg._buffer + pos, &nsPort, sizeof( Port ) );
    pos += sizeof( Port );

    memcpy( msg._buffer + pos, &nrPort, sizeof( Port ) );
    pos += sizeof( Port );

    msg._msgLength = pos;

    msg._faceID = 0;
    msg._type=UDP;

    std::cout<<"***********ControlPanel::regist() start***********"<<std::endl;

    printMsg( msg._buffer, msg._msgLength );

    sendMsg( msg, msg._faceID );
    sleep( 5 );
    std::cout<<"***********ControlPanel::regist() end***************"<<std::endl;

    int id = 0;

    return id;
}

/*
 * function: init the control panel
 * parameters: controllerIP : the ip of the controller
 */
void ControlPanel::initControlPanel( std::string controllerIP )
{
    //init the controller address
    std::vector< sockaddr_in6 > sendTransFaceTable, recevTransFaceTable;

    sockaddr_in6 controllerAddr;
    memset( &controllerAddr, 0, sizeof( sockaddr_in6 ) );
    controllerAddr.sin6_family = AF_INET6;
    inet_pton(AF_INET6,controllerIP.c_str(),&controllerAddr.sin6_addr);
    controllerAddr.sin6_port = htons( RECEVPORT );
    //add the sender address
    sendTransFaceTable.push_back( controllerAddr );

    controllerAddr.sin6_port = htons( SENDPORT );
    //add the receiver address
    recevTransFaceTable.push_back( controllerAddr );

    setSendTransFaceTable( sendTransFaceTable );

    setRecevTransFaceTable( recevTransFaceTable );

}


/*
 * function: translate the message to adjacent node table
 * parameters: msg: the message to translate
 */
void ControlPanel::handleAdjacentTable( Message& msg )
{
    /*
     * the format of message of adjacent table
     * PacketType | CurrentRole | CurrentID | NumOfAdjacent | AdjacentSet
     *
     * the format of adjacent set
     * Role | ID | CommunicateAddr, the order mean the index of interface linked
     *
     * the format of CommunicateAddr
     * IP | CSPort | CRPort | NSPort | NRPort
     */

    RoleID roleID;
    int numOfNode = 0;
    int numOfAdjacent = -1;
    CommunicateAddr comAddr;
    AdjacentItem item;

    int pos = sizeof( PacketType );

    //read the head of the message
    memcpy( &roleID._role, msg._buffer + pos, sizeof( Role ) );
    pos += sizeof( Role );

    memcpy( &roleID._id, msg._buffer + pos, sizeof( int ) );
    pos += sizeof( int );


    memcpy( &numOfAdjacent, msg._buffer + pos, sizeof( int ) );
    pos += sizeof( int );

    //write the adjacent information
    for( int j = 0; j < numOfAdjacent; ++j )
    {
        memcpy( &roleID._role, msg._buffer + pos, sizeof( Role ) );
        pos += sizeof( Role );

        memcpy( &roleID._id, msg._buffer + pos, sizeof( int ) );
        pos += sizeof( int );

        //read the ip information
        memcpy( &comAddr, msg._buffer + pos, sizeof( CommunicateAddr ) );
        pos += sizeof( CommunicateAddr );

        item._roleID = roleID;
        item._comAddr = comAddr;

        m_adjacentTable.push_back( item );
    }

}

/*
 * function: translate the message to router table
 * parameters: msg: the message to translate
 */
void ControlPanel::handleRouterTable( Message& msg )
{
    /*
     * the format of the message of router table
     * PacketType | NumOfItem | RouterItem
     *
     * the format of RouterItem
     * DestinationRoleID | NumberOfNextHop | NextHopSet
     *
     * the format of NextHopSet
     * Role | ID
     */
    int numOfNextHop = 0;
    int numOfItem = 0;
    RoleID roleID;

    int pos = sizeof( PacketType );

    memcpy( &numOfItem, msg._buffer + pos, sizeof( int ) );
    pos += sizeof( int );

    for( int j = 0; j < numOfItem; ++j )
    {
        RouterItem item ;

        memcpy( &roleID, msg._buffer + pos, sizeof( RoleID ) );
        pos += sizeof( RoleID );

        item._des = roleID;

        memcpy( &numOfNextHop, msg._buffer + pos, sizeof( int ) );
        pos += sizeof( int );

        for( int k = 0; k < numOfNextHop; ++k )
        {
            memcpy( &roleID, msg._buffer + pos, sizeof( RoleID ) );
            pos += sizeof( RoleID );

            item._nextSet.push_back( roleID );
        }

        m_routerTable.push_back( item );
    }

    sleep( 1 );
}


/*
 * function: set the role of current node
 */
void ControlPanel::setRole( Role role )
{
    m_roleID._role = role;
}


/*
 * function: start the sub list loader according the role type of current node
 */
void ControlPanel::startSubListLoader()
{
    if( m_roleID._role == Sub )
    {
        SubListDownloader* p = new SubListDownloader();
        p->setControlPanel( this );
        p->setDataPanel( m_pDataPanel );

        m_subListloader = new Checker( p );
    }
    else if( m_roleID._role == Pub )
    {
        SubListUploader* p = new SubListUploader();
        p->setControlPanel( this );
        p->setDataPanel( m_pDataPanel );

        m_subListloader = new Checker( p );
    }

    if( m_subListloader != NULL )
    {
        m_subListloader->create( m_subListloader );
    }
}

/*
 * function: the main logical handle function of the control panel
 */
void ControlPanel::running()
{
    //start the sender and receiver
    start();

    regist( m_roleID._role );

    Message msg = recevMsg();

    int id = -1;

    memcpy( (char*)&id, msg._buffer + sizeof( IDAssigned ), msg._msgLength );

    std::cout<<" the id assigned: "<<id<<std::endl;
    printMsg(msg._buffer,msg._bufferLength);

    //set the role of the data panel
    m_roleID._id = id;
    m_pDataPanel->setRoleID( m_roleID );


    msg = recevMsg();

    sleep( 1 );
    std::cout<<" ControlPanel::running receive adjacent table...: "<<id<<std::endl;
    printMsg( msg._buffer, msg._msgLength );

    sleep( 1 );
    handleAdjacentTable( msg );

    msg = recevMsg();

    sleep( 1 );
    std::cout<<"ControlPanel::running receive router table...: "<<id<<std::endl;
    printMsg( msg._buffer, msg._msgLength );

    sleep( 1 );
    handleRouterTable( msg );

    std::cout<<"ControlPanel::running injectTransFaceTableToDataPanel...: "<<id<<std::endl;
    injectTransFaceTableToDataPanel();

    std::cout<<"ControlPanel::running injectFIBToDataPanel...: "<<id<<std::endl;
    injectFIBToDataPanel();


    //start the sub list loader according the role type
    startSubListLoader();

    //test
    m_pDataPanel->start();
    m_pDataPanel->create( m_pDataPanel );

    while( true )
    {
        sleep( 10 );

        m_pDataPanel->pause();
        m_pDataPanel->resume();
    }
}
