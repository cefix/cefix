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

#ifndef PARTICLE_OPERATOR_HEADER
#define PARTICLE_OPERATOR_HEADER

#include <cefix/ParticleGroup.h>

namespace cefix {

class WidgetComposer;

/** base class for an operator class which operates on particles */
template <class ParticleClass>
class ParticleOperator : public osg::Referenced {

	public:
		typedef ParticleGroup<ParticleClass> Group;
		
		/** ctor */
		ParticleOperator(float weight = 1.0f)
            : osg::Referenced()
            , _weight(weight)
            , _enabled(true)
            , _operateOnEveryParticle(true)
            , _group(NULL)
        {
        }
		
		/** sets the particleGroup */
		void setGroup(Group* group) { _group = group;}
		
		void begin(const typename ParticleClass::time_t& dt) {};
		
		/** operate on a particle */
		void operator()(ParticleClass* p, const typename ParticleClass::time_t& dt) {}
		
		/** finish the operation */
		void finish() {};
		
		/** @return true if this operation is enabled */
		inline bool enabled() { return _enabled; }
		
		/** sets the enabled-flag */
		inline void setEnabled(bool f) { _enabled = f; }
		
		/** return true if this operator operates on every single particle or not */
		inline bool operatesOnEveryParticle() { return _operateOnEveryParticle; }
		
		/** sets the operateOnEveryParticleFlag */
		inline void setOperatesOnEveryParticle(bool f) { _operateOnEveryParticle = f; }
		
		/** @return the group */
		inline Group* getGroup() { return _group; }
		
		inline typename Group::List& getParticles() { return _group->getParticles(); }
	
		
		/** @return the weight of this operator */
		inline const float& getWeight() { return _weight; }
        
        void applyWidgetComposer(WidgetComposer& composer) {}
	protected:
		virtual ~ParticleOperator() {}
        float	_weight;
		
	private:
		bool	_enabled;
		bool	_operateOnEveryParticle;
		
		Group*	_group;
		

};

}

#endif