/*
 * Checker.cpp
 *
 *  Created on: Oct 14, 2015
 *      Author: njnu
 */

#include "Checker.h"
#include "CheckerImp.h"

//class DataPanel;

Checker::Checker(CheckerImp* pCheckerImp) :
		m_pCheckerImp(pCheckerImp) {
	// TODO Auto-generated constructor stub

}

Checker::~Checker() {
	// TODO Auto-generated destructor stub
}

void Checker::running() {
	while (true) {
		m_pCheckerImp->check();

		sleep( CHECHER_TIME_OUT);
	}

}
