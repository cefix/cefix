/*
 *  ThreadPool.cpp
 *  ThreadPool
 *
 *  Created by Stephan Huber on 01.03.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "ThreadPool.h"


namespace cefix {

	osg::NotifySeverity ThreadPool::_s_notifySeverity(osg::DEBUG_INFO);

// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

ThreadPool::WorkerThread::WorkerThread(ThreadPool* pool) 
:	osg::Referenced(), 
	OpenThreads::Thread(), 
	_pool(pool),
	_currentJob(NULL),
	_mayFinish(false),
	_isFinished(false),
	_isWaiting(false)
{
	static unsigned int s_id = 0;
	_id = s_id++;
}


// ----------------------------------------------------------------------------------------------------------
// continueWork
// ----------------------------------------------------------------------------------------------------------

void ThreadPool::WorkerThread::continueWork() {
	//std::cout << "thread " << _id << " continuing " << std::endl;
	setWaitingFlag(false);
}


// ----------------------------------------------------------------------------------------------------------
// run
// ----------------------------------------------------------------------------------------------------------

void ThreadPool::WorkerThread::run() {
	
	osg::notify(ThreadPool::getNotifySeverity()) << "thread " << _id << " is ready " << std::endl;
	
	// pool bescheid geben, dass dieser thread soweit ist...
	_pool->threadIsReady();
	
	while (_mayFinish == false) {
		_currentJob = _pool->getAvailableJob();
		if (_currentJob == NULL) {
			//std::cout << "thread " << _id << " blocking " << std::endl;
			setWaitingFlag(true);
		}
		if (_currentJob.valid()) {
			_currentJob->setPerThreadUserData(_userData.get());
			(*_currentJob)(  );
			_pool->addFinishedJob(_currentJob.get());
		}
		_currentJob = NULL;
	
	}
	osg::notify(ThreadPool::getNotifySeverity()) << "thread " << _id << " finished ... " << std::endl;

	_isFinished = true;
}



// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

ThreadPool::ThreadPool(unsigned int numThreads, bool finishWorkBeforeDestructing)
:	osg::Referenced(),
	_numThreads(numThreads),
	_paused(false),
	_finishWorkBeforeDestructing(finishWorkBeforeDestructing),
	_collectFinishedJobs(false)

{
	// force instantiation of animationfactory
	cefix::AnimationFactory* af = cefix::AnimationFactory::instance();
	af = NULL;
	for(unsigned int i = 0; i < numThreads; ++i) {
		WorkerThread* wt = new WorkerThread(this);
		_pool.push_back(wt);
		wt->start();
	}
	
	// darauf warten, dass alle Threads soweit sind...
	
	_startBarrier.block(_numThreads + 1);
	
}


// ----------------------------------------------------------------------------------------------------------
// dtor
// ----------------------------------------------------------------------------------------------------------

ThreadPool::~ThreadPool() 
{
	if (_finishWorkBeforeDestructing)
		waitUntilDone();
		
	flush();
	releaseWorkerThreads();
	waitForThreadFinishing();
	
	_pool.clear();
	osg::notify(ThreadPool::getNotifySeverity()) << "~ThreadPool" << std::endl;
}


// ----------------------------------------------------------------------------------------------------------
// addJob
// ----------------------------------------------------------------------------------------------------------

void ThreadPool::addJob(Job* job) 
{
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
		_jobs.push_back(job);
	}
	checkWaitingThreads();
}

	
void ThreadPool::checkWaitingThreads() {
	unsigned int m(0);
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
		m = _jobs.size();
	}
	unsigned int wts = 0;
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_waitingMutex);
		wts = _waitingThreads.size();
	}
	
	if ((m > 0) && (wts > 0)) {
		WorkerThread* thread(NULL);
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_waitingMutex);
			thread = _waitingThreads.begin()->first;
			_waitingThreads.erase(_waitingThreads.begin());
			_numWaitingThreads = _waitingThreads.size();
		}
		if (thread) thread->continueWork();
		
	}

}


// ----------------------------------------------------------------------------------------------------------
// releaseWorkerThreads
// ----------------------------------------------------------------------------------------------------------

void ThreadPool::releaseWorkerThreads() 
{
	for(ThreadVector::iterator i = _pool.begin(); i != _pool.end(); ++i) {
		(*i)->setFinishedFlag();
	}
}



// ----------------------------------------------------------------------------------------------------------
// getRemainingJobs
// ----------------------------------------------------------------------------------------------------------

unsigned int ThreadPool::getRemainingJobs() const
{
	unsigned int m = 0;
	{
		OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
		m = _jobs.size();
	}
	for(ThreadVector::const_iterator i = _pool.begin(); i != _pool.end(); ++i) {
		if ((*i).valid() && (*i)->getCurrentJob())
			++m;
	}
	
	return m;
}


// ----------------------------------------------------------------------------------------------------------
// done
// ----------------------------------------------------------------------------------------------------------

bool ThreadPool::done() {
	
	if (getRemainingJobs() > 0)
		return false;
	
	for (ThreadVector::iterator i = _pool.begin(); i != _pool.end(); ++i) 
		if (((*i)->isWaiting() == false) && ((*i)->isFinished() == false))
			return false;
		
	return true;
}


// ----------------------------------------------------------------------------------------------------------
// waitUntilDone
// ----------------------------------------------------------------------------------------------------------

void ThreadPool::waitUntilDone() {

	while (!done()) {
		checkWaitingThreads();
		//std::cout << (*this) << std::endl;
		osg::notify(ThreadPool::getNotifySeverity()) << (*this) << " waiting for threads ..." << std::endl;
		OpenThreads::Thread::microSleep(1000);
	}
	osg::notify(ThreadPool::getNotifySeverity()) << (*this) << " waiting for threads ..." << std::endl;
}


// ----------------------------------------------------------------------------------------------------------
// waitForThreadFinishing
// ----------------------------------------------------------------------------------------------------------

void ThreadPool::waitForThreadFinishing() {
	
	waitUntilDone();
	bool finished = false;
	while(!finished) {
		finished = true;
		for (ThreadVector::iterator i = _pool.begin(); i != _pool.end(); ++i) 
			if ((*i)->isFinished() == false)
				finished = false;
		OpenThreads::Thread::microSleep(1000);
		
	}
}


// ----------------------------------------------------------------------------------------------------------
// addFinishedJob
// ----------------------------------------------------------------------------------------------------------

void ThreadPool::addFinishedJob(Job* job)
{
	if (!_collectFinishedJobs) return;
	
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_finishedJobMutex);
	_finishedJobs.push_back(job);

}


void ThreadPool::accept(Visitor& visitor) 
{
    
    {
        for(ThreadVector::iterator i = _pool.begin(); i != _pool.end(); ++i) {
            osg::ref_ptr<Job> job = (*i)->getCurrentJob();
            if(job.valid())
                visitor(job, Visitor::JobInProgress);
        }
    }
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        for(JobDeque::iterator i = _jobs.begin(); i != _jobs.end(); ++i) {
            if(*i)
                visitor(*i, Visitor::JobWaiting);
        }
    }
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_finishedJobMutex);
        for(FinishedJobsVector::iterator i = _finishedJobs.begin(); i != _finishedJobs.end(); ++i) {
            if (*i)
                visitor(*i, Visitor::JobFinished);
        }
    }
}

// ----------------------------------------------------------------------------------------------------------
// operator << 
// ----------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& outstream, const ThreadPool& pool) {
	outstream << "remaining jobs: " << pool.getRemainingJobs() << " waiting threads: " << pool.getNumWaitingThreads() << " // ";
	for(ThreadPool::ThreadVector::const_iterator i = pool.getThreads().begin(); i != pool.getThreads().end(); ++i) {
		outstream << (*i)->isWaiting() << " ";
	}
	
	return outstream;
}




} // end of namespace