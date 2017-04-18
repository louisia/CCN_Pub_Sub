/*
 * DataPanel.h
 *
 *  Created on: Sep 20, 2015
 *      Author: njnu
 */

#ifndef DATAPANEL_H_
#define DATAPANEL_H_

#include "Panel.h"
#include "Checker.h"
#include "CheckerImp.h"
class DataPanel: public Panel, public CheckerImp, public Threading {
public:
    DataPanel( std::string localIP,string filename="" );
    virtual ~DataPanel();

    /*
     * function: the main logical handle function fo the control panel
     */
    virtual void running();

    /*
     * function: set the fib
     * parameters: fib : the fib table to setS
     */
    void setFIB( const std::vector< FIBItem >& fib );

    /*
     * function: check the data overtime
     */
    virtual void check(){};

    /*
     * function: set the roelID of current node
     * parameters: roleID : the roleID to set
     */
    void setRoleID( RoleID roleID );

    /*
     * function: get the roleID of current node
     */
    RoleID getRoleID();


    /*
     * function: set the subscription list
     * parameters: subList: the sublist to be set
     */
    void setSubList( const std::vector< std::string >& subList );

    /*
     * function: get the subscription list
     * return value: subList: the current sublist
     */
    std::vector< std::string > getSubList( );

    map<string, unsigned long> getRRotSet();

    string getFileName();


protected:
    /*
     * function: handle the message received
     * parameters: msg : the message received
     */
    virtual void handleMsg( Message& msg ){};

    /*
     * function: get the face list to translate to get the content
     * parameters: contentName : the name of the content to get
     * return value: the face list to translate
     */
    std::vector< int > getFaceList( std::string contentName );



private:
    Checker m_checker;                                 //the checker to check the time out item
    std::vector< FIBItem > m_fib;                      //the fib table

    RoleID m_roleID;                                   //the roleID of current node


    std::vector< std::string > m_subList;              //the subscription list
    pthread_mutex_t m_subListMutex;                    //the mutex of the subscription list
    std::string m_filename;

};

#endif /* DATAPANEL_H_ */
