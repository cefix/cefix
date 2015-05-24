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
 
#ifndef CEFIX_EXPORT_
#define CEFIX_EXPORT_

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
	#  if defined( CEFIX_LIBRARY_STATIC )
    #    define CEFIX_EXPORT
    #  elif defined( CEFIX_LIBRARY )
    #    define CEFIX_EXPORT   __declspec(dllexport)
    #  else
    #    define CEFIX_EXPORT   __declspec(dllimport)
    #  endif
#else
    #  define CEFIX_EXPORT
#endif

#ifdef _MSC_VER
#pragma warning (disable:4068)
#pragma warning (disable:4244)
#pragma warning (disable:4800)
#pragma warning (disable:4290)
#pragma warning (disable:4355)
#endif

#ifdef __APPLE__
#include <TargetConditionals.h>
#include <AvailabilityMacros.h>
    #if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
        #define CEFIX_FOR_IPHONE 1
        #define CEFIX_AV_FOUNDATION_AVAILABLE 1
    #else
    
        #if defined(MAC_OS_X_VERSION_10_8) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_8)
            #define CEFIX_AV_FOUNDATION_AVAILABLE 1
        #endif

        #if defined(MAC_OS_X_VERSION_10_6) && (!__LP64__) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_6)
            #define CEFIX_QT_KIT_AVAILABLE 1
        #endif

        #if defined (__APPLE__) && (!__LP64__) && (!defined(MAC_OS_X_VERSION_10_7))
        
            #define CEFIX_CORE_VIDEO_AVAILABLE 1
            #define CEFIX_QUICKTIME_AVAILABLE 1
        #endif
        
        #if defined (__LP64__)
            #define CEFIX_FOR_64BIT 1
        #endif
        
    #endif
#else
    #define CEFIX_QUICKTIME_AVAILABLE 1
#endif

#endif
