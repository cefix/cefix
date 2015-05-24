/*
 *  AnimationBasedOnDuration.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 08.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include <cefix/AnimationBasedOnDuration.h>
namespace cefix {

	AnimationBasedOnDuration::AnimationBasedOnDuration(float startTime, float duration, bool easyInOut) 
		: AnimationBase(startTime), 
		_duration(duration), 
		_easyInOut(easyInOut) 
	{
}
}