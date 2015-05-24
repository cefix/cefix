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

#ifndef PARTICLE_FOUNTAIN_EMITTER_HEADER
#define PARTICLE_FOUNTAIN_EMITTER_HEADER

#include <osg/Group>

#include <cefix/ParticleGroup.h>
#include <cefix/ParticleEmitter.h>



namespace cefix {

/** this particle-emitter-template-class places the particles at the given origin, adds them to a given group-node and 
    shoots them via a radial-shooter */

template <class ParticleClass> class ParticleFountainEmitter : public cefix::ParticleEmitterT<ParticleClass> {

	public:
		/** ctor 
		  * @param group group where the particles are added (the particleClass must implement the getNode-method
		  * @param origin origin of the shooter
		  * @param newParticlesPerSecond how many particles shoul dbe created per second
		  * @param speedRange a range of min and max speed 
		  * @param phiRange range for angle phi for the radial shooter
		  * @param thetaRange range for angle theta for the radial shooter
		  */
		ParticleFountainEmitter(	osg::Group* group, 
									osg::Vec3 origin, 
									unsigned int newParticlesPerSecond, 
									const cefix::floatRange& speedRange,
									const cefix::floatRange& phiRange, 
									const cefix::floatRange& thetaRange) :
			ParticleEmitterT<ParticleClass>( 
				new cefix::ParticleSteadyRateCounter<ParticleClass>(newParticlesPerSecond),
				new cefix::ParticlePointPlacer<ParticleClass>(origin),
				new cefix::ParticleRadialShooter<ParticleClass>(speedRange, phiRange, thetaRange)),
			_root(group)
		{
		}
		
		inline virtual void mergeNewParticle(ParticleClass* p, bool recycled) {
			if (!recycled && _root.valid())
				_root->addChild(p->getNode());
		}
		
		
		
		
	private:
		osg::ref_ptr<osg::Group>	_root;

};



}

#endif