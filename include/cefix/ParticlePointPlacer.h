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

#ifndef PARTICLE_POINT_PLACER_HEADER
#define PARTICLE_POINT_PLACER_HEADER

#include <osg/Vec3>

#include <cefix/ParticlePlacer.h>

namespace cefix {

class Particle;

/** standard point particle-placer */
template <class ParticleClass>
class ParticlePointPlacer : public ParticlePlacer {
	public:
		ParticlePointPlacer(const osg::Vec3& p = osg::Vec3(0,0,0)) : ParticlePlacer(), _center(p) {}
		
		const osg::Vec3& operator()(ParticleClass* p) const { return _center; }
		
		const osg::Vec3& getCenter() const { return _center; }
        void setCenter(const osg::Vec3& center) { _center = center; }
		
	private:
		osg::Vec3	_center;
};


};

#endif