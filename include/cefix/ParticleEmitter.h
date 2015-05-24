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

#ifndef PARTICLE_EMITTER_HEADER
#define PARTICLE_EMITTER_HEADER

#include <osg/observer_ptr>
#include <osg/Node>

#include <cefix/Particle.h>
#include <cefix/ParticleGroup.h>
#include <cefix/ParticlePointPlacer.h>
#include <cefix/ParticleRadialShooter.h>
#include <cefix/ParticleSteadyRateCounter.h>

#include <cefix/MathUtils.h>
namespace cefix {

class WidgetComposer;
	
/** simple callback which gets called for every emitted particle */	
template <class ParticleClass>
class ParticleEmittedCallback : public osg::Referenced {
public:
	
	virtual void operator()(ParticleClass* p, bool recycled) = 0;
	
};


/** abstract particle emitter base class */
template <class ParticleClass>
class ParticleEmitter : public osg::Referenced {

	public:
		typedef ParticleGroup<ParticleClass> Group;
		typedef ParticlesForRecyclingQueue<ParticleClass> RecyclingQueue;
		typedef ParticleEmittedCallback<ParticleClass> Callback;
		typedef std::vector< std::pair<ParticleClass*, bool> > CreatedParticlesVector;
		
		/** ctor */
		ParticleEmitter() : osg::Referenced(), _lifetime(-1), _t(0), _createCallback(NULL), _mergeCallback(NULL) {}
		
		/** emits new particle */
		void emit(const typename ParticleClass::time_t& dt, Group* group) {
			_t += dt;
			if (_trackedNode.valid())
				_currentWorldMatrix = _trackedNode->getWorldMatrices()[0];
			if (isAlive())
				createNewParticles(dt, group);
		}
		
					
		/** track given node to transform the particles from local space to world space */
		void trackNode(osg::Node* node) { _trackedNode = node; }
		
		/** @return the tracked node */
		osg::Node* getTrackedNode() { return _trackedNode.get(); }
		
		/** create new particles for this frame */
		virtual void createNewParticles(const typename ParticleClass::time_t& dt, Group* group) = 0;
		
		/** sets the lifetime for this emitter */
		inline void setLifeTime(double t) { _lifetime = t; _t = 0; } 
		
		/** returns true, if this emitter is aliver */
		inline bool isAlive() { return (_lifetime < 0) ? true : _t <= _lifetime; }
		
		void setRecyclingQueue(RecyclingQueue* q) { _particlesForRecycling = q; }
		
		void setCreateCallback(Callback* cb) { _createCallback = cb; }
		Callback* getCreateCallback() const { return _createCallback; } 
		
		void setMergeCallback(Callback* cb) { _mergeCallback = cb; }
		Callback* getMergeCallback() const { return _mergeCallback; } 
		
		virtual void mergeNewParticles() 
		{
			for(typename CreatedParticlesVector::iterator i = _createdParticles.begin(); i != _createdParticles.end(); ++i) {
				mergeNewParticle((*i).first, (*i).second);
				if (this->_mergeCallback.valid()) (*this->_mergeCallback)( (*i).first, (*i).second );
			}
			_createdParticles.clear();
		}
		
		/** apply custom sttings to a created particle, should be implemented by a subclass */
		virtual void applySettings(ParticleClass* p, bool recycled) {}
		
		/** merge new particle with scenegraph, sould be implemented by a subclass */
		virtual void mergeNewParticle(ParticleClass* p, bool recycled) {}
		
		virtual void applyWidgetComposer(WidgetComposer& composer) {}
	protected:
	
		void addNewParticle(ParticleClass* p, bool is_recycled)
		{ 
			_createdParticles.push_back(std::make_pair(p, is_recycled)); 
			this->applySettings(p, is_recycled);
			if (this->_createCallback.valid()) (*this->_createCallback)( p, is_recycled );
		}
	
		ParticleClass* getRecycledParticle() 
		{
			if ((!_particlesForRecycling.valid()) || (!_particlesForRecycling->hasParticles()))
				return NULL;
			ParticleClass* p = _particlesForRecycling->get();
			if (p) p->rebirth();
			return p;
		}
		
		virtual ~ParticleEmitter() {}
		
		
		inline osg::Vec3 transformToLocal(osg::Vec3 v) 
		{
			if (_trackedNode.valid()) 
			{
				return _currentWorldMatrix.preMult(v);
			}
			else
				return v;
		}
		
		inline osg::Vec3 rotateToLocal(osg::Vec3 v) 
		{
			if (_trackedNode.valid()) {
				return _currentWorldMatrix.preMult(v) - _currentWorldMatrix.preMult(osg::Vec3(0,0,0));
			}
			else
				return v;
		}
		
		CreatedParticlesVector& getCreatedParticlesVector() { return _createdParticles; }
		CreatedParticlesVector& getCreatedParticlesVector() const { return _createdParticles; }
						
	private:
	
		
		osg::observer_ptr<osg::Node>	_trackedNode;
		osg::Matrix						_currentWorldMatrix;
		typename ParticleClass::time_t				_lifetime, _t;
		osg::ref_ptr<RecyclingQueue>	_particlesForRecycling;
		CreatedParticlesVector	_createdParticles;
	
	protected:
		osg::ref_ptr<Callback>				_createCallback;
		osg::ref_ptr<Callback>				_mergeCallback;

};



/** base template-class for all particle-emitters, the four template-parameters are:
 *  ParticleClass : the concrete particle-class, the emitter creates on demand
 *  Counter       : the particle-counter-class, should have an <code>unsigned int operator()()</code> defined, which reports the # of particles to create
 *  Placer        : the particle-placer-class, should have an <code>osigtdg::Vec3 operator()(Particle* p)</code> defined, which returns the point of placement
 *  Shooter       : the particle-shooter-class, should have an <code>osg::Vec3 operator()(Particle* p)</code> defined, which returns the initial velocity
 */
 
template<	class ParticleClass, 
			class Counter = ParticleSteadyRateCounter<ParticleClass>, 
			class Placer = ParticlePointPlacer<ParticleClass>, 
			class Shooter = ParticleRadialShooter<ParticleClass> >class ParticleEmitterT : public ParticleEmitter<ParticleClass> {

	public:
		typedef ParticleGroup<ParticleClass> Group;
		
		
		using ParticleEmitter<ParticleClass>::getRecycledParticle;
				
		/** ctor */
		ParticleEmitterT() : ParticleEmitter<ParticleClass>(), _counter(new Counter()), _placer(new Placer()), _shooter(new Shooter()) {}
		
		/** ctor */
		ParticleEmitterT(Counter* counter, Placer* placer, Shooter* shooter) : 
			ParticleEmitter<ParticleClass>(), 
			_counter(counter), 
			_shooter(shooter),
			_placer(placer)
		{
		}
		
		
		
		/** create new particles and add them to the group */
		void createNewParticles(const typename ParticleClass::time_t& dt, Group* group) 
		{
			this->getCreatedParticlesVector().clear();
			unsigned int cnt = (*_counter)(dt);
			if (cnt > 0) {
				this->getCreatedParticlesVector().reserve(cnt);
				bool recycled = true;
				for(unsigned int i = 0; i < cnt; ++i) {
					recycled = true;
					ParticleClass* p = getRecycledParticle();
					if (p == NULL) {
						p = new ParticleClass();
						recycled = false; 
					}
					
					p->setPosition( this->transformToLocal( (*_placer)(p)  ) );
					p->setVelocity( this->transformToLocal( (*_shooter)(p) ) );
					group->add(p);
					
					this->addNewParticle(p, recycled);
					
				}
			}
		
		};
				
		/** gets the current counter */
		Counter* getCounter() { return _counter.get(); }
		
		/** gets the current shooter */
		Shooter* getShooter() { return _shooter.get(); }
		
		/** gets the current placer */
		Placer* getPlacer() { return _placer.get(); }
		
		/** sets the shooter to use */
		void setShooter(Shooter* shooter) { _shooter = shooter; }
		
		/** sets the placer */
		void setPlacer(Placer* placer) { _placer = placer; }
		
		/** sets the placer */
		void setCounter(Counter* counter) { _counter = counter; }
		
		
	protected:
	
		virtual ~ParticleEmitterT() {}
	private:
		
		osg::ref_ptr<Counter>	_counter;
		osg::ref_ptr<Shooter>	_shooter;
		osg::ref_ptr<Placer>	_placer;



};

}

#endif