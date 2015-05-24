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

#ifndef DRAGGABLE_HEADER
#define DRAGGABLE_HEADER
#include <osg/io_utils>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <cefix/Pickable.h>
#include <cefix/AnimationBase.h>
#include <cefix/AnimationFactory.h>

#include "NodePathUtils.h"
namespace cefix {





/** a geode for showind the drag-plane, used for debug-purposes */
class DragDebugGeode : public osg::Geode {

	public:
		/* *ctor */
		DragDebugGeode();
		
		/** sets the plane from a point and a normal */
		void setPlaneNormal(const osg::Vec3 p, const osg::Vec3& n, float dw = 0.3f);
		
		/** sets the intersection point of mouse and plane */
		void setIntersection(const osg::Vec3& p, const osg::Vec3& p2) { (*_vertices)[2] = p; (*_vertices)[3] = p2; _geo->dirtyBound(); };
		
		/** draws a small quad around the intersecion */
		void setPlaneIntersection(const osg::Vec3& p, const osg::Vec3& normal);
		
	private:
		osg::ref_ptr<osg::Vec3Array> _vertices;
		osg::ref_ptr<osg::Geometry>  _geo;

};

/** class, which will do the hard work to get dragging working in 2D and 3D. 
 *  It computes the drag-delta and calls the virtual methods dragStarted, drag(delta) and dragStopped)
 *  you'll have to implement these methods
 */
class CEFIX_EXPORT Draggable : public cefix::Pickable {

	public:
		enum DragMode { 
			DRAG_ON_SCREEN_PLANE, /// drag on a plane which is parallel to the screen-surface
			DRAG_ON_MOUSE_PLANE, ///drag the object onto the plane which is perpendicular to the mouse-ray
			DRAG_ON_SURFACE_PLANE,  /// drag the object onto the surface-plane of the intersection-point
			DRAG_ON_CUSTOM_WORLD_PLANE,    /// use a custom drag plane, the normal is in world-space
			DRAG_ON_CUSTOM_LOCAL_PLANE	 /// use a custom drag plane, the normal is in local-space
		};
		
		/** ctor 
			@param mode the drag-mode
			@param planeNormal the normal for the drag-plane 
		 */
		Draggable(DragMode mode = DRAG_ON_SCREEN_PLANE);
		
		/** ctor 
			@param mode the drag-mode
			@param planeNormal the normal for the drag-plane 
		 */
		Draggable(DragMode mode, osg::Vec3 planeNormal);
		
		/** sets the drag-plane-normal */
		void setDragPlaneNormal(osg::Vec3 v) { _planeNormal = v; }
		
		/** sets the drag mode */
		void setDragMode(DragMode mode) {_dragMode = mode; }
		
		/** handles the mousedown, saves the matix used from converting mouse to local coordinates, sets up the drag-plane */
		virtual void down();
	
        virtual void sequenceBegan();
		
		/// handles mouse up, stops the drag
		virtual void up(bool inside);
        
        virtual void handle();
		
		/// stop a draggin process
		void stopDragging() { 
			_dragInProgress = false;
		}
		
		/** to be implemented by the inherited class, return true, if you want to happen the drag */
		virtual bool dragStarted() { return true; }
		
		/** to be implemented by the inherited class, return true, if the drag should continue */
		virtual bool drag(const osg::Vec3& delta) { return true; }
		
		/** to be implemented by the inherited class, return true, the drag stopped */
		virtual void dragStopped() {}
		
		bool isDragInProgress() { return _dragInProgress; }
		
		void setDragDebugGeode(DragDebugGeode* geode) { _debuggeode = geode; }
		
		void setContinuosComputationOfDragPlaneFlag(bool f) { _continuosComputationOfDragPlane = f; }
		
		virtual Draggable* asDraggable() { return this; }
	protected:
		/// do the drag, called from the animation-class
		bool doDrag();
			
	private:
		void computeInverseAndDragPlane();
		
		DragMode	_dragMode;
		osg::Vec3	_clickLoc;
		osg::Plane	_dragPlane;
		osg::Vec3	_planeNormal;
		bool		_dragInProgress;
		osg::Matrix	_invMatrix;
		osg::observer_ptr<DragDebugGeode> _debuggeode;
		osgUtil::LineSegmentIntersector::Intersection _intersectionOnDown;
		bool		_continuosComputationOfDragPlane;
        int         _pointerId;
	
	
	friend class AnimateDraggable;
};

template <class DraggableClass> class RefDraggable : public DraggableClass, public osg::Referenced {

};


} // end of namespace

#endif
