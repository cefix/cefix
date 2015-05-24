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

#ifndef PARTICLE_RADIAL_SHOOTER_HEADER
#define PARTICLE_RADIAL_SHOOTER_HEADER

#include <cefix/MathUtils.h>
#include <cefix/Particle.h>
#include <cefix/ParticleShooter.h>

namespace cefix {

/** standard radial Particle-shooter */
template <class ParticleClass>
class ParticleRadialShooter : public ParticleShooter {
	
	public:
		ParticleRadialShooter(
				const cefix::floatRange& speedRange = cefix::floatRange(10,10), 
				const cefix::floatRange& phiRange = cefix::floatRange(-osg::PI_4, osg::PI_4), 
				const cefix::floatRange& thetaRange = cefix::floatRange(-osg::PI_4, osg::PI_4),
				const cefix::vec3Range& rotSpeedRange = cefix::vec3Range()) :
			ParticleShooter(),
			_speedRange(speedRange),
			_phiRange(phiRange),
			_thetaRange(thetaRange),
			_rotSpeedRange(rotSpeedRange)
		{
		}
		
		osg::Vec3 operator()(ParticleClass* p) {
			float theta = _thetaRange.random();
			float phi =   _phiRange.random();
			float speed = _speedRange.random();
			osg::Vec3 q = _rotSpeedRange.random();
			p->setAngularVelocity(q);
			
			return osg::Vec3(
				speed * sinf(theta) * cosf(phi),
				speed * sinf(theta) * sinf(phi),
				speed * cosf(theta)
				);
		}
		
		void setSpeedRange(float min, float max) { setSpeedRange(cefix::floatRange(min,max)); }
		void setSpeedRange(const cefix::floatRange& range) { _speedRange = range; }
        void setPhiRange(const cefix::floatRange& range) { _phiRange = range; }
        void setThetaRange(const cefix::floatRange& range) { _thetaRange = range; }
		void setInitialRotationSpeedRange(const cefix::vec3Range& rotSpeedRange) { _rotSpeedRange = rotSpeedRange; }
		
	private:
		cefix::floatRange _speedRange, _phiRange, _thetaRange;
		vec3Range _rotSpeedRange;

};


}

#endif