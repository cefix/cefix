/*
 *  Version.cpp
 *  cefix-for-iphone
 *
 *  Created by Stephan Huber on 06.05.11.
 *  Copyright 2011 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */


#include <cefix/Version.h>
#include <osg/Version>
#include <stdio.h>
#include <sstream>

const char* cefixGetVersion()
{
    static char cefix_version[256];
    static int cefix_version_init = 1;
    if (cefix_version_init)
    {
        sprintf(cefix_version,"%d.%d.%d",CEFIX_MAJOR_VERSION, CEFIX_MINOR_VERSION,CEFIX_PATCH_VERSION);
        cefix_version_init = 0;
    }
    
    return cefix_version;
}


std::string cefixGetVersionDetailed() {
    
    std::ostringstream ss;
    ss << "cefix v." << cefixGetVersion();
    #ifdef CEFIX_FOR_64BIT
        ss << ", 64bit";
    #else
        ss << ", 32bit";
    #endif
    
    #ifdef CEFIX_FOR_IPHONE
        ss << ", ios";
    #endif
    
    #ifdef CEFIX_QUICKTIME_AVAILABLE
        ss << ", quicktime";
    #endif
    
    #ifdef CEFIX_QT_KIT_AVAILABLE
        ss << ", QT Kit";
    #endif
    
    #ifdef CEFIX_CORE_VIDEO_AVAILABLE
        ss << ", CoreVideo";
    #endif
    
    #ifdef CEFIX_AV_FOUNDATION_AVAILABLE
        ss << ", AvFoundation";
    #endif
    
    ss << ", OpenSceneGraph v." << osgGetVersion();
    
    return ss.str();
}