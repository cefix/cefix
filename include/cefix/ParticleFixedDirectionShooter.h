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

#ifndef PARTICLE_FIXED_DIRECTION_SHOOTER 
#define PARTICLE_FIXED_DIRECTION_SHOOTER

#include <cefix/MathUtils.h>
#include <cefix/ParticleShooter.h>

/** shooter class wich shoots particles in a fixed direction */
namespace cefix {

		
	/** small shooter-class, which will shoot particles in a fixed direction with optionally variable intensity */
	template <class ParticleClass>
	class ParticleFixedDirectionShooter : public ParticleShooter {
		public:
			/**
			 * @param direction direction to shoot particle to
			 * @param intensity range of intensity
			 */
			ParticleFixedDirectionShooter(const osg::Vec3& direction, cefix::floatRange intensity = cefix::floatRange(1,1)) : ParticleShooter(), _direction(direction), _intensity(intensity) {}
			
			/** shoots the particle in given direction */
			inline osg::Vec3 operator()(ParticleClass* p) { return _direction * _intensity.random(); }
			
		private:
			osg::Vec3	_direction;
			floatRange	_intensity;
	};
}

#endif