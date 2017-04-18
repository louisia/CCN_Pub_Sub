/*
 * Checker.h
 *
 *  Created on: Oct 14, 2015
 *      Author: njnu
 */

#ifndef CHECKER_H_
#define CHECKER_H_

#include "Threading.h"
#include "CheckerImp.h"

class Checker: public Threading {
public:
    Checker( CheckerImp* pDataPanel );
    virtual ~Checker();

private:
    virtual void running();

private:
    CheckerImp* m_pCheckerImp;                        //the pointer of the data panel which has the virtual function named check
};

#endif /* CHECKER_H_ */
