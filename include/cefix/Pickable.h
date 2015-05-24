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


#include <osgGA/GUIEventAdapter>
#include <osgUtil/LineSegmentIntersector>


namespace cefix {
class PickEventHandler;

class Draggable;
class WindowPickable;
/** 
 * this class defines a base class for all pickable objects. Pickables are part of the scene-graph either directly or via node->setUserData
 * the PickEventHandler picks the objects under all pointers and fires various events.
 */
class Pickable {
public:

    typedef osgUtil::LineSegmentIntersector::Intersection Hit;
    
    typedef int PointerIdType;
    
    /** data storing all needed data for one pointer */
    struct PointerData {
        enum Phase { UNKNOWN, DOWN, UP, DRAG, MOVE };
        
        /// the id of the pointer
        PointerIdType id;
        
        /// coordinates
        float x, y, revertedY;
        
        /// Phase
        Phase phase;
        
        /// downCount
        unsigned int downCount;
        
        /// event associated with this pointer
        osg::ref_ptr<osgGA::GUIEventAdapter> event;
        
        /// computed near and far point in world-coords
        osg::Vec3 nearPoint, farPoint;
        
        /// ctor
        PointerData(const osgGA::GUIEventAdapter& ea, const osgGA::GUIEventAdapter::TouchData::TouchPoint& tp);
        PointerData(const osgGA::GUIEventAdapter& ea);
        PointerData() : id(0), x(0), y(0), phase(UNKNOWN), downCount(0), event(NULL) {}
        
        inline osg::Vec2 asVec2() const { return osg::Vec2(x,revertedY); }
		inline osg::Vec2 asNativeVec2() const { return osg::Vec2(x,y); }
		
		inline osg::Vec3 asRay() const { return osg::Vec3(x, y, 1) - osg::Vec3(x, y, 0); }
		inline osg::Vec3 asTransformedRay(const osg::Matrix& m) const { return asLocalFar() * m - asLocalNear() * m; }
		inline osg::Vec3 asLocalNear() const { return osg::Vec3(x, y, 0); }
		inline osg::Vec3 asLocalFar() const { return osg::Vec3(x, y, 1); }
        
        static Phase getPhase(const osgGA::GUIEventAdapter::TouchPhase& touch_phase);
        static Phase getPhase(const osgGA::GUIEventAdapter::EventType& event_type);
        static PointerIdType getMousePointerId();
    };
    
    /// small struct encapsulating hit and pointer
    struct HitData {
        PointerData pointer;
        Hit hit;
        
        HitData(const PointerData& in_pt, const Hit& in_hit) : pointer(in_pt), hit(in_hit) {}
        HitData() : pointer(), hit() {}
    };
    
    typedef std::map<PointerIdType, HitData> HitMap;
    typedef std::vector<PointerIdType> PointerIdVector;
    
    /// ctor
    Pickable();
    
    /// dtor
    virtual ~Pickable();
    
    /// called when a sequence began, mostly occuring on the first down
    virtual void sequenceBegan() {}
    
    /// called when a sequence ended, when the last pointer is in the phase up
    virtual void sequenceEnded() {}
    
    /// a pointer is in phase down
    virtual void down() {}
    
    /// a pointer is in phase up
    virtual void up(bool inside) {}
    
    /// a pointer has entered the object (only available when the PickEventHandler handles over-states)
    virtual void enter() {}
    
    /// a pointer has left the object (only available when the PickEventHandler handles over-states)
    virtual void leave() {}
    
    /// a pointer is within the object (only available when the PickEventHandler handles over-states)
    virtual void within() {}
    
    /// handle is called for every picked pickable, regardless of event-bubbling
    virtual void handle() {}
    

    /// stop propagating the event, this will hinder Pickables "behind" the current pickables to get notified, addtionally it blocks standard osg-handlers
    void stopPropagation() { _stopPropagation = true; }
    
    /// allow event propagation, the event "bubbles up" to other pickables "behind" the current event, if all pickables allow propagation, other osg-handler can handle the event
    void allowPropagation() { _stopPropagation = false; }
    
    bool shouldPropagationBeStopped() const { return _stopPropagation; }
    
    /// set the enabled-flag
    void setEnabled(bool b) { _enabled = b; }
    
    /// is this pickable enabled?
    bool isEnabled() const { return _enabled; }
    
    /// accept a hit, if a subclass returns false, the hit is not added to this pickable, and no events get fired
    virtual bool accept(const Hit& hit) { return true; }
        
    /// get num pointers pickint this Pickable
    unsigned int getNumPointers() const { return _pointerIds.size(); }
    
    /// get the id of the last pointer, suitable to differentiate the pointers from within the up/down/within-event
    unsigned int getLastPointerId() const { return _lastPointerId; }
    
    /**
     * returns the id of the nth pointer (note, the ids are not sorted, so in one frame 
     * the first pointer may have the id 2 and on the next frame the pointer with id 2 is on slot 2
     */
    PointerIdType getNthPointerId(unsigned int ndx) const { return _pointerIds[ndx]; }
    
    /// returns a hit for a specific pointer
    const Hit& getHitById(PointerIdType ptid) const
    {
        HitMap::const_iterator i = _hits.find(ptid);
        return i->second.hit;
    }
    
    /// returns the hit for the nth pointer
    const Hit& getNthHit(unsigned int ndx) const {
        return getHitById(getNthPointerId(ndx));
    }
    
    /// get the current hit
    const Hit& getCurrentHit() const {
        return getHitById(getLastPointerId());
    }
    
    /// get the pointer-data for a given pointer id
    const PointerData& getPointerDataById(PointerIdType ptid) const {
        HitMap::const_iterator i = _hits.find(ptid);
        return i->second.pointer;
    }
    
    /// get the pointer-data for the nth pointer
    const PointerData& getNthPointerData(unsigned int ndx) const {
        return getPointerDataById(getNthPointerId(ndx));
    }
    
    /// get the pointer-data for the current pointer
    const PointerData& getCurrentPointerData() const {
        return getPointerDataById(getLastPointerId());
    }
    
    /// returns the draggable, if suitable
    virtual Draggable* asDraggable() { return NULL; }
    
    /// get the window-pickable
    WindowPickable* getWindowPickable() const;
protected:
    
    /// adds a hit to the list of hits, called from the PickEventHandler
    void addHit(const PointerData& pt, const Hit& hit)
    {
        _hits[pt.id] = HitData(pt, hit);
        _pointerIds.push_back(pt.id);
    }
    
    /// clear all hits, called from the PickEventHandler
    void clearHits() {
        _hits.clear();
        _pointerIds.clear();
    }
    void setLastPointerId(PointerIdType aid) { _lastPointerId = aid; }
    
    void handleSequenceBegan() {
        if (_sequenceBegan == false) {
            _sequenceBegan = true;
            sequenceBegan();
        }
    }
    
    void handleSequenceEnded() {
        if(_sequenceBegan)
            sequenceEnded();
        _sequenceBegan = false;
    }
    
    void setPickEventHandler(PickEventHandler* handler);
    PickEventHandler* getPickEventHandler() const { return _handler; }
    
    bool isRegistered() const { return _registered; }
private:
    PickEventHandler* _handler;
    bool _stopPropagation, _registered, _enabled, _sequenceBegan;
    HitMap _hits;
    unsigned int _numPointers;
    PointerIdType _lastPointerId;
    PointerIdVector _pointerIds;
    
friend class PickEventHandler;
};

}