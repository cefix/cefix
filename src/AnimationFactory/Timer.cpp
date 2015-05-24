/*
 *  Timer.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 03.09.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include <cefix/Timer.h>
#include <cefix/AnimationFactory.h>

namespace cefix {

void Timer::start() 
{
	AnimationFactory::instance()->add(this);
	_stopped = false;
	_inited = false;
}

}