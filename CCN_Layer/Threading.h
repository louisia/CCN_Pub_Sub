/*
 * Threading.h
 *
 *  Created on: Sep 4, 2015
 *      Author: njnu
 */

#ifndef THREADING_H_
#define THREADING_H_

#include <pthread.h>

class Threading {
public:
    Threading();
    virtual ~Threading();

    /*
     * function: create the thread
     */
    void create( Threading* pThread );

    /*
     * function: pause the thread
     */
    void pause();

    /*
     * function: resume the thread
     */
    void resume();

protected:
    /*
     * function: check the status of the thread
     * return value: return true if the status of the thread is run, otherwise return false
     */
    bool isRunning();

    /*
     * function: the main function of the thread
     */
    virtual void running(){};

    /*
     * function: the thread function
     */
    static void* threadfunction( void* arg );

private:
    enum { Pause, Run } m_status;                              //the status of the current thread
    pthread_cond_t m_cond;                                     //the conditional variable of the status
    pthread_mutex_t m_mutex;                                   //the mutex variable of the status
};

#endif /* THREADING_H_ */
