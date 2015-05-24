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


#ifndef QUICKTIME_MOVIE_EXPORTER_HEADER_
#define QUICKTIME_MOVIE_EXPORTER_HEADER_

#include <cefix/Export.h>

#if defined CEFIX_QUICKTIME_AVAILABLE

#include <osg/Referenced>
#include <OpenThreads/Thread>
#include <list>
#include <map>
#include <osg/Image>
#include <osg/Camera>
#include <OpenThreads/Thread>
#include <osg/observer_ptr>
#include <osg/Timer>

namespace cefix {

/** this class helps you to "export" a viswible scene to a quicktime-movie. You can use this class also to create 
 *  quicktime-movies out of your own images. Please feed this class with RGB images, with no alpha-channel.
 */
class CEFIX_EXPORT QuicktimeMovieExporter : public osg::Referenced, public OpenThreads::Thread {
	private:
		typedef std::pair<osg::ref_ptr<osg::Image>, double> ImagePair;
		typedef std::list< ImagePair > ImageQueue;
		
		class PrivateData;
		
	public:
		/** postDrawCallback for saving the currently rendered image of a specific camera and add it to the exporter
		 */
		class PostDrawCallback : public osg::Camera::DrawCallback {
			public:
				/** ctor */
				PostDrawCallback(QuicktimeMovieExporter* exporter, float framesPerSecond = 25, bool everyFrame = false) :
					_exporter(exporter),
					_enabled(false),
					_fps(framesPerSecond),
					_everyFrame(everyFrame)
				{
				}
				
								
				/** sets the enabled flag, if enabled the callback will capture the currently rendered image and 
				 *  add it to the exporter */
				void setEnabledFlag(bool flag) {
					_enabled = flag;
					_lastCapturedTimeStamp = _t.tick();
				}
				/** callback-operator -- do the work */
				virtual void operator()(const osg::Camera& camera) const;
				
			private:
				osg::ref_ptr<QuicktimeMovieExporter>	_exporter;
				bool										_enabled;
				float										_fps, _everyFrame;
				osg::Timer									_t;
				mutable osg::Timer_t								_lastCapturedTimeStamp;
				
		};
		
		/** ctor
		    @param filename filename to store the movie in
			@param w width of the movie
			@param h height of the movie
			@param fps frames per second of the movie
		*/
		QuicktimeMovieExporter(std::string filename, unsigned int w, unsigned int h, float fps = 25.0f, bool verbose = false);
		
		/** adds an image with duration */
		void addImage(osg::Image* image, double duration) {
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
			if (_inited) 
				_imageQueue.push_back(ImagePair(image,duration));
		}
		
		/** adds an image with default duration */
		void addImage(osg::Image* image) {
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
			if (_inited) 
				_imageQueue.push_back(ImagePair(image,1/_fps));
		}
		
		/** @return the nuber of images to process */
		unsigned int getRemainingItems() {
			OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
			return _imageQueue.size();
		}
		
		/** do the work */
		virtual void run();
		
		/** stops the thread an wait until thread is finished */
		void stop();
		
		/** @return true, if the thread is finished */
		bool isFinished() const { return _isFinished; }
		
		/** sets the amount of frames to save into memory before saving them to disk */
		void setPreloadFrameCount(unsigned int p) { _preloadFrameCount = p; }
        
        /** set the max num of images stored concurrently in RAM, the render threads pauses, 
         *  if more images are in the queue
         */
        void setMaxConcurrentImages(unsigned int p) { _maxConcurrentImages = p; }
        unsigned int getMaxConcurrentImages() const { return _maxConcurrentImages; }

		void setVerbose(bool b);
		
		void setUseAlphaFlag(bool use_alpha);
		bool getUseAlphaFlag() const { return _useAlpha; }

	protected:
		virtual ~QuicktimeMovieExporter();
	
	private:
		ImageQueue			_imageQueue;
		OpenThreads::Mutex	_mutex;
		PrivateData*		_data;
		bool				_isFinished;
		float				_fps;
		bool				_inited, _wantToQuit;
		unsigned int		_preloadFrameCount;
		bool				_verbose, _useAlpha;
        unsigned int        _maxConcurrentImages;
		
};

}

#endif
#endif