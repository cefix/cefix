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

#include <cefix/AlterableController.h>
#include <cefix/GroupWidget.h>

namespace cefix {

/// An AlterableController-implementation, creates for every Alterable-group a drawer with widgets
class WidgetsAlterableControllerImplementation : public AlterableControllerImplementation {
public:
    /// ctor
    WidgetsAlterableControllerImplementation(float group_width = 200, float group_gap = 30, float dy = 0, float start_z = 900);
    
    /// implementation of rebuild, recreates all widgets
    virtual void rebuild();
    
    /// get the scenegraph-node for the created widgets
    osg::Node* getNode() { return _group; }
    
private:
    osg::ref_ptr<osg::Group> _group;
    osg::ref_ptr<cefix::GroupWidget> _widgets;
    float _groupWidth, _groupGap, _dy, _startZ;
    std::vector<osg::ref_ptr<osg::Referenced> > _responderHelper;
};


}