/*
 *
 *      (\(\
 *     (='.')           cefix --
 *    o(_")")           a multipurpose library on top of OpenSceneGraph
 *  
 *
 *                      Copyright 2001-2011, stephanmaximilianhuber.com 
 *                      Stephan Maximilian Huber. 
 *
 *                      All rights reserved.
 *
 */

#ifndef PARTICLE_SHOOTER_HEADER
#define PARTICLE_SHOOTER_HEADER

#include <osg/Referenced>

namespace cefix {

/** base class for all particle-shooters, no functionality */
class ParticleShooter : public osg::Referenced {
	public:
		ParticleShooter() : osg::Referenced() {}
};

}

#endif