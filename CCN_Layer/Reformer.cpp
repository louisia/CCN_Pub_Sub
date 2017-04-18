/*
 * Reformer.cpp
 *
 *  Created on: Sep 5, 2015
 *      Author: njnu
 */

#include "Reformer.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>

Reformer::Reformer( Buffer& buffer ): m_msgBuffer( buffer ), m_checker( this ) {
    // TODO Auto-generated constructor stub

    for( int i = 0; i < SET_SIZE; ++i )
    {
        memset( &m_sliceBufferSet[ i ]._id, 0, sizeof( PacketID ));
        m_sliceBufferSet[ i ]._isUsed = false;
    }

    //init the mutex
    pthread_mutex_init( &m_bufferMutex, NULL );

    //start the checker threading
    m_checker.create( &m_checker );
}

Reformer::~Reformer() {
    // TODO Auto-generated destructor stub
}

/*
 * function: find the buffer index of slice according the packetId of the slice
 * return value: the index of the buffer
 */
int Reformer::findSliceBuffer( PacketID id )
{
    for( int i = 0; i < SET_SIZE; ++i )
    {
        if( true != m_sliceBufferSet[ i ]._isUsed )
            continue;

        if( 0 == memcmp( id._id, m_sliceBufferSet[ i ]._id._id, sizeof( PacketID ) ) )
        {
            return i;
        }
    }

    return -1;
}

/*
 * function: find the empty space of the slice buffer
 * return value: int index of the buffer
 */
int Reformer::findEmptySliceBuffer()
{
    for( int i = 0; i < SET_SIZE; ++i )
    {
        if( true != m_sliceBufferSet[ i ]._isUsed )
        {
            //unlock the mutex
            //pthread_mutex_unlock( &m_bufferMutex );

            return i;
        }
    }

    return -1;
}

/*
 * function: adjudge the slice of the special packetid whether complete or not
 * return value: return true if the slice buffer complete, otherwise false
 */
bool Reformer::isSliceBufferCompeleted( PacketID id )
{
    int index = findSliceBuffer( id );

    if( -1 == index )
        return true;

    return m_sliceBufferSet[ index ]._isCompleted;
}

/*
 * function: put the slice into the slice buffer
 * return value: return true if success, otherwise false
 */
bool Reformer::putSlice( Slice& slice, FaceID faceID )
{


    //lock the buffer
    pthread_mutex_lock( &m_bufferMutex );


    int index = findSliceBuffer( slice._pid );

    if( -1 == index )
    {
        //find the empty slice buffer for the first slice
        if( slice._sid != 1 )
        {
            sleep( 1 );

            std::cerr<<"Reformer::putSlice: error sid received..."<<slice._sid<<std::endl;

            //unlock the mutex
            pthread_mutex_unlock( &m_bufferMutex );


            return false;
        }

        index = findEmptySliceBuffer();


        if( -1 == index )
        {
            std::cerr<<"not found the empty space to store the slice..."<<std::endl;

            //unlock the mutex
            pthread_mutex_unlock( &m_bufferMutex );


            return false;
        }

        //create the new slice buffer
        m_sliceBufferSet[ index ]._id = slice._pid;
        m_sliceBufferSet[ index ]._isCompleted = false;
        m_sliceBufferSet[ index ]._isUsed = true;
        m_sliceBufferSet[ index ]._recevFaceID = faceID;
        m_sliceBufferSet[ index ]._time = getCurrentTime();
    }

    if( faceID != m_sliceBufferSet[ index ]._recevFaceID )
    {
        std::cerr<<"the faceID of the slice received is error..."<<faceID<<m_sliceBufferSet[ index ]._recevFaceID<<std::endl;

        //return false;
    }

    //create the space and copy the slice
    Slice* p = new Slice();
    memcpy( p, &slice, sizeof(slice) );

    p->_sliceData = new char[ slice._sliceSize ];
    memcpy( p->_sliceData, slice._sliceData, slice._sliceSize );

    //update the time
    m_sliceBufferSet[ index ]._time = getCurrentTime();

    //here we can decide the method how to handle the unordered slice
    if( slice._sid - 1 == m_sliceBufferSet[ index ]._sliceSet.size() )
    {
        m_sliceBufferSet[ index ]._sliceSet.push_back( p );
    }
    else
    {

        std::cerr<<"error sid: "<<slice._sid<<" "<<m_sliceBufferSet[ index ]._sliceSet.size()<<std::endl;

        //unlock the mutex
        pthread_mutex_unlock( &m_bufferMutex );


        return false;
    }

    if( slice._isLast )
    {
        m_sliceBufferSet[ index ]._isCompleted = true;

        if( reassembleSlice( index ) )
        {
            removeSlice( index );
        }
    }

    //unlock the mutex
    pthread_mutex_unlock( &m_bufferMutex );


    return true;
}

/*
 * function: remove the special slice
 * return value: return true if success, otherwise false
 */
bool Reformer::removeSlice( int index )
{
    if( index < 0 || index > SET_SIZE  )
    {
        return false;
    }

    memset( &m_sliceBufferSet[ index ]._id, 0, sizeof( m_sliceBufferSet[ index ]._id ) );
    m_sliceBufferSet[ index ]._isCompleted = false;
    m_sliceBufferSet[ index ]._isUsed = false;
    m_sliceBufferSet[ index ]._recevFaceID = -1;
    m_sliceBufferSet[ index ]._time = -1;

    std::vector< Slice* >* p = &m_sliceBufferSet[ index ]._sliceSet;
    for( int i = 0; i < p->size(); ++i )
    {
        //here need to delete the all memory that apply
        delete[] p->at( i )->_sliceData;
        delete p->at( i );

        p->at( i ) = 0;
    }

    p->resize( 0 );

    return true;
}


/*
 * function: reassemble the slice from the slice buffer
 * return value: return true if success, otherwise false
 */
bool Reformer::reassembleSlice( int index )
{
    if( index < 0 || index > SET_SIZE  )
    {
        return false;
    }

    SliceBuffer* p = &m_sliceBufferSet[ index ];

    if( 0 == p->_sliceSet.size() )
    {
        std::cerr<<"size == 0 error ..."<<std::endl;
        return false;
    }

    Message msg;

    int bufferSize = p->_sliceSet.at( 0 )->_sliceSize * p->_sliceSet.size();

    msg._buffer = new char[ bufferSize ];

    msg._bufferLength = bufferSize;
    msg._msgLength = 0;

    msg._faceID = p->_recevFaceID;


    int pos = 0;
    for( int i = 0; i < p->_sliceSet.size(); ++i )
    {
        memcpy( (char*)msg._buffer + pos, p->_sliceSet[ i ]->_sliceData , p->_sliceSet[ i ]->_sliceSize );//here we need to pay the address of the msg

        pos += (p->_sliceSet[ i ]->_sliceSize);
    }
    msg._msgLength = pos;

    memset( msg._buffer + msg._msgLength, 0, msg._bufferLength - msg._msgLength );


    //put the msg into the message buffer
    m_msgBuffer.putMsg( msg );

    return true;
}

/*
 * function: the implememtation of the interface of the checker
 */
void Reformer::check()
{
    //lock the mutex
    pthread_mutex_lock( &m_bufferMutex );

    Time_t curTime = getCurrentTime();


    for( int i = 0; i < SET_SIZE; ++i )
    {
        if( true != m_sliceBufferSet[ i ]._isUsed )
            continue;



        if( curTime - m_sliceBufferSet[ i ]._time > REFORMER_TIME_OUT )
        {
            //remove the item which over time
            removeSlice( i );
        }
    }

    //unlock the mutex
    pthread_mutex_unlock( &m_bufferMutex );
}
