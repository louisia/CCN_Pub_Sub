/*
 * PublishThreading.cpp
 *
 *  Created on: Jun 3, 2016
 *      Author: njnu
 */

#include "PublishThreading.h"
#include "Publisher.h"

PublishThreading::PublishThreading( Publisher* pPublisher ) : m_pPublisher( pPublisher ){
    // TODO Auto-generated constructor stub
}

PublishThreading::~PublishThreading() {
    // TODO Auto-generated destructor stub
}

/*
 * function: the main function of the thread
 */
void PublishThreading::running()
{
    sleep( 20 );

    while( true )
    {
        m_pPublisher->publish();

        sleep( 5 );
    }
}
