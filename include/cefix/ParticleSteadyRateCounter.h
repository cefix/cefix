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

#ifndef PARTICLE_STEADY_RATE_COUNTER_
#define PARTICLE_STEADY_RATE_COUNTER_

#include <osg/Referenced>
#include <cefix/Particle.h>
#include <cefix/ParticleCounter.h>

namespace cefix {

 /// standard particle-counter-class, delivers counts between a given range
template <class ParticleClass>
class ParticleSteadyRateCounter : public ParticleCounter {
	
	public:
		ParticleSteadyRateCounter(unsigned int particlesPerSecond = 100) : 
			ParticleCounter(), 
			_particlesPerSecond(particlesPerSecond), 
			_overflow(0) 
		{
		}
		
		unsigned int operator()(const typename ParticleClass::time_t& dt) {
			
			float numParticlesf = dt * _particlesPerSecond + _overflow;
			unsigned int numParticles = (int)numParticlesf;
			
			_overflow = numParticlesf - numParticles;
			
			return numParticles;		
		}
        
        void setParticlesPerSecond(unsigned int pps) { _particlesPerSecond  = pps; }
        unsigned int getParticlesPerSecond() const { return _particlesPerSecond; }
		
	private:
		unsigned int		_particlesPerSecond;
		float				_overflow;

};


}

#endif