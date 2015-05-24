/*
 *  AnimationTimeBase.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 30.05.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "AnimationTimeBase.h"

namespace cefix {

AnimationTimeBase* AnimationTimeBase::instance() {
	static osg::ref_ptr<AnimationTimeBase> s_animation_time_base = new AnimationTimeBase();
	
	return s_animation_time_base.get(); 
}

}