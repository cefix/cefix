/*
 *  ThreadedFilterApplier.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 13.04.11.
 *  Copyright 2011 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "ThreadedFilterApplier.h"

namespace cefix {

ThreadedFilterApplier::ThreadedFilterApplier(unsigned int num_threads, unsigned int num_jobs)
:   _numThreads( num_threads == 0 ? OpenThreads::GetNumberOfProcessors() : num_threads ),
    _numJobs (num_jobs == 0 ? _numThreads : num_jobs),
    _pool(NULL),
    _fa()
{
    _pool = new ThreadPool(_numThreads);
    //_pool->setNotifySeverity(osg::ALWAYS);
}
 
 
}