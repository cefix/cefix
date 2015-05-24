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

#ifndef PARTICLE_PLACER_HEADER
#define PARTICLE_PLACER_HEADER

#include <osg/Referenced>

namespace cefix {

/** base class for all particle-shooters, no functionality */
class ParticlePlacer : public osg::Referenced {
	public:
		ParticlePlacer() : osg::Referenced() {}
};

}

#endif