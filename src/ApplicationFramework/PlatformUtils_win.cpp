//
//  PlatformUtils.cpp
//  cefixSketch
//
//  Created by Stephan Huber on 07.08.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "PlatformUtils.h"

#ifdef WIN32

namespace cefix {

PlatformUtils::NSAutoreleasePoolHelper::NSAutoreleasePoolHelper()
:   _pool(NULL)
{
}



PlatformUtils::NSAutoreleasePoolHelper::~NSAutoreleasePoolHelper()
{
}


NSString* PlatformUtils::toNSString(const std::string& str) {
    return NULL;
}

std::string PlatformUtils::toString(NSString* str) {
    return std::string();
}

}
#endif