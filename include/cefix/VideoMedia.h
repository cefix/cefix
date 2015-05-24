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

#ifndef QUICKTIME_MEDIA_HEADER_
#define QUICKTIME_MEDIA_HEADER_
#include <cefix/Export.h>
#include <osg/Referenced>
#include <string>
#include <list>
#include <cefix/VideoNewFrameCallback.h>

namespace cefix {


    /**
     * this class is an abstract class for Quicktime-Media. The idle-method is called by the VideoMediaManager every 
     * frame, so the media gets a chance to update itself
     * it also registers itself automagically with the VideoMediaManager
     */
    class CEFIX_EXPORT VideoMedia : public osg::Referenced{
    
        public:
            /** 
             * is called byt the QuicktimeMediaManager, here you should update your media
             */
            virtual void idle() = 0;
            /**
             * default constructor -- register this media with the mediamanager
             */
            VideoMedia();
            /**
             * the idle-method is only called, if isRunning returns true, so set this flag according to your needs
             * @return true, if the media should be updated
             */
            inline bool isRunning() const { return _isRunning;}
            /**
             * @return a unique id for this media
             */
            inline int ID() const { return _id;}
			
			/** @return the name of the media */
			virtual std::string getMediaTypeName() { return "unnamed media"; }
						
			inline void setRunning(bool b) { _isRunning = b; }
            
        protected:
            virtual ~VideoMedia();
            bool _isRunning;
            int _id;
			
	};
    
    

	template <class T>
	class VideoMediaT : public VideoMedia {
	
		public:
			VideoMediaT(T* t, const std::string& mediaTypeName) : VideoMedia(), _t(t) { setMediaTypeName(mediaTypeName); }
			virtual void idle() { _t->idle(); }
			
			virtual std::string getMediaTypeName() { return _mediaTypeName; }
			
			void setMediaTypeName(const std::string& name) { _mediaTypeName = name; }
		protected:
			virtual ~VideoMediaT() {}
		private:
			T* _t;
			std::string _mediaTypeName;
	};
	
	
	template <class T>
	class VideoMediaAdapter : public VideoFrameCallbackHandlerBase  {
	
		public:
			
			typedef VideoMediaAdapter<T> adapter_type;
			
			VideoMediaAdapter(const std::string& mediaTypeName) : VideoFrameCallbackHandlerBase(), _media(new VideoMediaT<adapter_type>(this, mediaTypeName)) {};
			
			inline void idle() { (static_cast<T*>(this))->idle(); }
			
			bool isRunning() const { return (_media.valid()) ? _media->isRunning() : false; }
            
		protected:
			inline void setRunning(bool b) { _media->setRunning(b); }
            
            void clear() { _media = NULL; }
			
		private:
			osg::ref_ptr<VideoMediaT<adapter_type> > _media;

	
	};

}

#endif
