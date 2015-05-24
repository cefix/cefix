//
//  SysUtils.mm
//  cefix
//
//  Created by Stephan Maximilian Huber on 17.03.12.
//  Copyright (c) 2012 stephanmaximilianhuber.com. All rights reserved.
//

#include <iostream>
#include <cefix/SysUtils.h>

#ifdef CEFIX_FOR_IPHONE
#import <UIKit/UIKit.h>
#else
#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#endif
#endif

namespace cefix {

#ifdef __APPLE__

class AutoReleasePoolHelper {
public:
    AutoReleasePoolHelper() {
        pool = [[NSAutoreleasePool alloc] init];
    }
    
    ~AutoReleasePoolHelper() { [pool release]; }
private:
    NSAutoreleasePool* pool;
};

#ifdef CEFIX_FOR_IPHONE

//ios specific

bool SysUtils::isWindows() { return false; }
bool SysUtils::isMac() { return false; }
bool SysUtils::isIOS() { return true; }

bool SysUtils::isIphone() 
{ 
    AutoReleasePoolHelper arph;
    return ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPhone); 
}

bool SysUtils::isIpad() 
{ 
    AutoReleasePoolHelper arph;
    return ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad);
}


float SysUtils::getUIScale(const osg::GraphicsContext::ScreenIdentifier& si) 
{ 
    AutoReleasePoolHelper arph;
    UIScreen* screen = [[UIScreen screens] objectAtIndex:si.screenNum];
    
    return (([screen respondsToSelector:@selector(scale)]) ? [screen scale] : 1.0f);
}

#else

//mac-specific

bool SysUtils::isWindows() { return false; }
bool SysUtils::isMac() { return true; }
bool SysUtils::isIOS() { return false; }
bool SysUtils::isIphone() { return false; }
bool SysUtils::isIpad() { return false; }
float SysUtils::getUIScale(const osg::GraphicsContext::ScreenIdentifier& si) { return 1.0f; }
#endif
#else

// win-specific

bool SysUtils::isWindows() { return true; }
bool SysUtils::isMac() { return false; }
bool SysUtils::isIOS() { return false; }
bool SysUtils::isIphone() { return false; }
bool SysUtils::isIpad() { return false; }
float SysUtils::getUIScale(const osg::GraphicsContext::ScreenIdentifier& si) { return 1.0f; }

#endif

}