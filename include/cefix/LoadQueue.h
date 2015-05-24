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

#ifndef LOAD_QUEUE_HEADER_
#define LOAD_QUEUE_HEADER_
#include <list>
#include <cefix/AbstractDataContainer.h>
#include <OpenThreads/Thread>
#include <cefix/AnimationBase.h>

namespace cefix {

/** this class implements a loading queue, the objects are loaded asynchronously in another thread*/

class CEFIX_EXPORT LoadQueue : public osg::Referenced, public OpenThreads::Thread {
    public: 
        /** default constructor */
        LoadQueue() : 
			OpenThreads::Thread(), 
			_finished(false), 
			_cancelled(false),
			_loadInProgress(false),
			_pausing(false)
		{
            setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_NOMINAL);
        }
    
        /** runs the loading thread */
        virtual void run();
        
        /** quits the thread */
        void quit() {
            _cancelled = true;
            while (!_finished) {
				OpenThreads::Thread::YieldCurrentThread();
			}
            osg::notify(osg::INFO) << "LoadQueue finished" << std::endl;
        }
            
        /** adds an AbstractDataContainer to the queue */
        void add(AbstractDataContainer* data);        
        /** @return the nr. of remaining items in the queue */
        unsigned int getRemainingItems();  
		
		/** @return the nr of unhandled error-msgs */
		unsigned int getUnhandledErrorCount();
		
		/** @return the unhandled errors */
		AbstractDataContainer::ErrorVector getUnhandledErrors();
		
		/** clears the observer */
		void clearObserver();
		
		void setPauseFlag(bool flag);
		
		/** dtor*/
		virtual ~LoadQueue();

    protected:
		typedef std::list< osg::ref_ptr< AbstractDataContainer > > Queue;
		
        bool									_finished, _cancelled;
		AbstractDataContainer::ErrorVector		_errors;
		bool									_loadInProgress;
		bool									_pausing;
		Queue									_queue;
};

}

#endif