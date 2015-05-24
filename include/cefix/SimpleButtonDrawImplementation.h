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

#ifndef SIMPLE_BUTTON_DRAW_IMPLEMENTATION
#define SIMPLE_BUTTON_DRAW_IMPLEMENTATION

#include <osg/Geometry>
#include <osg/Geode>
#include <cefix/AbstractWidget.h>
#include <cefix/Pickable.h>
#include <cefix/Functor.h>
#include <cefix/Export.h>


namespace cefix {
class ButtonWidget;
class PropertyList;

class CEFIX_EXPORT SimpleButtonDrawImplementation : public osg::Geode, public cefix::Pickable {

	public:
		typedef std::map<AbstractWidget::States, osg::Vec4> ColorMap;
		
		SimpleButtonDrawImplementation(const osg::Vec3& pos, const osg::Vec2& size);
		
		void update(ButtonWidget* widget);
		
		virtual void enter() { stopPropagation(); if (_fn.valid()) _fn->call(AbstractWidget::MOUSE_ENTER);  }
		virtual void leave() { if (_fn.valid()) _fn->call(AbstractWidget::MOUSE_LEAVE);  }

		virtual void down()  {
			stopPropagation();
			if (_fn.valid()) _fn->call(AbstractWidget::MOUSE_DOWN); 
		}
		
		virtual void up(bool inside) { if (_fn.valid()) _fn->call( (inside) ? AbstractWidget::MOUSE_UP : AbstractWidget::MOUSE_UP_OUTSIDE ); }

		void setBackgroundColorForState(AbstractWidget::States state, osg::Vec4 c) { _colorMap[state] = c; }
		
		void setCallback(cefix::Functor1ParamAbstractT<void, AbstractWidget::EventTypes>* fn) { _fn = fn; }
		
		const osg::Vec2& getSize() const { return _size; }
		
		const osg::Vec3& getPosition() const { return _pos; }

		
		void applyPropertyList(cefix::PropertyList* pl);

	
	protected:
		osg::Vec3	_pos;
		osg::Vec2	_size;
	private:	
		ColorMap	_colorMap;
		osg::ref_ptr<osg::Geometry> _geo;
		osg::ref_ptr<osg::Vec4Array> _colors;
		osg::ref_ptr< cefix::Functor1ParamAbstractT<void, AbstractWidget::EventTypes> > _fn;
		
};

}

#endif