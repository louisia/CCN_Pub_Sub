/*
 * SubscribeThreading.h
 *
 *  Created on: Jun 3, 2016
 *      Author: njnu
 */

#ifndef SUBSCRIBETHREADING_H_
#define SUBSCRIBETHREADING_H_

#include "../CCN_Layer/Threading.h"

class Subscriber;

class SubscribeThreading: public Threading {
public:
    SubscribeThreading( Subscriber* pSubscriber );
    virtual ~SubscribeThreading();

protected:
    /*
     * function: the main function of the thread
     */
    virtual void running();

private:
    Subscriber* m_pSubscriber;              //the pointer of the subscriber
};

#endif /* SUBSCRIBETHREADING_H_ */
