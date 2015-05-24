/*
 *  AnimationFactory.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 08.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include <cefix/AnimationFactory.h>
#include <cefix/DataFactory.h>

namespace cefix {

static osg::ref_ptr<AnimationFactory> s_animation_factory = NULL; 

static void cleanup_animation_factory() {
    s_animation_factory = NULL;
}

struct AnimationfactoryIniter {
    AnimationfactoryIniter()
    {
        s_animation_factory = new AnimationFactory();
        DataFactory::instance()->addCleanupHandler(&cleanup_animation_factory);
    }
};

AnimationFactory* AnimationFactory::instance() 
{
    static AnimationfactoryIniter initer;
    return s_animation_factory.get();
    
}

}
