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

#pragma once

#ifdef __OBJC__
@class NSString;
@class NSAutoreleasePool;
#else
class NSString;
class NSAutoreleasePool;
#endif


#include <string>


namespace cefix {

class PlatformUtils {
public:
    
    class NSAutoreleasePoolHelper {
    public:
        NSAutoreleasePoolHelper();
        ~NSAutoreleasePoolHelper();
    private:
        NSAutoreleasePool* _pool;
    };
    
    static NSString* toNSString(const std::string& str);
    
    static std::string toString(NSString* str);
    
private:
    PlatformUtils() {}

};

}