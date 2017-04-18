/*
 * Buffer.h
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <pthread.h>
#include <semaphore.h>

#include "Structure.h"

#define MSG_SIZE 2048

class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    /*
     * function: get the next message available
     * return value: the message available
     */
    Message getNextMsg( );
    /*
     * function: put the message into the message set
     * input values: the message to put into the message set
     * return values: return true if put the msg into the message set success, otherwise false
     */
    bool putMsg( Message& msg );

private:
    /*
     * function: initialize the parameters of the message set
     */
    void initBuffer();

private:
    int m_readPos;                        //the pos can be read
    int m_writePos;                       //the pos can be write

    Message m_msg[ MSG_SIZE ];            //the space of message

    sem_t m_readSem;                      //the sem of read
    sem_t m_writeSem;                     //the sem of write
    pthread_mutex_t m_msgMutex;           //the mutex of the message set
};

#endif /* BUFFER_H_ */
