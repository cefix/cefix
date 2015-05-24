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

#include <osgGA/GUIEventHandler>
#include <cefix/Pickable.h>
#include <cefix/WindowPickable.h>

namespace cefix {

/**
 *  osgGA event-handler wjich intersect the current scene for every pointer andfires various 
 *  events on registered pickables
 */
 
class PickEventHandler : public osgGA::GUIEventHandler {
public:
    typedef std::set<Pickable*> PickableSet;
    
    /// various permanent data for every Pointer
    struct PickStack {
        typedef std::map<float, Pickable*> Stack;
        
        Stack stack;
        Pickable::PointerData pt;
        
        PickableSet currentlyDown, currentlyOver;
        
        bool down, ignoreHandledFlag;
        
        PickStack() : stack(), pt(), currentlyDown(), currentlyOver(), down(false), ignoreHandledFlag(false) {}
    };
    
    typedef std::map<Pickable::PointerIdType, PickStack> IdStackMap;
    
    PickEventHandler(osg::Camera* camera);
    ~PickEventHandler();
    
    /// enable multi-touch-picking
    void setMultiTouchEnabled(bool b) { _handleMultiTouch = b; }
    bool isMultiTouchEnabled() const { return _handleMultiTouch; }
    
    /// enable handling the over-state (makes only sense for mouse-input)
    void setHandleOverState(bool b) { _handleOverState = b; }
    bool getHandleOverState() const { return _handleOverState; }
    
    void setEnabled(bool b) { _enabled = b; }
    bool isEnabled() const { return _enabled; }
    
    /** enable/disable hierarchival picking, if enabled the parent nodes of a
     *  picked geode are queried until a valid pickable is found
     */
    void setHierarchicalPickingEnabled(bool b) { _hierarchicalPickingEnabled = b; }
    bool isHierarchicalPickingEnabled() const { return _hierarchicalPickingEnabled; }
    
    /// set the traversal mask
    void setTraversalMask(osg::Node::NodeMask mask) { _traversalMask = mask; }
    osg::Node::NodeMask getTraversalMask() const { return _traversalMask; }
    
    /// handle the osgGA::GUIEventAdapter
    virtual bool handle (const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *, osg::NodeVisitor *);
    
    /// get the window-pickable
    WindowPickable* getWindowPickable() const { return _windowPickable; }
    
    /// set the window-pickable
    void setWindowPickable(WindowPickable* pickable) { _windowPickable = pickable; }
protected:

    bool isCameraChild(const osg::NodePath& path);
    
    void intersect(const osgGA::GUIEventAdapter& ea, Pickable::PointerData& pt);
    bool fireEvents();
    void clearPicks();
    
    void addPick(const Pickable::PointerData& pt, const Pickable::Hit& hit, Pickable* picked);
    
    void registerPickable(Pickable* pickable);
    void deregisterPickable(Pickable* pickable);
    
    Pickable* getPickable(osg::Node* node)
    {
        Pickable* p = dynamic_cast<Pickable*>(node);
        if (!p) p = dynamic_cast<Pickable*>(node->getUserData());
        if (p && (p->isEnabled() == false)) p = NULL;
        return p;
    }
    
    PickableSet _registeredPickables, _currentlyPicked, _pickedLastFrame;
    bool        _handleMultiTouch, _handleOverState, _enabled, _hierarchicalPickingEnabled;
    osg::Node::NodeMask _traversalMask;
    osg::Camera* _camera;
    
    IdStackMap _stacks;
    osg::ref_ptr<WindowPickable> _windowPickable;
    
friend class Pickable;
};

}