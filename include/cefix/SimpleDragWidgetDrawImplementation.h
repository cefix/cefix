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
#ifndef SIMPLE_DRAG_WIDGET_DRAW_IMPLEMENTATION_HEADER
#define SIMPLE_DRAG_WIDGET_DRAW_IMPLEMENTATION_HEADER

#include <osg/Geode>
#include <cefix/Draggable.h>
#include <cefix/PropertyList.h>

namespace cefix {
class DragWidget;
class DragWidgetDraggable : public Draggable {
	
	public: 
		DragWidgetDraggable() : Draggable(), _functor(NULL), _origin() { stopPropagation(); }
		
		virtual bool dragStarted() {  
			// std::cout << "intersect: " << p->hit().localIntersectionPoint << " origin " << _origin << std::endl;
			_p = getCurrentHit().localIntersectionPoint - _origin;
			return true; 
		}
		
		virtual inline bool drag(const osg::Vec3& delta) { 
			// std::cout << "origin: " << _origin << " p: " << _p << " delta " << delta << std::endl;
			if (_functor.valid()) _functor->call( _origin  + delta); 
			return true; 
		}
		
		void setOrigin(const osg::Vec3& p) {_origin = p; }
		void setFunctor(Functor1ParamAbstractT<void, const osg::Vec3&>* f) { _functor = f; }
		
	private:
		osg::ref_ptr< Functor1ParamAbstractT<void, const osg::Vec3&> > _functor;
		osg::Vec3 _origin, _p;
	
};

class CEFIX_EXPORT SimpleDragWidgetDrawImplementation : public osg::Geode, public DragWidgetDraggable {
	
	public:
		SimpleDragWidgetDrawImplementation();
		
		void update(DragWidget* widget); 
		
		virtual void down()
		{
			DragWidgetDraggable::setOrigin(_pos); 
			DragWidgetDraggable::down();
		}
		
		void setFillColor(osg::Vec4 c) { (*_colors)[0] = c; _geo->dirtyDisplayList(); }
		void setStrokeColor(osg::Vec4 c) { (*_colors)[1] = c; _geo->dirtyDisplayList(); }
		
		void setDragCallback(Functor1ParamAbstractT<void, const osg::Vec3&>* functor) {
			if (functor) {
				setFunctor(functor); 
				setOrigin(_pos);
			} else
				setFunctor(NULL);
		}
		
		void applyPropertyList(cefix::PropertyList* pl) {}
		
	private:
		osg::Vec3 _pos;
		osg::Vec2 _size, _throbberSize;
		
		osg::ref_ptr<osg::Geometry>		_geo;
		osg::ref_ptr<osg::Vec3Array>	_vertices;
		osg::ref_ptr<osg::Vec4Array>	_colors;

};


} // end of namespace


#endif