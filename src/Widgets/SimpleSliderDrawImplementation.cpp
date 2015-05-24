/*
 *  SimpleSliderDrawImplementation.cpp
 *  cefixWidgets
 *
 *  Created by Stephan Huber on 04.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "SimpleSliderDrawImplementation.h"
#include <cefix/DataFactory.h>
#include <cefix/SliderWidget.h>
#include <cefix/SimpleWidgetDrawImplementationDefaults.h>

namespace cefix {

class RefSliderDraggable : public SliderDraggable, public osg::Referenced {
public:
	RefSliderDraggable() : SliderDraggable(), osg::Referenced() {}
	
};


SimpleSliderDrawImplementation::SimpleSliderDrawImplementation(const osg::Vec3& pos, const osg::Vec2& size, const osg::Vec2& throbberSize):
	osg::Geode(),
	_pos(pos),
	_size(size),
	_throbberSize(throbberSize)
{
    SimpleWidgetDrawImplementationDefaults* defaults = SimpleWidgetDrawImplementationDefaults::instance();
    
	_geo = new osg::Geometry();
	_vertices = new osg::Vec3Array();
	_vertices->resize(8);
	_geo->setVertexArray(_vertices.get());
	
	_colors = new osg::Vec4Array();
	_colors->resize(4);
	_geo->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	_geo->setColorArray(_colors.get());
	_geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 8));
	_geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 4));
	_geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 4, 4));
	
	setBackgroundFillColor(defaults->getSliderBackgroundFillColor());
	setBackgroundStrokeColor(defaults->getSliderBackgroundStrokeColor());
	
	setThrobberFillColor(defaults->getSliderThrobberFillColor());
	setThrobberStrokeColor(defaults->getSliderThrobberStrokeColor());
	
	setFillColorForDisabledState(defaults->getSliderDisabledFillColor());
	setStrokeColorForDisabledState(defaults->getSliderDisabledStrokeColor());
	
	addDrawable(_geo.get());
	getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	_draggable = new RefSliderDraggable();
	setUserData(_draggable);
	
}

void SimpleSliderDrawImplementation::setSlider(SliderWidget* slider) {
	_draggable->setSlider(slider);
	_draggable->setOrigin(_pos);
	_draggable->setDragMode(cefix::Draggable::DRAG_ON_SURFACE_PLANE);
}


void SimpleSliderDrawImplementation::update(SliderWidget* widget) {
	
	osg::Vec3 throbberPosition = widget->getThrobberPos();

	if (widget->isEnabled()) {
		(*_colors)[0] = _bgFillColor;
		(*_colors)[2] = _bgStrokeColor;
		(*_colors)[1] = _thrFillColor;
		(*_colors)[3] = _thrStrokeColor;
	} else {
		(*_colors)[0] = _disabledFillColor;
		(*_colors)[2] = _disabledStrokeColor;
		(*_colors)[1] = _disabledFillColor;
		(*_colors)[3] = _disabledStrokeColor;
	}
	
	(*_vertices)[0] = _pos + osg::Vec3(0,0,0);
	(*_vertices)[1] = _pos + osg::Vec3(_size[0],0,0);
	(*_vertices)[2] = _pos + osg::Vec3(_size[0],_size[1],0);
	(*_vertices)[3] = _pos + osg::Vec3(0,_size[1],0);
	
	(*_vertices)[4] = _pos + throbberPosition +  osg::Vec3(0,0,0);
	(*_vertices)[5] = _pos + throbberPosition  + osg::Vec3(_throbberSize[0],0,0);
	(*_vertices)[6] = _pos + throbberPosition  + osg::Vec3(_throbberSize[0],_throbberSize[1],0);
	(*_vertices)[7] = _pos + throbberPosition  + osg::Vec3(0,_throbberSize[1],0);	
		
	_geo->dirtyBound();
	_geo->dirtyDisplayList();

}

void SimpleSliderDrawImplementation::applyPropertyList(cefix::PropertyList* pl) {

	if (pl && pl->hasKey("prefs"))
		applyPropertyList(cefix::DataFactory::instance()->getPreferencesPropertyListFor(pl->get("prefs")->asString()));
		
	if (pl == NULL)	return;

	if (pl->hasKey("backgroundFillColor"))
		setBackgroundFillColor(pl->get("backgroundFillColor")->asVec4());
	
	if (pl->hasKey("backgroundStrokeColor"))
		setBackgroundStrokeColor(pl->get("backgroundStrokeColor")->asVec4());
		
	if (pl->hasKey("throbberFillColor"))
		setThrobberFillColor(pl->get("backgroundFillColor")->asVec4());
	
	if (pl->hasKey("throbberStrokeColor"))
		setThrobberStrokeColor(pl->get("backgroundStrokeColor")->asVec4());
	
	if (pl->hasKey("FillColorForDisabledState"))
		setFillColorForDisabledState(pl->get("FillColorForDisabledState")->asVec4());	

	if (pl->hasKey("StrokeColorForDisabledState"))
		setStrokeColorForDisabledState(pl->get("StrokeColorForDisabledState")->asVec4());	
}


}