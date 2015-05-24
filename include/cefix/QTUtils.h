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

#ifndef QTUTILS_HEADER_
#define QTUTILS_HEADER_

#include <cefix/Export.h>

#if defined CEFIX_QUICKTIME_AVAILABLE

#include <osg/GL>

#ifdef __APPLE__
    #if defined (__APPLE__)  && (!__LP64__)
        #include <Quicktime/Quicktime.h>
        #include <Carbon/Carbon.h>
    #endif
#else
	#include <QTML.h>
	#include <Movies.h>
	#include <Quickdraw.h>
	#include <QDOffscreen.h>
	#include <QuicktimeComponents.h>
	#include <FixMath.h>
	#include <CGBitmapContext.h>
	#include <CGImage.h>
	#include <CGColorSpace.h>
	#include <ImageCompression.h>

	extern "C" {
		/** legacy function for Windows */
		inline void GetPortBounds(GWorldPtr gw, Rect* rect) {
			(*rect) = (gw->portRect);
		}
		/** legacy function for Windows */
		inline PixMapHandle GetPortPixMap (CGrafPtr port) {
			return port->portPixMap;
		}
 
	}

#define SetRect MacSetRect
#define OffsetRect MacOffsetRect

#endif

namespace cefix {
	/**
	 * inits Quicktime, if erase = true, the Quicktime-stuff gets cleaned, call it before your app exits
	 * @param erase true, if you want to cleanup quicktime-related stuff
	 */
	CEFIX_EXPORT void initQuicktime(bool erase = false);
	
	/** cleans up all quicktime-related stuff */
	CEFIX_EXPORT void exitQuicktime();


	/** constructs an FSSpec out of an path */
	CEFIX_EXPORT  OSStatus MakeFSSpecFromPath(const char* path, FSSpec& spec);

	
	/** opens a movie from a path */
	CEFIX_EXPORT  OSStatus MakeMovieFromPath(const char* path, Movie* movie);
	
	CEFIX_EXPORT  OSType getNativePixelFormatForQuicktime();
	
	CEFIX_EXPORT  GLenum getBestInternalFormatForQuicktime();
    

}
#endif
#endif