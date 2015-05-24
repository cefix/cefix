//
//  SharedThreadPool.cpp
//  cefix-for-iphone
//
//  Created by Stephan Huber on 15.11.11.
//  Copyright (c) 2011 Digital Mind. All rights reserved.
//

#include <cefix/SharedThreadPool.h>
#include <cefix/DataFactory.h>
#include <cefix/Log.h>

namespace cefix {

static osg::ref_ptr<ThreadPool> s_thread_pool = NULL;
static unsigned int s_num_threads = 0;

void cleanup_thread_pool() {
    s_thread_pool = NULL;
}

void SharedThreadPool::setNumThreads(unsigned int num_threads)
{
    s_num_threads = num_threads;
}

ThreadPool* SharedThreadPool::instance() 
{
    if (!s_thread_pool.valid()) 
    {
        unsigned int num_threads = cefix::DataFactory::instance()->getPreferencesFor("/num_threads_for_threadpool", s_num_threads);
        if (num_threads == 0)
            num_threads = std::max(1, OpenThreads::GetNumberOfProcessors()-1);
            
        s_thread_pool = new ThreadPool(num_threads);
        cefix::DataFactory::instance()->addCleanupHandler(&cleanup_thread_pool);
        cefix::log::info("SharedThreadPool") << "created shared ThreadPool with " << num_threads << " threads" << std::endl;
    }
    
    return s_thread_pool.get();
}


}