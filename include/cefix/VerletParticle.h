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

#ifndef CEFIX_VERLET_PARTICLE_HEADER
#define CEFIX_VERLET_PARTICLE_HEADER

#include <cefix/Particle.h>

namespace cefix {


/** particle base class, integrating velocity and position via Verlet integration */

class VerletParticle : public Particle {

public:
	VerletParticle() : Particle() {}
	
	inline void addVelocity(const osg::Vec3& v ) { _position += v; }
	inline void setVelocity(const osg::Vec3& v ) { _position = _lastPosition + v; }
	inline osg::Vec3 getVelocity() { return (_position - _lastPosition); }

	inline void resetPosition(const osg::Vec3& p) { _position = _lastPosition = p; }
    inline void setPosition(const osg::Vec3& p) { osg::Vec3 v(getVelocity()); _position = p; _lastPosition = p - v;}
	
	inline void updatePosition(time_t dt) 
	{
		if (!_isFixed && _isAlive) 
		{
			osg::Vec3 temp = _position;
			_position = (_position * 2)  - _lastPosition + _acceleration * dt * dt;
			_lastPosition = temp;
			
			updateRotation(dt);
			_acceleration.set(0,0,0);
		}
	}
	
	inline void update(time_t dt) {
		_t += dt;
		_isAlive = (_lifetime >= 0) ? (_t <= _lifetime) : true;
		updatePosition(dt);
	}

};

}

#endif