//
//  SharedThreadPool.h
//  cefix-for-iphone
//
//  Created by Stephan Huber on 15.11.11.
//  Copyright (c) 2011 Digital Mind. All rights reserved.
//

#pragma once

#include <cefix/ThreadPool.h>

namespace cefix {
    /** 
     * a shared thread pool for general usage, creates a threadpool with x threads, where x is the number of processors - 1, 
     * if not specified explicitely via setNumThreads()
     */
    class SharedThreadPool {
    public:
        static ThreadPool* instance();
        static void setNumThreads(unsigned int num_threads);
    private:
        SharedThreadPool() {}
    };
}