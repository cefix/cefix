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

#ifndef PARTICLE_PROCESSOR_HEADER
#define PARTICLE_PROCESSOR_HEADER

#include <cefix/ParticleGroup.h>
#include <cefix/ParticleEmitter.h>
#include <cefix/ParticleOperationStack.h>
#include <cefix/Export.h>
#include <cefix/AnimationBase.h>
#include <cefix/ParticlesForRecyclingQueue.h>

namespace cefix {

/** this class unifies the processing of particles, it emits new particles via a ParticleEmitter, it process the particles via a ParticleProgramStack and updates them */
template <class ParticleClass, class OperationStack = cefix::ParticleOperationStack<ParticleClass> >
class CEFIX_EXPORT ParticleProcessor : public osg::Referenced {
	
	public:
		typedef ParticleGroup<ParticleClass> Group;
		typedef ParticlesForRecyclingQueue<ParticleClass> RecyclingQueue;
		typedef ParticleEmitter<ParticleClass> Emitter;
		typedef typename Group::DeleteCallback DeleteCallback;
		
		enum Mode { TERMINATE_WHEN_NO_PARTICLES, RUN_FOREVER } ;
		/** ctor */
		ParticleProcessor() : osg::Referenced(), _isAlive(true), _runsWithoutAction(0), _useRecycling(false), _mode(TERMINATE_WHEN_NO_PARTICLES), _terminate(false), _enabled(true) {}
		
		void setRunMode(Mode mode) { _mode = mode; }
		
		void terminate() { _terminate = true; }
		
		/** get the group of particles */
		Group* getParticles() { return _particles.get(); }
		
		/** sets a new group with particles */
		void setParticles(Group* g) { _particles = g; checkRecycling(); }
		
		/* adds a particle */
		void addParticle(ParticleClass* p) { 
			if (!_particles.valid()) {
				_particles = new Group();
				checkRecycling();
			}
			_particles->add(p);
		}
		
		/** emit new particles, process them and update them */
		virtual void run(const typename ParticleClass::time_t& dt)
		{
			if (!_particles.valid()) {
				_particles = new Group();
				checkRecycling();
			}
			if (_emitter.valid()) 
				_emitter->emit(dt, _particles.get());
			
			if (_operationStack.valid())
				_operationStack->process(dt, _particles.get());
			
			if (_emitter.valid())
				_emitter->mergeNewParticles();
			_particles->update(dt);
			_particles->updateRepresentation();
			
			if (_postOperationStack.valid())
				_postOperationStack->process(dt, _particles.get());
			
			_runsWithoutAction = (_particles->isAlive())  ? 0 : _runsWithoutAction+1;
			_isAlive = ( _runsWithoutAction <= 30 ); 
			
		}

		
		/** get the ParticleEmitter */
		Emitter* getEmitter() { return _emitter.get(); }
		
		/** sets the Emitter */
		void setEmitter(Emitter* emitter) { _emitter = emitter; checkRecycling();  }
		
		/** get the programStack */
		OperationStack* getOperationStack() { return _operationStack.get(); }
		
		/** sets the operation stack */
		void setOperationStack(OperationStack* operationStack) { _operationStack = operationStack; } 
		
		/** sets the post OperationStack */
		void setPostOperationStack(OperationStack* operationStack) { _postOperationStack = operationStack; } 
		
		bool isAlive() { return ((_isAlive || (_mode == RUN_FOREVER)) && (!_terminate)) ; }
		
		inline void enableParticleRecycling() { _useRecycling = true; checkRecycling(); }
		inline void disableParticleRecycling() { _useRecycling = false; checkRecycling(); }
		inline bool isEnabled() const { return _enabled; }
		
		inline void enable() { _enabled = true; }
		inline void disable() { _enabled = false; }
		
		void emitParticles(unsigned int amount) 
		{
			if (!_particles.valid()) {
				_particles = new Group();
				checkRecycling();
			}
			
			while (_emitter.valid() && (_particles->size() < amount))
				_emitter->emit(0.01,_particles.get());
		}
		
		virtual void applyWidgetComposer(WidgetComposer& composer) 
		{
			if (_emitter.valid())
				_emitter->applyWidgetComposer(composer);
			if (_operationStack.valid())
				_operationStack->applyWidgetComposer(composer);
			if (_postOperationStack.valid())
				_postOperationStack->applyWidgetComposer(composer);
		}
        
        void setDeletedCallback(DeleteCallback* cb) { _deletedCallback = cb; checkDeletedCallback(); }
        
	protected: 
	
		void checkRecycling()
		{
			if (_useRecycling) {
				if (!_queue.valid())
					_queue = new RecyclingQueue();
				
				if (_particles.valid())
					_particles->setRecyclingQueue(_queue.get());
				if (_emitter.valid()) 
					_emitter->setRecyclingQueue(_queue.get());
			} 
			else 
			{
				if (_particles.valid())
					_particles->setRecyclingQueue(NULL);
				if (_emitter.valid()) 
					_emitter->setRecyclingQueue(NULL);
				_queue = NULL;
			}
            
            checkDeletedCallback();
		}
        
        void checkDeletedCallback() {
            if (_particles.valid())
                _particles->setDeletedCallback(_deletedCallback);
        }

				
		virtual ~ParticleProcessor() {}
		
	protected:
		osg::ref_ptr<Group>						_particles;
		osg::ref_ptr<Emitter >					_emitter;
		osg::ref_ptr<OperationStack>			_operationStack, _postOperationStack;
		bool									_isAlive;
		unsigned int							_runsWithoutAction;
		osg::ref_ptr<RecyclingQueue>			_queue;
		bool									_useRecycling;
		
		Mode									_mode;
		bool									_terminate, _enabled;
        osg::ref_ptr<DeleteCallback>            _deletedCallback;

};


/** small adapter-class, so you can add a ParticleProcessor to the Animationfactory, which will then update the particle-processor */
template <class ParticleClass>
class ParticleProcessorAnimationAdapter : public cefix::AnimationBase {

	public:
		/** ctor */
		ParticleProcessorAnimationAdapter(ParticleProcessor<ParticleClass>* p, bool realtime = true, float delta = 0.0f) : cefix::AnimationBase(0), _p(p), _realtime(realtime), _delta(delta), _last(0) {}
		
	protected:
		/** animate */
		virtual void animate(float t) 
		{
			if ( _p.valid() && _p->isEnabled() ) {
			_p->run(_realtime ? t - _last : _delta); 
			
			if (!_p->isAlive()) {
				std::cout << "finishing particleProcessor ...." << std::endl;
				// wenn keine partikel mehr da sind zum animieren, dann selbstzerstörung :)
				setFinishedFlag();
			}
			_last = t;
		}
		else 
			setFinishedFlag();
	}
	
	private:
		osg::ref_ptr< ParticleProcessor<ParticleClass> >		_p;
		bool									_realtime;
		float									_delta;
		float									_last;
	
};

}


#endif