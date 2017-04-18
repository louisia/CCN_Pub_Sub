/*
 * MulticastThreading.cpp
 *
 *  Created on: 2017年3月14日
 *      Author: louisia
 */

#include "MulticastThreading.h"
#include"../CCN_Layer/Structure.h"
#include"../CCN_Layer/Slicer.h"
#include"../CCN_Layer/Reformer.h"
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <stdio.h>
#include <openssl/md5.h>
MulticastThreading::MulticastThreading(EPS_Router *epsRouter) :
		m_epsRouter(epsRouter) {

}

MulticastThreading::~MulticastThreading() {

}

void MulticastThreading::running() {

	m_epsRouter->multicast();

}

