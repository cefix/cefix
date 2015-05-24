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

#ifndef PARTICLES_FOR_RECYCLING_QUEUE_HEADER
#define PARTICLES_FOR_RECYCLING_QUEUE_HEADER

#include <cefix/Particle.h>
#include <deque>
#include <iostream>

namespace cefix {

template <class ParticleClass>
class ParticlesForRecyclingQueue : public osg::Referenced {
	
	public:
		ParticlesForRecyclingQueue() : osg::Referenced() {}
		
		void add(ParticleClass* p) { _queue.push_back(p); }
		ParticleClass* get() { osg::ref_ptr<ParticleClass> p = _queue.front().get(); _queue.pop_front(); return p.release(); }
		bool  hasParticles() const { return (_queue.size() > 0); }
		
		unsigned int size() const { return _queue.size(); }
		
	private:
		void dump() { std::cout << "ParticlesForRecyclingQueue size: " << _queue.size() << std::endl; }
		std::deque< osg::ref_ptr <ParticleClass> > _queue;
};

}

#endif