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


#ifndef CEFIX_THREADED_FILTER_APPLIER_HEADER
#define CEFIX_THREADED_FILTER_APPLIER_HEADER

#include <cefix/FilterApplier.h>
#include <cefix/ThreadPool.h>
#include <cefix/Pixel.h>

namespace cefix {

class ThreadedFilterApplier {

public:

	class PixelFunctor {
	public:
		void setUserData(osg::Referenced* ref) {}
		void operator()(cefix::Pixel& input, cefix::Pixel& output, int x, int y);
		void finish() {}
		
		virtual ~PixelFunctor() {} // because of multiple inheritance
	};

    typedef FilterApplier::Region Region;
    typedef FilterApplier::ImageVector ImageVector;
    template <class FilterClass>
    class FilterApplierJob : public ThreadPool::Job {
    public:
        FilterApplierJob(FilterApplier* fa, osg::Image* source, osg::Image* dest, FilterClass* afilter, Region* region, bool hor, bool vert)
        :   ThreadPool::Job(),
            _fa(fa),
            _source(source), 
            _dest(dest),
            _filter(afilter),
            _region(region),
            _hor(hor),
            _vert(vert)
        {
        }
        
        virtual void operator()() 
        {
            _fa->apply<FilterClass>(_source.get(), _dest.get(), _filter.get(), _region.get(), _hor, _vert);
        }
        
    private:
        FilterApplier* _fa;
        osg::ref_ptr<osg::Image> _source, _dest;
        osg::ref_ptr<FilterClass> _filter;
        osg::ref_ptr<Region> _region;
        bool _hor, _vert;
    };
	
	
	template <class CustomPixelFunctor>
	class CustomPixelOperationJob  : public ThreadPool::Job, public CustomPixelFunctor {
	public:
		CustomPixelOperationJob(osg::Image* source, osg::Image* dest, Region* region)
		:	ThreadPool::Job(),
			_source(source),
			_dest(dest),
			_region(region)
		{
		}
		
		virtual void operator()() {
			cefix::Pixel source_pixel(_source);
			cefix::Pixel dest_pixel(_dest);
			for(int y = _region->top; y < _region->bottom; ++y) {
				for(int x = _region->left; x < _region->right; ++x) {
					static_cast<CustomPixelFunctor*>(this)->operator()(source_pixel, dest_pixel, x, y);
				}
			}
		}
	private:
		osg::ref_ptr<osg::Image> _source, _dest;
		osg::ref_ptr<Region> _region;
	};

    
    ThreadedFilterApplier(unsigned int num_threads, unsigned int num_jobs = 0);
            
    template<class FilterClass> 
    void apply(osg::Image* source, osg::Image* dest, FilterClass* afilter = NULL, bool hor = true, bool vert = true) 
    {
        unsigned int start_y = 0;
        unsigned int height = std::ceil(source->t() / static_cast<float>(_numJobs));
        
        // std::cout << "image size: " << source->s() << " x " << source->t() << std::endl;
        for(unsigned int i = 0; i < _numJobs; ++i) {
            Region* region = new Region(source);
            region->top = start_y;
            region->bottom = std::min<unsigned int>(source->t(), start_y + height);
            // std::cout << "filtering from " << region->left << "/" << region->top << " -> " << region->right << "/" << region->bottom << std::endl;
            _pool->addJob(new FilterApplierJob<FilterClass>(&_fa, source, dest, afilter, region, hor, vert));
            start_y += height;
        }
        
        _pool->waitUntilDone();
        dest->dirty();
    }
    
    template<class FilterClass>
    void apply(ImageVector& images, FilterClass* afilter = NULL, bool hor = true, bool vert = true) 
    {
        osg::ref_ptr<osg::Image> result(NULL);
        
        for(ImageVector::iterator i = images.begin(); i != images.end(); ++i) 
        {
            result = osg::clone(i->get());
            this->apply<FilterClass>(*i, result, afilter, hor, vert);
            memcpy((*i)->data(), result->data(), result->getTotalSizeInBytes());
            (*i)->dirty();
        }
    }
	
	
	template<class CustomPixelFunctor> 
    void applyPixelFunctor(osg::Image* source, osg::Image* dest = NULL, osg::Referenced* user_data = NULL) 
    {
        unsigned int start_y = 0;
        unsigned int height = std::ceil(source->t() / static_cast<float>(_numJobs));
        _pool->setCollectFinishedJobs(true);
        // std::cout << "image size: " << source->s() << " x " << source->t() << std::endl;
        for(unsigned int i = 0; i < _numJobs; ++i) 
		{
            Region* region = new Region(source);
            region->top = start_y;
            region->bottom = std::min<unsigned int>(source->t(), start_y + height);
            // std::cout << "filtering from " << region->left << "/" << region->top << " -> " << region->right << "/" << region->bottom << std::endl;
            CustomPixelOperationJob<CustomPixelFunctor>* job = new CustomPixelOperationJob<CustomPixelFunctor>(source, dest, region);
			job->setUserData(user_data);
			_pool->addJob(job);
            start_y += height;
        }
        
        _pool->waitUntilDone();
        if (dest) dest->dirty();
		
		if(user_data) 
		{
			ThreadPool::FinishedJobsVector& finished_jobs = _pool->getFinishedJobs();
			for(ThreadPool::FinishedJobsVector::iterator i = finished_jobs.begin(); i != finished_jobs.end(); ++i) 
			{
				dynamic_cast<CustomPixelFunctor*>(i->get())->finish();
			}
		}
		_pool->setCollectFinishedJobs(false);
    }
    
private:
    unsigned int _numThreads, _numJobs;
    osg::ref_ptr<cefix::ThreadPool> _pool;
    FilterApplier _fa;

};

}

#endif