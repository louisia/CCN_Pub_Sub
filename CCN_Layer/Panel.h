/*
 * MsgHandler.h
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#ifndef MSGHANDLER_H_
#define MSGHANDLER_H_

#include "Buffer.h"
#include "MsgSender.h"
#include "MsgReceiver.h"

class Panel {
public:
    Panel( std::string localIP);
    virtual ~Panel();

    /*
     * function: send the msg through the interface named faceID
     * parameters: msg: the message to send
     *             faceID: the faceID of the interface to send through
     * return value: the size of the message send
     */
    int sendMsg( Message& msg, FaceID faceID );

    /*
     * function: receive a message
     * return value: the message received
     */
    Message recevMsg();

    /*
     * function: the main logical handle function of the panel
     */
    virtual void running(  );

    /*
     * function: set the transmission face table of the sender
     * parameters: sendTransFaceTable: the transmission face table to set
     */
    void setSendTransFaceTable( std::vector< sockaddr_in6 >& sendTransFaceTable );

    /*
     * function: get the transmission face table of the sender
     * return value: the send transmission face table of current panel
     */
    std::vector< sockaddr_in6 > getSendTransFaceTable();

    /*
     * function: set the transmission face table of the receiver
     * parameters: recevTransFaceTable: the transmission face table fo the receiver
     */
    void setRecevTransFaceTable( std::vector< sockaddr_in6 >& recevTransFaceTable );

    /*
     * function: get the transmission face table of the receiver
     * return value: the receive transmission face table of current panel
     */
    std::vector< sockaddr_in6 > getRecevTransFaceTable();

    /*
     * function: start the running of the panel
     */
    virtual void start();

    /*
     * function: end the running of the panel
     */
    void end();

    /*
     * function: stop the running of the panel
     */
    void stop();

    /*
     * function: start the sender and receiver thread
     */
    void initPanel();

    /*
     * function: get the port of the sender's socket
     * return value: the port number
     */
    int getSenderPort( );

    /*
     * function: set the port of the sender's socket
     * parameters: port: the port to set
     */
    void setSenderPort( int port );

    /*
     * function: set the port of the receiver's socket
     * return value: the port number
     */
    int getReceiverPort();

    /*
     * function: get the port of the receiver's socket
     * parameters: port: the port to set
     */
    void setReceiverPort( int port );

    /*
     * function: get the IP of current panel
     */
    std::string getPanelIP();

    Buffer &getReceiverBuffer(){
    	return m_recevBuffer;
    }

    MsgReceiver &getMsgReceiver(){
    	return m_msgReceiver;
    }

private:
    Buffer m_sendBuffer;                              //the buffer of message to send
    Buffer m_recevBuffer;                             //the buffer of message recevied

    MsgSender m_msgSender;                            //the send thread to send message
    MsgReceiver m_msgReceiver;                        //the receive thread to receive message

};

#endif /* MSGHANDLER_H_ */
