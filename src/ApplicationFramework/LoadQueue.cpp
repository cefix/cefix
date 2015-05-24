/*
 *  LoadQueue.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 11.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include "LoadQueue.h"

#include <cefix/PropertyList.h>
#include <cefix/AnimationBase.h>
#include <cefix/Notify.h>
#include <cefix/AnimationFactory.h>
#include <cefix/ErrorDispatcher.h>
#include <osg/observer_ptr>

namespace cefix {

static OpenThreads::Mutex* s_lqMutex = new OpenThreads::Mutex;

/** this animationbased class observes the laodqueue for accumulated error-messages, and if present,
    send them via the NotificationCenter to the appropiate listeners.
	This is necessary, because of threading-issues -- the AbstractDataContainer MAY NOT do anything
	which has consequences to other data or to the scene graph
*/
class AnimateLoadQueueObserver : public cefix::AnimationBase {
	public:
		/** ctor */
		AnimateLoadQueueObserver(LoadQueue* q) : cefix::AnimationBase(0.0f), _q(q) {}
		
		/** cleans up the stuff */
		virtual void cleanUp() {
			cefix::AnimationBase::cleanUp();
			if (_q.valid())
				_q->clearObserver();
			_q = NULL;
		}

		void clearQueueRef() { _q = NULL; }
		
	protected:
		/** the animate-method checks the queue every 100ms for new errormessages and send them via the
		    notificationCenter */
		virtual void animate(float elapsed_time) {
			if (elapsed_time > _lastCheck + 0.1f) {
				_lastCheck = elapsed_time;
				
				if ((_q.valid()) && (_q->getUnhandledErrorCount() > 0)) {
					AbstractDataContainer::ErrorVector errs = _q->getUnhandledErrors();
					for(AbstractDataContainer::ErrorVector::iterator i = errs.begin(); i < errs.end(); i++) {
						cefix::notify("addToErrorLog", (*i));
					}
				}
				if ((_q.valid() == false) || (_q->getRemainingItems() == 0))
					setFinishedFlag();
			}
		}
		
		float _lastCheck;
		osg::observer_ptr<LoadQueue> _q;
};


/** adds an AbstractDataContainer to the queue */
void LoadQueue::add(AbstractDataContainer* data) {
    
    if(!data->needsQueued()) {
        // this datacontainer does not want to get queued, load it immeadetly
        if (!data->loadSucceeded()) {
            data->load();
            data->setLoadFlag();
        }
        return;
    }
    
    if (data->isQueued() || data->loadSucceeded()) // already loaded / in queue?
        return;
        
	s_lqMutex->lock();
    data->setQueued(true);
	_queue.push_back(data);
	
		
	s_lqMutex->unlock();
}

void LoadQueue::clearObserver() { 
	
}

// --------------------------------------------------------------------------------------------------------------------
// getremainingItems
// --------------------------------------------------------------------------------------------------------------------
unsigned int LoadQueue::getRemainingItems() {
	s_lqMutex->lock();
	unsigned int s = _queue.size();
	if (_loadInProgress)
		s++;
	s_lqMutex->unlock();
	return s;
}

// --------------------------------------------------------------------------------------------------------------------
// getUnhandledErrorCount
// --------------------------------------------------------------------------------------------------------------------

unsigned int LoadQueue::getUnhandledErrorCount() {
	s_lqMutex->lock();
	unsigned int s = _errors.size();
	s_lqMutex->unlock();
	return s;
}


// --------------------------------------------------------------------------------------------------------------------
// setPauseFlag
// --------------------------------------------------------------------------------------------------------------------

void LoadQueue::setPauseFlag(bool flag) {
	s_lqMutex->lock();
	_pausing = flag;
	s_lqMutex->unlock();
}

// --------------------------------------------------------------------------------------------------------------------
// getUnhandledErrors
// --------------------------------------------------------------------------------------------------------------------

AbstractDataContainer::ErrorVector LoadQueue::getUnhandledErrors() {
	s_lqMutex->lock();
	AbstractDataContainer::ErrorVector errs(_errors);
	_errors.clear();
	s_lqMutex->unlock();
	return errs;
}

// --------------------------------------------------------------------------------------------------------------------
// run
// --------------------------------------------------------------------------------------------------------------------

void LoadQueue::run() {
    _finished = false;
	ErrorDispatcher errorDispatcher("", ErrorDispatcher::THREAD_SAFE);
    while (!_cancelled) {
		s_lqMutex->lock();
		unsigned int s = _queue.size();
		s_lqMutex->unlock();
        if ((s > 0) && (_pausing == false)) {
			s_lqMutex->lock();
            osg::ref_ptr<AbstractDataContainer> data = _queue.front().get();   // erstes element holen
            _queue.pop_front();                                                // und loeschen
			_loadInProgress = true;
			s_lqMutex->unlock();
			
            data->load();
            data->setLoadFlag();
            data->setQueued(false);
			_loadInProgress = false;
			
			if (data->getErrorCount() > 0) {
				s_lqMutex->lock();
				AbstractDataContainer::ErrorVector errs = data->getErrors();
				for (	AbstractDataContainer::ErrorVector::iterator i = errs.begin();
						i != errs.end();
						i++) 
				{
					_errors.push_back((*i));
				}
				data->clearErrors();
				errorDispatcher.handle("", _errors);
				_errors.clear();
				s_lqMutex->unlock();
			}
        }

        if (s > 0) 
            osg::notify(osg::INFO) << "LoadQueue:: " << s << " items remaining in the queue." << std::endl;

        microSleep(100000);
    }
    _finished = true;
    
}

LoadQueue::~LoadQueue() 
{
	osg::notify(osg::INFO) << "LoadQueue destructed ..." << std::endl;
}

}