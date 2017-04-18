/*
 * PublishThreading.h
 *
 *  Created on: Jun 3, 2016
 *      Author: njnu
 */

#ifndef PUBLISHTHREADING_H_
#define PUBLISHTHREADING_H_

#include "../CCN_Layer/Threading.h"

class Publisher;

class PublishThreading: public Threading {
public:
    PublishThreading( Publisher* pPublisher );
    virtual ~PublishThreading();

protected:
    /*
     * function: the main function of the thread
     */
    virtual void running();

private:
    Publisher* m_pPublisher;                   //the pointer of the publisher
};

#endif /* PUBLISHTHREADING_H_ */
