/*
 * MulticastThreading.h
 *
 *  Created on: 2017年3月14日
 *      Author: louisia
 */
#include"../CCN_Layer/Threading.h"
#include"../CCN_Layer/Buffer.h"
#include"EPSRouter.h"
#ifndef CCN_TOPER_MULTICASTTHREADING_H_
#define CCN_TOPER_MULTICASTTHREADING_H_
class EPS_Router;
class MulticastThreading:public Threading {
public:
	MulticastThreading(EPS_Router *epsRouter);
	virtual ~MulticastThreading();
	virtual void running();
private:
	EPS_Router *m_epsRouter;
};

#endif /* CCN_TOPER_MULTICASTTHREADING_H_ */
