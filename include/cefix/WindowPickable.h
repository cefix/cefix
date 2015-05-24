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

#include <osg/Referenced>
#include "Pickable.h"
#include <cefix/NodePathUtils.h>


namespace cefix {

class PickEventHandler;

/** the windowPickble class provides a way to get mouseevents on a per window-basis without any geometry needed */
class WindowPickable : public osg::Referenced {

public:
    /// small helper class to store all properties of a pointer. A pointer is a mouse-pointer, or a touch-point
	typedef Pickable::PointerData PointerData;
	typedef std::map<Pickable::PointerIdType, PointerData> PointerMap;
	typedef PointerMap::const_iterator const_iterator;
	typedef PointerMap::iterator iterator;
	
    /// c'tor
	WindowPickable() : osg::Referenced(), _pointers(), _pickEventHandler(NULL) {}
	
    /// adds a pointer
    void addPointer(const PointerData& pt)
	{ 
		if(pt.id == Pickable::PointerData::getMousePointerId())
			_mousePointer = pt;
			
		_pointers.insert(std::make_pair(pt.id, pt));
	}
	
    /// clear all pickables
	inline void clearPickables(unsigned int max=0)
    {
        _pickables.clear();
        if (max) _pickables.reserve(max);
    }
	
    /// clear all pointers
    inline void clearPointers()
    {
        _pointers.clear();
    }
	
    /// get num pickables
    inline unsigned int getNumPickables()
    {
        return _pickables.size();
    }
	
    /// add a pickable
    inline void addPickable(Pickable * p)
    {
        _pickables.push_back(p);
    }
	
    /// get num pointers
    unsigned int getNumPointers() const
    {
        return _pointers.size();
    }
	
    inline Pickable::PointerIdType getMousePointerId() const
    {
        return Pickable::PointerData::getMousePointerId();
    }
    
	/// @return the x-coord of the mouse
	inline float getScreenX(Pickable::PointerIdType pointer_id) const
    {
        return getPointer(pointer_id).x;
    }
	inline float getScreenX() const
    {
        return getScreenX(getMousePointerId());
    }
	
    /// @return the y-coord of the mouse
	inline float getScreenY(Pickable::PointerIdType pointer_id) const
    {
        return getPointer(pointer_id).revertedY;
    }
	inline float getScreenY() const
    {
        return getScreenY(getMousePointerId());
    }
    
	/// @return the native y-coord of the mouse, 0 = bottom xx = top
	inline float getNativeY(Pickable::PointerIdType pointer_id) const
    {
        return getPointer(pointer_id).y;
    }
	inline float getNativeY() const
    {
        return getNativeY(getMousePointerId());
    }
    
	/// @return the native X-coord of the mouse, (same as getScreenX)
	inline float getNativeX(Pickable::PointerIdType pointer_id) const
    {
        return getPointer(pointer_id).x;
    }
	inline float getNativeX() const
    {
        return getNativeX(getMousePointerId());
    }
    
	/// return the mouse-coord as osg::Vec2
	inline osg::Vec2 getScreen(Pickable::PointerIdType pointer_id) const
    {
        return getPointer(pointer_id).asVec2();
    }
    inline osg::Vec2 getScreen() const
    {
        return getScreen(getMousePointerId());
    }

	
	/// return the mouse-coord as osg::Vec2
	inline osg::Vec2 getNative(Pickable::PointerIdType pointer_id) const
    {
        return getPointer(pointer_id).asNativeVec2();
    }
    inline osg::Vec2 getNative() const
    {
        return getNative(getMousePointerId());
    }

	
	/// @return the mouse-coord in world-coords (on the near-plane)
	inline const osg::Vec3 &getNear(Pickable::PointerIdType pointer_id) const
    {
        return getPointer(pointer_id).nearPoint;
    }
    inline const osg::Vec3 &getNear() const
    {
        return getNear(getMousePointerId());
    }
	
	/// @return the mouse-coord in wolrd-coords (on the far plane)
	inline const osg::Vec3 &getFar(Pickable::PointerIdType pointer_id) const
    {
        return getPointer(pointer_id).farPoint;
    }
    inline const osg::Vec3 &getFar() const
    {
        return getFar(getMousePointerId());
    }
	
	// gets the ray of the mouse-coord
	inline osg::Vec3 getRay(Pickable::PointerIdType pointer_id = 0) const
    {
        return getPointer(pointer_id).asRay();
    }
	inline osg::Vec3 getRay() const
    {
        return getRay(getMousePointerId());
    }
    
	/** return the mouse ray in local space coords */
	inline const osg::Vec3 getLocalRay(const osg::NodePath& path, Pickable::PointerIdType pointer_id)
    {
		// Mausposition ins lokale Koordinatensystem transformieren
		osg::Matrix m = cefix::NodePathUtils::getWindowToLocalTransform(path);
		return getPointer(pointer_id).asTransformedRay(m);
	}
    inline const osg::Vec3 getLocalRay(const osg::NodePath& path)
    {
        return getLocalRay(path, getMousePointerId());
    }
	
	inline const osg::Vec3 getLocalNear(const osg::NodePath& path, Pickable::PointerIdType pointer_id)
    {
		osg::Matrix m = cefix::NodePathUtils::getWindowToLocalTransform(path);
		return getPointer(pointer_id).asLocalNear() * m; 
	}
    inline const osg::Vec3 getLocalNear(const osg::NodePath& path)
    {
        return getLocalNear(path, getMousePointerId());
    }
	
	inline const osg::Vec3 getLocalFar(const osg::NodePath& path, Pickable::PointerIdType pointer_id)
    {
		osg::Matrix m = cefix::NodePathUtils::getWindowToLocalTransform(path);
		return getPointer(pointer_id).asLocalFar() * m; 
	}
    inline const osg::Vec3 getLocalFar(const osg::NodePath& path)
    {
        return getLocalFar(path, getMousePointerId());
    }
    
    virtual void down() {}
    virtual void up() {}

	iterator begin() { return _pointers.begin(); }
	iterator end() { return _pointers.end(); }
	
	const_iterator begin() const { return _pointers.begin(); }
	const_iterator end() const  { return _pointers.end(); }
	
    PickEventHandler* getPickEventHandler() { return _pickEventHandler; }
protected:
    void setPickEventHandler(PickEventHandler* h) { _pickEventHandler = h; }
    
private:
	const PointerData& getPointer(Pickable::PointerIdType pointer_id) const
	{ 
		if (pointer_id == Pickable::PointerData::getMousePointerId()) return _mousePointer;
		PointerMap::const_iterator i = _pointers.find(pointer_id);
		return (i != _pointers.end()) ? i->second : _mousePointer; 
	}
	
	PointerMap      _pointers;
	PointerData	   _mousePointer;
	
	std::vector<Pickable*> _pickables;
    
    PickEventHandler* _pickEventHandler;

friend class PickEventHandler;
};

}
