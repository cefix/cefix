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

#ifndef CAMERA_REPRESENTATION_HEADER_
#define CAMERA_REPRESENTATION_HEADER_

#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <cefix/Export.h>

namespace cefix {

/** 
 * This class implements a visual camera representation in the scenegraph. you can change the visibility of this camera 
 * and/or attach it to a CameraManipulator to view the scene through this camera. This geode should represent all 
 * parameters of a camera, the position, rotation, etc.
 */
class CEFIX_EXPORT ViewPointNode: public osg::Group {

    public:
        /**default constructor, creates the geometry */
        ViewPointNode(float scale = 1.0f) : osg::Group(), _switchVisibilityAutomatically(true) { createGeometry(scale); }
        
        /** switch the visibility on or off */
        void switchVisibility(bool visible) { setNodeMask((visible) ? 1:0); }
        
        /** called from camera-manipulator to determine if the representation may be switched on/off */
        bool maySwitchVisibility() { return _switchVisibilityAutomatically; }
        /** sets the flag to determine if a camera may switch on/off this representation */
        void allowSwitchVisibility(bool flag) { _switchVisibilityAutomatically = flag; }
        
    protected:
        void createGeometry(float scale = 1.0f);
        
        bool _switchVisibilityAutomatically;
		
		virtual ~ViewPointNode() {}
    

};

}

#endif