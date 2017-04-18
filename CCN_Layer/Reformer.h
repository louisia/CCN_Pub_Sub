/*
 * Reformer.h
 *
 *  Created on: Sep 5, 2015
 *      Author: njnu
 */

#ifndef REFORMER_H_
#define REFORMER_H_

#include "Structure.h"
#include "Buffer.h"
#include "CheckerImp.h"
#include "Checker.h"

#define SET_SIZE 10

class Reformer : public CheckerImp {
public:
    Reformer( Buffer& buffer );
    virtual ~Reformer();

    /*
     * function: put the slice into the slice buffer
     * return value: return true if success, otherwise false
     */
    bool putSlice( Slice& slice, FaceID faceID );

    /*
     * function: the implememtation of the interface of the checker
     */
    virtual void check();

private:
    /*
     * function: find the buffer index of slice according the packetId of the slice
     * return value: the index of the buffer
     */
    int findSliceBuffer( PacketID id );

    /*
     * function: find the empty space of the slice buffer
     * return value: int index of the buffer
     */
    int findEmptySliceBuffer();

    /*
     * function: adjudge the slice of the special packetid whether complete or not
     * return value: return true if the slice buffer complete, otherwise false
     */
    bool isSliceBufferCompeleted( PacketID id );


    /*
     * function: remove the special slice
     * return value: return true if success, otherwise false
     */
    bool removeSlice( int index );

    /*
     * function: reassemble the slice from the slice buffer
     * return value: return true if success, otherwise false
     */
    bool reassembleSlice( int index );



private:
    pthread_mutex_t m_bufferMutex;                             //the mutex of the slice buffer set
    SliceBuffer m_sliceBufferSet[ SET_SIZE ];                  //the buffer set of the slice
    Buffer& m_msgBuffer;                                       //the msg buffer to receive the msg which is composed of slice

    Checker m_checker;                                         //the checker to check the time over
};

#endif /* REFORMER_H_ */
