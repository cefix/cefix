//
//  Pickable.cpp
//  cefixSketch
//
//  Created by Stephan Huber on 30.08.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Pickable.h"
#include "PickEventHandler.h"

namespace cefix {

Pickable::PointerIdType Pickable::PointerData::getMousePointerId()
{
    return -1;
}
Pickable::PointerData::PointerData(
    const osgGA::GUIEventAdapter& ea,
    const osgGA::GUIEventAdapter::TouchData::TouchPoint& tp)
:   id(tp.id),
    x(tp.x),
    y(ea.getWindowHeight() - tp.y),
    revertedY(y),
    phase(getPhase(tp.phase)),
    downCount(tp.tapCount),
    event(new osgGA::GUIEventAdapter(ea))
{
}
        
Pickable::PointerData::PointerData(const osgGA::GUIEventAdapter& ea)
:   id(getMousePointerId()),
    x(ea.getX()),
    y(ea.getY()),
    revertedY(ea.getWindowHeight() - ea.getY()),
    phase(getPhase(ea.getEventType())),
    downCount(0),
    event(new osgGA::GUIEventAdapter(ea))
{
}

Pickable::PointerData::Phase Pickable::PointerData::PointerData::getPhase(const osgGA::GUIEventAdapter::EventType& event_type)
{
    switch (event_type) {
        case osgGA::GUIEventAdapter::PUSH:
            return DOWN;
            break;
        
        case osgGA::GUIEventAdapter::RELEASE:
            return UP;
            break;
            
        case osgGA::GUIEventAdapter::DOUBLECLICK:
            return DOWN;
            break;
        
        case osgGA::GUIEventAdapter::DRAG:
            return DRAG;
            break;
            
        case osgGA::GUIEventAdapter::MOVE:
            return MOVE;
            break;
        
        default:
            return UNKNOWN;
    }
}
        
Pickable::PointerData::Phase Pickable::PointerData::getPhase(const osgGA::GUIEventAdapter::TouchPhase& touch_phase) {
    switch(touch_phase) {
        case osgGA::GUIEventAdapter::TOUCH_BEGAN:
            return DOWN;
            break;
            
        case osgGA::GUIEventAdapter::TOUCH_MOVED:
        case osgGA::GUIEventAdapter::TOUCH_STATIONERY:
            return DRAG;
            break;
        
        case osgGA::GUIEventAdapter::TOUCH_ENDED:
            return UP;
            break;
        
        default:
            return UNKNOWN;
    }
}

Pickable::Pickable()
:   _handler(NULL),
    _stopPropagation(true),
    _registered(false),
    _enabled(true),
    _sequenceBegan(false),
    _hits(),
    _numPointers(0),
    _lastPointerId(0)
{

}

Pickable::~Pickable()
{
    if (_handler) {
        _handler->deregisterPickable(this);
        _handler = NULL;
    }
}

WindowPickable* Pickable::getWindowPickable() const
{
    return getPickEventHandler()->getWindowPickable();
}

void Pickable::setPickEventHandler(PickEventHandler* handler)
{
    _handler = handler;
    if (!_registered && handler) {
        handler->registerPickable(this);
        _registered = true;
    }
}

}