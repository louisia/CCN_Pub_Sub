/*
 * MsgHandler.cpp
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#include "Panel.h"

#include <iostream>
#include <string.h>

Panel::Panel( std::string ip4 ): m_msgSender( m_sendBuffer, ip4 ), m_msgReceiver( m_recevBuffer, ip4 ) {
    // TODO Auto-generated constructor stub

    initPanel();
}

Panel::~Panel() {
    // TODO Auto-generated destructor stub
}

void Panel::initPanel()
{

}

/*
 * function: send the msg through the interface named faceID
 * parameters: msg: the message to send
 *             faceID: the faceID of the interface to send through
 * return value: the size of the message send
 */
int Panel::sendMsg( Message& msg, FaceID faceID )
{
    //check the faceID to send through
    if( -1 == faceID )
        faceID = msg._faceID;

    if( faceID != msg._faceID )
    {
        std::cerr<<"Panel::sendMsg: faceID != msg._sendFaceID--"<<faceID<<" -> "<<msg._faceID<<std::endl;

        return -1;
    }

    int result = msg._msgLength;

    //put the message to send into the buffer of sender
    m_sendBuffer.putMsg( msg );

    return result;
}

/*
 * function: receive a message
 * return value: the message received
 */
Message Panel::recevMsg()
{
    return m_recevBuffer.getNextMsg();
}

/*
 * function: set the transmission face table of the sender
 * parameters: sendTransFaceTable: the transmission face table to set
 */
void Panel::setSendTransFaceTable( std::vector< sockaddr_in6 >& sendTransFaceTable )
{
    m_msgSender.setTransFaceTable( sendTransFaceTable );
}

/*
 * function: get the transmission face table of the sender
 * return value: the send transmission face table of current panel
 */
std::vector< sockaddr_in6 > Panel::getSendTransFaceTable()
{
    return m_msgSender.getTransFaceTable();
}

/*
 * function: set the transmission face table of the receiver
 * parameters: recevTransFaceTable: the transmission face table fo the receiver
 */
void Panel::setRecevTransFaceTable( std::vector< sockaddr_in6 >& recevTransFaceTable )
{
    m_msgReceiver.setTransFaceTable( recevTransFaceTable );
}

/*
 * function: get the transmission face table of the receiver
 * return value: the receive transmission face table of current panel
 */
std::vector< sockaddr_in6 > Panel::getRecevTransFaceTable()
{
    return m_msgReceiver.getTransFaceTable();
}

/*
 * function: start the running of the panel
 */
void Panel::start()
{
    m_msgSender.create( &m_msgSender );

    m_msgReceiver.create( &m_msgReceiver );

}

/*
 * function: end the running of the panel
 */
void Panel::end()
{

}

/*
 * function: stop the running of the panel
 */
void Panel::stop()
{

}

/*
 * function: get the port of the sender's socket
 * return value: the port number
 */
int Panel::getSenderPort( )
{
    return m_msgSender.getPort();
}

/*
 * function: set the port of the sender's socket
 * parameters: port: the port to set
 */
void Panel::setSenderPort( int port )
{
    m_msgSender.setPort( port );
}

/*
 * function: set the port of the receiver's socket
 * return value: the port number
 */
int Panel::getReceiverPort()
{
    return m_msgReceiver.getPort();
}

/*
 * function: get the port of the receiver's socket
 * parameters: port: the port to set
 */
void Panel::setReceiverPort( int port )
{
    m_msgReceiver.setPort( port );
}

/*
 * function: get the IP of current panel
 */
std::string Panel::getPanelIP()
{
    std::string senderIP = m_msgSender.getLacalIP();
    std::string receiverIP = m_msgReceiver.getLocalIP();

    if( senderIP != receiverIP )
    {
        std::cerr<<"the ip of sender and receiver is different..."<<std::endl;
    }

    return receiverIP;
}

/*
 * function: the main logical handle function of the panel
 */
void Panel::running()
{

}
