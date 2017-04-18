/*
 * Threading.cpp
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#include "Threading.h"

#include <iostream>

Threading::Threading() {
    // TODO Auto-generated constructor stub

    //init the conditional variable and the mutex variable of the status
    pthread_cond_init( &m_cond, NULL );
    pthread_mutex_init( &m_mutex, NULL );

    m_status = Run;
}

Threading::~Threading() {
    // TODO Auto-generated destructor stub
}

/*
 * function: create the thread
 */
void Threading::create( Threading* pThread )
{
    //create the thread
    pthread_t id = 0;
    pthread_create( &id , NULL, &threadfunction, pThread );


}

/*
 * function: the thread function
 */
void* Threading::threadfunction( void* arg )
{
    ( (Threading*)arg)->running();
}

/*
 * function: pause the thread
 */
void Threading::pause()
{
    pthread_mutex_lock( &m_mutex );

    m_status = Pause;

    pthread_mutex_unlock( &m_mutex );
}

/*
 * function: resume the thread
 */
void Threading::resume()
{
    pthread_mutex_lock( &m_mutex );

    if( m_status == Pause )
    {
        m_status = Run;
        pthread_cond_signal( &m_cond );
    }

    pthread_mutex_unlock( &m_mutex );
}

/*
 * function: check the status of the thread
 * return value: return true if the status of the thread is run, otherwise return false
 */
bool Threading::isRunning()
{
    pthread_mutex_lock( &m_mutex );

    while( m_status != Run )
    {
        pthread_cond_wait( &m_cond, &m_mutex );
    }

    pthread_mutex_unlock( &m_mutex );

    return true;
}
