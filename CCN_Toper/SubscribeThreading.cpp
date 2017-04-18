/*
 * SubscribeThreading.cpp
 *
 *  Created on: Jun 3, 2016
 *      Author: njnu
 */

#include "SubscribeThreading.h"
#include "Subscriber.h"

SubscribeThreading::SubscribeThreading( Subscriber* pSubscriber ): m_pSubscriber( pSubscriber ){
    // TODO Auto-generated constructor stub

}

SubscribeThreading::~SubscribeThreading() {
    // TODO Auto-generated destructor stub
}

/*
 * function: the main function of the thread
 */
void SubscribeThreading::running()
{
    sleep( 20 );

    while( true )
    {
        m_pSubscriber->subscribe();

        sleep( 5 );
    }
}
