/*
 * Slicer.h
 *
 *  Created on: Sep 5, 2015
 *      Author: njnu
 */

#ifndef SLICER_H_
#define SLICER_H_

#include "Structure.h"


class Slicer {
public:
    Slicer();
    virtual ~Slicer();

    /*
     * function: set the current buffer
     */
    void setBuffer( char* buffer, int size );

    /*
     * function: adjuge whether current message is sliced finished
     * return value: return true if the current message is sliced finished, otherwise return false
     */
    bool isSliceFinished();

    /*
     * function: get the next slice of the current message
     * return value: return the next slice of the current message
     */
    Slice getNextSlice();

private:
    /*
     * function: init the parameter of the slicer
     */
    void initSlicer();

    /*
     * function: reset the parameter of the slicer
     */
    void resetSlicer();

private:
    char* m_buffer;                //the buffer the current message
    int m_bufferSize;              //the size of the current buffer

    int m_sliceSize;               //the size of each slice

    PacketID m_packetID;           //the packetID of current message
    SliceID m_sliceID;             //the sliceID of the current slice
};

#endif /* SLICER_H_ */
