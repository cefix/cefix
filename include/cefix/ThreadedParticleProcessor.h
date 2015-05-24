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

#ifndef CEFIX_THREADED_PARTICLE_PROCESSOR_HEADER
#define CEFIX_THREADED_PARTICLE_PROCESSOR_HEADER

#include <cefix/ParticleProcessor.h>
#include <OpenThreads/Thread>
#include <OpenThreads/Block>


namespace cefix {
	
template <class ParticleClass, class OperationStack = ParticleOperationStack<ParticleClass> >
class ThreadedParticleProcessor : public ParticleProcessor<ParticleClass, OperationStack>, public OpenThreads::Thread {

public:
	ThreadedParticleProcessor(unsigned int maxMissedUpdates = 0) 
	:	ParticleProcessor<ParticleClass, OperationStack>(),
		OpenThreads::Thread(),
		_doingMerge(false),
		_runningOperations(false),
		_runningPostOperations(false),
		_firstRun(true),
        _lastDt(),
		_workerThreadFinished(true),
		_missedUpdates(0),
		_maxMissedUpdates(maxMissedUpdates),
        _updateBlock(),
        _startBlock()
	{
        _startBlock.reset();
        
        start();
        
        _startBlock.block();
        
        
        // std::cout << "waiting for worker thread ... " << std::endl;
		// std::cout << "worker thread ready " << std::endl;
	}
					
	
	virtual void run(const typename ParticleClass::time_t& dt)
	{
		if (_firstRun) {
			createGroup();
			runOperations(dt);
		}
		
		if (_runningOperations || _runningPostOperations) {
			++_missedUpdates;
			if ((_maxMissedUpdates > 0) && (_missedUpdates > _maxMissedUpdates)) {
				while (_runningOperations || _runningPostOperations) {
					OpenThreads::Thread::microSleep(1000);
				}
				_missedUpdates = 0;
			} else {
				return;
			}
		}
			
		merge(dt);
		
		if (_firstRun)
			runPostOperations(dt);
		_firstRun = false;
		_lastDt = dt; 
		// std::cout << "Releasing block " << std::endl;
		_updateBlock.release();
		
	}
	
	virtual void run() 
	{
		_workerThreadFinished = false;
        _startBlock.release();
        
		while(this->isAlive()) 
		{
			// std::cout << "blocking" << std::endl;
            _updateBlock.reset();
            _updateBlock.block();
            // std::cout << "block released" << std::endl;
            
            runPostOperations(_lastDt);
			runOperations(_lastDt);
			_missedUpdates = 0;
		}
		_workerThreadFinished = true;
	}

protected:

	inline void merge(const typename ParticleClass::time_t& dt) {
		_doingMerge = true;
		
		//std::cout << "merge" << std::endl;
				
		if (this->_emitter.valid())
			this->_emitter->mergeNewParticles();
		this->_particles->updateRepresentation();
		_doingMerge = false;
	}
	
	inline void runOperations(const typename ParticleClass::time_t& dt) 
	{
		_runningOperations = true;
		
		// std::cout << "runOperations" << std::endl;
		
		if (this->_emitter.valid()) 
			this->_emitter->emit(dt, this->_particles.get());
		
		if (this->_operationStack.valid())
			this->_operationStack->process(dt, this->_particles.get());
			
		this->_particles->update(dt);
		_runningOperations = false;
	}
	
	inline void runPostOperations(const typename ParticleClass::time_t& dt) 
	{
		_runningPostOperations = true;
		
		// std::cout << "runPostOperations" << std::endl;
		
		if (this->_postOperationStack.valid())
			this->_postOperationStack->process(dt, this->_particles.get());
			
		this->_runsWithoutAction = (this->_particles.valid() && this->_particles->isAlive())  ? 0 : this->_runsWithoutAction+1;
		this->_isAlive = ( this->_runsWithoutAction <= 30 );
		_runningPostOperations = false;
	}
protected:
	virtual ~ThreadedParticleProcessor()
	{
		this->terminate();
		
		
		while (!_workerThreadFinished) {
			_updateBlock.release();
			OpenThreads::Thread::microSleep(1000*1000);
		}
		
	}
	
	
private:
	
	inline void createGroup()
	{
		if (!this->_particles.valid()) {
			this->_particles = new typename ParticleProcessor<ParticleClass>::Group();
			this->checkRecycling();
		}
	}
	
	bool _doingMerge, _runningOperations, _runningPostOperations, _firstRun;
	typename ParticleClass::time_t	_lastDt;
	bool _workerThreadFinished;
	unsigned int _missedUpdates,_maxMissedUpdates;
    OpenThreads::Block _updateBlock, _startBlock;
};

}


#endif