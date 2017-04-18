/*
 * SubListUpload.h
 *
 *  Created on: Apr 10, 2016
 *      Author: njnu
 */

#ifndef SUBLISTUPLOAD_H_
#define SUBLISTUPLOAD_H_

#include "CheckerImp.h"

#include "ControlPanel.h"
#include "DataPanel.h"

class SubListUploader: public CheckerImp {
public:
    SubListUploader();
    virtual ~SubListUploader();

    /*
     * function: the implementation of the interface of the checker
     */
    virtual void check();

    /*
     * function: set the datapanel controlled by current controlpanel,
     *          and it should be called before calling the check function
     */
    void setDataPanel( DataPanel* pDataPanel );

    /*
     * function: set the current controlpanel, and it should be called before calling the check function
     */
    void setControlPanel( ControlPanel* pControlPanel );

private:
    /*
     * function: generate the subscription list from the file named by the file named fileName
     * parameters: fileName : the name of the file which contain the content to publish
     * return value: return the subscription list of the content to publish
     */
    vector< string > readSubList( string fileName );

private:
    DataPanel* m_pDataPanel;               //the datapanel controlled by the current controlpanel
    ControlPanel* m_pControlPanel;         //the current controlpanel
};

#endif /* SUBLISTUPLOAD_H_ */
