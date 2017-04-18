/*
 * Buffer.cpp
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#include "Buffer.h"

#include <iostream>
#include <stdlib.h>

Buffer::Buffer() {
    initBuffer();

}

Buffer::~Buffer() {
    //delete the buffer of message
    for( int i = 0; i < MSG_SIZE; ++i )
    {
        if( 0 != m_msg[ i ]._buffer )
        {
            delete m_msg[ i ]._buffer;
            m_msg[ i ]._buffer = 0;
        }
    }
}

/*
 * function: get the next message available
 * return value: the message available
 */
Message Buffer::getNextMsg( )
{
    Message msg;

    sem_wait( &m_readSem );

    pthread_mutex_lock( &m_msgMutex );

    m_readPos = ( m_readPos + 1 ) % MSG_SIZE;

    msg = m_msg[ m_readPos ];

    m_msg[ m_readPos ]._buffer = 0;
    m_msg[ m_readPos ]._bufferLength = 0;
    m_msg[ m_readPos ]._msgLength = -1;
    m_msg[ m_readPos ]._faceID = -1;


    sem_post( &m_writeSem );

    pthread_mutex_unlock( &m_msgMutex );

    return msg;
}

/*
 * function: put the message into the message set
 * input values: the message to put into the message set
 * return values: return true if put the msg into the message set success, otherwise false
 */
bool Buffer::putMsg( Message& msg )
{

    sem_wait( &m_writeSem );

    pthread_mutex_lock( &m_msgMutex );

    if( 0 != m_msg[ m_writePos ]._buffer )
    {
        delete m_msg[ m_writePos ]._buffer;
    }

    m_msg[ m_writePos ] = msg;
    m_writePos = ( m_writePos + 1 ) % MSG_SIZE;

    msg._buffer = 0;
    msg._bufferLength = 0;
    msg._msgLength = -1;
    msg._faceID = -1;


    sem_post( &m_readSem );

    pthread_mutex_unlock( &m_msgMutex );


    return true;
}

/*
 * function: initialize the parameters of the message set
 */
void Buffer::initBuffer()
{
    //init the mutex
    pthread_mutex_init( &m_msgMutex, NULL );

    //init the sem
    sem_init( &m_writeSem, 0, MSG_SIZE-1 );
    sem_init( &m_readSem, 0, 0 );

    //init the readPos and the writePos
    m_readPos = 0;
    m_writePos = 1;

    //init the buffer of message
    for( int i = 0; i < MSG_SIZE; ++i )
    {
        m_msg[ i ]._buffer = 0;
        m_msg[ i ]._bufferLength = 0;
        m_msg[ i ]._msgLength = -1;
        m_msg[ i ]._faceID = -1;
    }
}
