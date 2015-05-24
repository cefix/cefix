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

#ifndef _MOUSE_LOCATION_PROVIDER_HEADER_
#define _MOUSE_LOCATION_PROVIDER_HEADER_

#include <osg/Vec3>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <cefix/Log.h>

#include <cefix/Export.h>
#include <cefix/NodePathUtils.h>
#include <cefix/WindowPickable.h>

namespace cefix {

    /**
     * this class provides an interface for getting the current mouse-coordinate
     * this class is deprecated, please use WindowPickable instead.
     */
    class CEFIX_EXPORT MouseLocationProvider : public WindowPickable {
    
        public:
            
            /// @return a reference to the singleton
            static WindowPickable* instance();
            
    };
    
    
}

#endif