/*
 * SubListDownload.h
 *
 *  Created on: Apr 10, 2016
 *      Author: njnu
 */

#ifndef SUBLISTDOWNLOAD_H_
#define SUBLISTDOWNLOAD_H_

#include "CheckerImp.h"

#include "ControlPanel.h"
#include "DataPanel.h"

class SubListDownloader: public CheckerImp {
public:
    SubListDownloader();
    virtual ~SubListDownloader();

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
    DataPanel* m_pDataPanel;               //the datapanel controlled by the current controlpanel
    ControlPanel* m_pControlPanel;         //the current controlpanel
};

#endif /* SUBLISTDOWNLOAD_H_ */
