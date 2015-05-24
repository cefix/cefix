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


#ifndef PARTICLE_RANDOM_RATE_COUNTER_HEADER
#define PARTICLE_RANDOM_RATE_COUNTER_HEADER

#include <cefix/MathUtils.h>
#include <cefix/Particle.h>
#include <cefix/ParticleCounter.h>

namespace cefix {

 /// standard particle-counter-class, delivers counts between a given range
template <class ParticleClass>
class ParticleRandomRateCounter : public ParticleCounter {
	
	public:
		ParticleRandomRateCounter(const cefix::floatRange& range = cefix::floatRange(0,100) ) : 
			ParticleCounter(), 
			_range(range), 
			_overflow(0) 
		{
		}
		
		unsigned int operator()(const typename ParticleClass::time_t& dt) {
			
			float numParticlesf = dt * _range.random() + _overflow;
			unsigned int numParticles = (int)numParticlesf;
			
			_overflow = numParticlesf - numParticles;
			
			return numParticles;		
		}
        
        void setRange(const cefix::floatRange& range) { _range = range; }
        
        const cefix::floatRange& getRange() const { return _range; }
		
	private:
		cefix::floatRange	_range;
		float				_overflow;

};

}


#endif
