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

#ifndef CEFIX_RK2_PARTICLE_HEADER
#define CEFIX_RK2_PARTICLE_HEADER

#include <cefix/Particle.h>

namespace cefix {


/** particle base class, integrating velocity and position via Runge Kutta (order 2) */

class RK2Particle : public Particle {

public:
	RK2Particle() : Particle() {}
	
	inline void setPosition(const osg::Vec3& p) { _position = _lastPosition = p; }
	
	inline void updatePosition(time_t dt) 
	{
		if (!_isFixed && _isAlive) 
		{
			_lastPosition = _position;
			_position +=_velocity*dt + _acceleration * static_cast<time_t>(0.5) * dt * dt;
			_velocity += _acceleration * dt;
			
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