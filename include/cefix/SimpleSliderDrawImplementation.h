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

#ifndef SIMPLE_SLIDER_DRAW_IMPLEMENTATION_HEADER
#define SIMPLE_SLIDER_DRAW_IMPLEMENTATION_HEADER

#include <osg/Geode>
#include <cefix/Draggable.h>
#include <cefix/AbstractWidget.h>


namespace cefix {

class PropertyList;
class SliderWidget;
class RefSliderDraggable;


class CEFIX_EXPORT SimpleSliderDrawImplementation : public osg::Geode {
	
	public:
		SimpleSliderDrawImplementation(const osg::Vec3& pos, const osg::Vec2& size, const osg::Vec2& throbberSize);
		
		void update(SliderWidget* widget); 
		
		void setBackgroundFillColor(osg::Vec4 c) { _bgFillColor = c; (*_colors)[0] = c; _geo->dirtyDisplayList(); }
		void setBackgroundStrokeColor(osg::Vec4 c) { _bgStrokeColor = c; (*_colors)[2] = c; _geo->dirtyDisplayList(); }
		
		void setThrobberFillColor(osg::Vec4 c) { _thrFillColor = c; (*_colors)[1] = c; _geo->dirtyDisplayList(); }
		void setThrobberStrokeColor(osg::Vec4 c) { _thrStrokeColor = c; (*_colors)[3] = c; _geo->dirtyDisplayList(); }
		
		void setFillColorForDisabledState(const osg::Vec4& c) { _disabledFillColor = c; }
		void setStrokeColorForDisabledState(const osg::Vec4& c) { _disabledStrokeColor = c; }
		
		void applyPropertyList(cefix::PropertyList* pl);
		
		void setSlider(SliderWidget* slider);
	private:
		osg::Vec3 _pos;
		osg::Vec2 _size, _throbberSize;
		
		osg::ref_ptr<osg::Geometry>		_geo;
		osg::ref_ptr<osg::Vec3Array>	_vertices;
		osg::ref_ptr<osg::Vec4Array>	_colors;
		
		osg::Vec4						_bgFillColor, _bgStrokeColor, _thrFillColor, _thrStrokeColor;
		osg::Vec4						_disabledFillColor, _disabledStrokeColor;
		RefSliderDraggable*				_draggable;

};


} // end of namespace


#endif