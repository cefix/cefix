//
//  PlatformUtils.cpp
//  cefixSketch
//
//  Created by Stephan Huber on 07.08.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <cefix/PlatformUtils.h>

#ifdef __APPLE__
#if (TARGET_OS_IPHONE)
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

namespace cefix {



PlatformUtils::NSAutoreleasePoolHelper::NSAutoreleasePoolHelper()
{
    _pool = [[NSAutoreleasePool alloc] init];
}



PlatformUtils::NSAutoreleasePoolHelper::~NSAutoreleasePoolHelper()
{
    [_pool release];
}



NSString* PlatformUtils::toNSString(const std::string& str) {
    return [NSString stringWithUTF8String: str.c_str()];
}

std::string PlatformUtils::toString(NSString* str) {
    return [str UTF8String];
}

}

#else

namespace cefix {


NSString* PlatformUtils::toNSString(const std::string& str) {
    return NULL;
}

std::string PlatformUtils::toString(NSString* str) {
    return std::string();
}

}
#endif