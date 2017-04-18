/*
 * Slicer.cpp
 *
 *  Created on: Sep 5, 2015
 *      Author: njnu
 */

#include "Slicer.h"
#include <openssl/md5.h>
#include <string.h>

Slicer::Slicer() {
    // TODO Auto-generated constructor stub

    initSlicer();
}

Slicer::~Slicer() {
    // TODO Auto-generated destructor stub

    resetSlicer();
}

/*
 * function: init the parameter of the slicer
 */
void Slicer::initSlicer()
{
    m_buffer = 0;                                              //the buffer the current message
    m_bufferSize = 0;                                          //the size of the current buffer

    m_sliceSize = SLICER_SIZE;                                 //the size of each slice

    memset( &m_packetID, 0, sizeof( m_packetID ) );            //the packetID of current message
    m_sliceID = 0;                                             //the sliceID of the current slice
}

/*
 * function: reset the parameter of the slicer
 */
void Slicer::resetSlicer()
{
    if( 0 != m_buffer )
    {
        delete m_buffer;
        m_buffer =0;
        m_bufferSize = 0;

        memset( &m_packetID, 0, sizeof( m_packetID ) );
        m_sliceID = 0;
    }
}

/*
 * function: set the current buffer
 */
void Slicer::setBuffer( char* buffer, int size )
{
    resetSlicer();

    //set the buffer and its size
    m_buffer = new char[ size ];
    memcpy( m_buffer, buffer, size );
    m_bufferSize = size;

    //set the packetID
    MD5( (unsigned char*)m_buffer, m_bufferSize, (unsigned char*)&m_packetID );
}

/*
 * function: adjuge whether current message is sliced finished
 * return value: return true if the current message is sliced finished, otherwise return false
 */
bool Slicer::isSliceFinished()
{
    return m_sliceID * m_sliceSize >= m_bufferSize ? true : false;
}

/*
 * function: get the next slice of the current message
 * return value: return the next slice of the current message
 */
Slice Slicer::getNextSlice()
{
    Slice slice;

    slice._pid = m_packetID;
    slice._sid = ++m_sliceID;

    int pos = ( m_sliceID - 1 ) * m_sliceSize;

    slice._sliceData = m_buffer + pos;

    if( isSliceFinished() )
    {
        slice._sliceSize = m_bufferSize - pos;
        slice._isLast = true;
    }
    else
    {
        slice._sliceSize = m_sliceSize;
        slice._isLast = false;
    }

    return slice;
}
