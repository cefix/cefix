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

#ifndef CEFIX_THREAD_POOL_HEADER
#define CEFIX_THREAD_POOL_HEADER

#include <deque>
#include <map>
#include <OpenThreads/Thread>
#include <OpenThreads/Block>
#include <osg/Referenced>
#include <cefix/AnimationFactory.h>

namespace cefix {

/** 
 *the threadpool class combines a number of worker threads and an interface to add jobs to the threadpool. Jobs are added to a FIFO-queue and the worker-threads are doing the jobs.
 * constructing and destructing a threadpool is not cheap, but adding jobs and doing the work in them is relatively cheap.
 */
class ThreadPool : public osg::Referenced {

	public:
		
		/** base class for a job, inherited classes must implement the operator()(), which  gets called by the threads when the work should be done */
		class Job : public osg::Referenced {
			protected:
				template<class T, class JobClass> class AnimateCallOnMainThread : public cefix::AnimationBase {
				
					public:
						AnimateCallOnMainThread(T* t, void(T::*fpt)(JobClass*), JobClass* job) 
						:	cefix::AnimationBase(),
							_t(t),
							_fpt(fpt),
							_job(job)
						{}
					protected:
						virtual void animate(float elapsed) 
						{
							(*_t.*_fpt)(_job.get());
							setFinishedFlag();
						}
						
						osg::ref_ptr<T> _t;
						void(T::*_fpt)(JobClass*);
						osg::ref_ptr<JobClass>	_job;
				};
				
				
				/// ctor
				Job() : osg::Referenced() {}
				
				template<class T, class JobClass>void callOnMainThread(T* t, void(T::*fpt)(JobClass*), JobClass* job ) {
				
					osg::ref_ptr<cefix::AnimationFactory> fac = cefix::AnimationFactory::instance();
                    if (fac) fac->add(new AnimateCallOnMainThread<T, JobClass>(t, fpt, job));
					
				}
			
			public:
				/// called when work should be done
				virtual void operator()() = 0;
				
				inline void setPerThreadUserData(osg::Referenced* data) { _perThreadUserData = data; }
				
			protected:
				
				osg::Referenced* getPerThreadUserData() { return _perThreadUserData; }
				
			    /// dtor
				virtual ~Job() {}
			private:
				osg::Referenced*	_perThreadUserData;
		};
		
		/** a worker thread does one job after another. afterfinishing a job, the thread aks the ppol for a new job. 
		  * if none is available, it waits for anotification by the pool, when a new job is available.
		  */
		class WorkerThread : public osg::Referenced, public OpenThreads::Thread {
		
			public:
				/// ctor
				WorkerThread(ThreadPool* pool);
				
				/// run
				virtual void run();
				
				/// get the current job, NULL; if nothing to do
				inline Job* getCurrentJob() const { return _currentJob.get(); }
				
				/// sets the finished-flag, the thread will terminate
				inline void setFinishedFlag() {
					OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_waitingMutex);
					_mayFinish = true; 
					_block.release();  
				}
				
				/// start working again, a new job might be available
				inline void continueWork();
				
				/// return true, if the thread has finished
				inline bool isFinished() const { return _isFinished; } 
				
				/// return true, if the thread is waiting for a new job
				inline bool isWaiting() const { return _isWaiting; }
				
				
				void setUserData(osg::Referenced* data) { _userData = data; }
			
			protected:
				/// sets the waiting flag
				inline void setWaitingFlag(bool f) {
					{
						OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_waitingMutex);
						_isWaiting = f;
						if (_isWaiting) {
							_pool->addWaitingThread(this);
							_block.reset();
						}
					}
					
					if (f) {
						_block.block();
					} else {
						_block.release();
					}
					
				}
			
				
			private:
				ThreadPool*			_pool;
				osg::ref_ptr<Job>	_currentJob;
				bool				_mayFinish;
				OpenThreads::Block	_block;
				unsigned int		_id;
				bool				_isFinished, _isWaiting;
				OpenThreads::Mutex			_waitingMutex;
				osg::ref_ptr<osg::Referenced> _userData;
			
		};
		
		typedef std::deque< osg::ref_ptr<Job> > JobDeque;	
		typedef std::vector< osg::ref_ptr<WorkerThread> > ThreadVector;
		typedef std::map<WorkerThread*, bool> WaitingThreadsMap;	
		typedef std::vector< osg::ref_ptr<Job> > FinishedJobsVector;
        
        class Visitor {
        public:
            enum JobState { JobFinished, JobInProgress, JobWaiting };
            
            virtual void operator()(Job* job, JobState state) = 0;
            virtual ~Visitor() {}
        
        };
        
        
		/* ctor
		 * @param numWorkerThreads # threads to create
		 */ 
		ThreadPool(unsigned int numWorkerThreads, bool finishWorkBeforeDestructing = true );
		
		/// wait till all obs an threads are finished
		void waitUntilDone();
		
		/// add a new job to the queue
		void addJob(Job* job);
		
		/// returns true, if all jobs are done
		bool done();
		
		/// return the number of remaining jobs
		unsigned int getRemainingJobs() const; 
		
		/// get the number of waiting threads
		unsigned int getNumWaitingThreads() const { return _numWaitingThreads; }
		
		
		/// clear all pending jobs
		void flush() 
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
			_jobs.clear();
		}
		
		/// cancel operation on all threads, clears the queue of pending jobs.
		void cancel() {
			flush();
			releaseWorkerThreads();
		}
		
		/// get the vector of all worker threads
		const ThreadVector& getThreads() const { return _pool; }
		
		/// dtor
		virtual ~ThreadPool();
		
		static void setNotifySeverity(osg::NotifySeverity s) { _s_notifySeverity = s;}
		inline static osg::NotifySeverity getNotifySeverity() { return _s_notifySeverity; }

		inline void startPausing() { _paused = true; }
		inline void stopPausing() { _paused = false; checkWaitingThreads(); }
		
		inline unsigned int getNumThreads() { return _pool.size(); }
		
		void setUserDataForThread(unsigned int thread_ndx, osg::Referenced* data) { _pool[thread_ndx]->setUserData(data); }
		
		/// if you set the flag to false, the threadpool will cancel all operations in its destructor, 
		/// if set to true, the dtor will wait, until all operations are done
		void setFinishWorkBeforeDestructingFlag(bool f) { _finishWorkBeforeDestructing = f; }
		
		void setCollectFinishedJobs(bool b) { _collectFinishedJobs = b ; } 
		
		FinishedJobsVector& getFinishedJobs() { return _finishedJobs; }
		const FinishedJobsVector& getFinishedJobs() const { return _finishedJobs; }
		
		void clearFinishedJobs() { _finishedJobs.clear(); }
        
        void accept(Visitor& visitor);
	protected:
		void checkWaitingThreads();
		/// returns an available job or NULL, if non is available
		Job* getAvailableJob() {
			if (_paused) return NULL;
			osg::ref_ptr<Job> job = NULL;
			{
				OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
				if (_jobs.size() == 0)
					return NULL;
				job = _jobs.front();
				_jobs.pop_front();
			}
			return job.release();
		}
		
		/// release worker threads, they will terminate in the near future.
		void releaseWorkerThreads();
		
		/// called by a thread to signal, that the thread is ready to start working.
		void threadIsReady() { _startBarrier.block(_numThreads+1); }
		
		/// adds a thread to the waiting list
		void addWaitingThread(WorkerThread* w) 
		{
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_waitingMutex);	
			_waitingThreads[w] = true;
			_numWaitingThreads = _waitingThreads.size();
		}
		
		void waitForThreadFinishing();
		
		void addFinishedJob(Job* job);
	private:
					
		JobDeque			_jobs;
		ThreadVector		_pool;
		mutable OpenThreads::Mutex	_mutex;
		mutable OpenThreads::Mutex	_waitingMutex;
		mutable OpenThreads::Mutex	_finishedJobMutex;
		unsigned int _numWaitingThreads, _numThreads;
		OpenThreads::Barrier	_startBarrier;
		WaitingThreadsMap		_waitingThreads;
		bool					_paused;
		bool					_finishWorkBeforeDestructing;
		bool					_collectFinishedJobs;
		FinishedJobsVector		_finishedJobs;

		static osg::NotifySeverity _s_notifySeverity;
		
	friend class WorkerThread;


};

std::ostream& operator<<(std::ostream& outstream, const ThreadPool& pool);

}


#endif