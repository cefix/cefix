/*
 *  CoreVideoAdapter.h
 *  cefix
 *
 *  Created by Stephan Huber on 06.11.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef CORE_VIDEO_ADAPTER_HEADER
#define CORE_VIDEO_ADAPTER_HEADER

#include <cefix/Export.h>
#if defined (CEFIX_CORE_VIDEO_AVAILABLE)

#include <cefix/QtUtils.h>
#include <cefix/MovieData.h>
#include <cefix/Video.h>
#include <CoreVideo/CoreVideo.h>
#include <Quicktime/Quicktime.h>

namespace cefix {

class CoreVideoAdapter : public osg::Referenced {
	
	public:
		CoreVideoAdapter(osg::State& state, osg::Image* image);
		
		
		void setVideo(osg::Image* image);
		
		void setTimeStamp(const CVTimeStamp* ts) {_timestamp = ts; getFrame();}
		bool getFrame();
		
		inline GLenum getTextureName() { return _currentTexName; }
		inline GLenum getTextureTarget() { return _currentTexTarget; }
        
        QTVisualContextRef getVisualContext() { return _context; }
	
		virtual ~CoreVideoAdapter();
		
	private:
		osg::ref_ptr<Video>             _video;
		QTVisualContextRef				_context;
		const CVTimeStamp*				_timestamp;
		CVOpenGLTextureRef				_currentFrame;
		GLint							_currentTexName;
		GLenum							_currentTexTarget;

};


}




#endif
#endif