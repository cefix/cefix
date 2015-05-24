/*
 *  AnimationBase.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 08.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include <cefix/AnimationBase.h>
#include <cefix/PropertyList.h>
#include <cefix/Settings.h>
#include <cefix/AllocationObserver.h>

namespace cefix {
AnimationBase::AnimationBase(float startTime) : 
				osg::Referenced(), 
				_finished(false), 
				_inited(false),
                _cancelled(false),
				_startTime(startTime),
                _parent(NULL)
{
	cefix::AllocationObserver::instance()->observe(this);
}


AnimationBase::AnimationBase(float startTime, bool dontobserve) : 
				osg::Referenced(), 
				_finished(false), 
				_inited(false),
                _cancelled(false),
				_startTime(startTime),
                _parent(NULL) 
{
}

}