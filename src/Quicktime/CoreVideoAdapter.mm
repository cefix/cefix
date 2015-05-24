/*
 *  CoreVideoAdapter.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 06.11.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/Export.h>
#if defined (CEFIX_CORE_VIDEO_AVAILABLE)

#include "CoreVideoAdapter.h"
#include <osg/GL>
#include <osg/State>
#include <osgViewer/api/Cocoa/GraphicsWindowCocoa>
#import <Cocoa/Cocoa.h>

namespace cefix {



// ----------------------------------------------------------------------------------------------------------
// ctor CoreVideoAdapter
// ----------------------------------------------------------------------------------------------------------

CoreVideoAdapter::CoreVideoAdapter(osg::State& state, osg::Image* image) :
	osg::Referenced(),
	_context(NULL),
	_timestamp(NULL),
	_currentFrame(NULL),
	_currentTexTarget(GL_TEXTURE_RECTANGLE_EXT)
{
	setVideo(image);
	if (!_video.valid())
		return;
	
    
	CGLContextObj cglcntx(NULL);
	CGLPixelFormatObj cglPixelFormat;
	OSStatus err = noErr;
	    
    if (cglcntx == NULL) {
        osgViewer::GraphicsWindowCocoa* win = dynamic_cast<osgViewer::GraphicsWindowCocoa*>(state.getGraphicsContext());
        if (win) 
        {
			NSOpenGLContext* context = win->getContext();
			cglcntx = (CGLContextObj)[context CGLContextObj];
            cglPixelFormat = (CGLPixelFormatObj)[ win->getPixelFormat() CGLPixelFormatObj];
        }
    }
    
    
	if ((cglcntx == NULL) || (err != noErr)) {
		log::error("CoreVideoTexture") << "could not get Context/Pixelformat " << err << std::endl;
		return;
	}
	
	CFTypeRef keys[] = { kQTVisualContextWorkingColorSpaceKey };
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	CFDictionaryRef textureContextAttributes = CFDictionaryCreate(kCFAllocatorDefault,
                                                                  (const void **)keys,
                                                                  (const void **)&colorSpace, 1,
                                                                  &kCFTypeDictionaryKeyCallBacks,
                                                                  &kCFTypeDictionaryValueCallBacks);
    
    // create a new OpenGL texture context for quicktime to render into from a specified OpenGL context and pixel format
	err = QTOpenGLTextureContextCreate(kCFAllocatorDefault, cglcntx, cglPixelFormat, textureContextAttributes, &_context);

	setVideo(_video.get());
	// QTVisualContextSetImageAvailableCallback(_context, MyQTVisualContextImageAvailableCallback, (void*)this);
	setTimeStamp(NULL);
}


// ----------------------------------------------------------------------------------------------------------
// dtor
// ----------------------------------------------------------------------------------------------------------

CoreVideoAdapter::~CoreVideoAdapter() {

	setVideo(NULL);
	
	if (_currentFrame) {
    	CVOpenGLTextureRelease(_currentFrame);
        _currentFrame = NULL;
    }

	// release the OpenGL Texture Context
    if (_context) {
    	 CFRelease(_context);
         _context = NULL;
    }

}


// ----------------------------------------------------------------------------------------------------------
// setVideo
// ----------------------------------------------------------------------------------------------------------

 void CoreVideoAdapter::setVideo(osg::Image* image) {
	if (_video.valid()) {
		_video->setCoreVideoAdapter(NULL);
	}
	_video = dynamic_cast<Video*>(image);
	
    if ((_context) && (_video.valid()))
    {
		_video->setCoreVideoAdapter(this);
		setTimeStamp(NULL);
	}	
}

// ----------------------------------------------------------------------------------------------------------
// getFrame
// ----------------------------------------------------------------------------------------------------------

bool CoreVideoAdapter::getFrame() { 
	QTVisualContextTask(_context);
	bool b = QTVisualContextIsNewImageAvailable(_context, _timestamp);
	if (b){
		
		CVOpenGLTextureRef newFrame;
		QTVisualContextCopyImageForTime(_context, kCFAllocatorDefault, _timestamp, &newFrame);
		
		if (_currentFrame) CVOpenGLTextureRelease(_currentFrame);
		_currentFrame = newFrame;
		
		_currentTexTarget = CVOpenGLTextureGetTarget(_currentFrame);
		_currentTexName =  CVOpenGLTextureGetName(_currentFrame);
	}
	//std::cerr << _movie->getFileName() << ": " << b << " / " << _movie->isPlaying() << " " << _movie->getCurrentTime() << std::endl;
	return b;
}



}

#endif