/*
 *  SimpleButtonDrawInmplementation.cpp
 *  cefixWidgets
 *
 *  Created by Stephan Huber on 04.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <osg/Geometry>

#include "SimpleButtonDrawImplementation.h"
#include <cefix/DataFactory.h>
#include <cefix/ButtonWidget.h>
#include <cefix/SimpleWidgetDrawImplementationDefaults.h>

namespace cefix {

SimpleButtonDrawImplementation::SimpleButtonDrawImplementation(const osg::Vec3& pos, const osg::Vec2& size) : 
	osg::Geode(),
	Pickable(), 
	_pos(pos), 
	_size(size) 
{
	stopPropagation();
    
    SimpleWidgetDrawImplementationDefaults* defaults = SimpleWidgetDrawImplementationDefaults::instance();
	
	_geo = new osg::Geometry();
	osg::Vec3Array* vertices = new osg::Vec3Array(4);
	
	_geo->setVertexArray(vertices);
	
	(*vertices)[0] = _pos + osg::Vec3(0,0,0);
	(*vertices)[1] = _pos + osg::Vec3(_size[0],0,0);
	(*vertices)[2] = _pos + osg::Vec3(_size[0],_size[1],0);
	(*vertices)[3] = _pos + osg::Vec3(0,_size[1],0);
	
	_geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0,4));
	_geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0,4));
	_colors = new osg::Vec4Array(1);
	
    AbstractWidget::States states[6] = {
        AbstractWidget::DISABLED, 
        AbstractWidget::NORMAL,
        AbstractWidget::NORMAL_OVER,
        AbstractWidget::SELECTED,
        AbstractWidget::SELECTED_OVER,
        AbstractWidget::PRESSED
    };
    
    for(unsigned int i = 0; i < 6; ++i) {
        setBackgroundColorForState(states[i], defaults->getBtnBgColorFor(states[i]));
    }
    
	
	(*_colors)[0] = _colorMap[AbstractWidget::NORMAL];
	_geo->setColorArray(_colors.get());
	_geo->setColorBinding(osg::Geometry::BIND_OVERALL);
	
	addDrawable(_geo.get());
	
	getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	
}


void SimpleButtonDrawImplementation::update(ButtonWidget* widget) {

	if (_colorMap.find(widget->getState()) != _colorMap.end()) {
		(*_colors)[0] = _colorMap[widget->getState()];
		_geo->dirtyDisplayList();
	}

}
void SimpleButtonDrawImplementation::applyPropertyList(cefix::PropertyList* pl) {

	if (pl && pl->hasKey("prefs"))
		applyPropertyList(cefix::DataFactory::instance()->getPreferencesPropertyListFor(pl->get("prefs")->asString()));
		
	if (pl == NULL)	return;
	
	static std::map<std::string, cefix::AbstractWidget::States> statemap;
	if (statemap.size() == 0) {
		statemap["disabled"] = AbstractWidget::DISABLED;
		statemap["normal"] = AbstractWidget::NORMAL;
		statemap["normal_over"] = AbstractWidget::NORMAL_OVER;
		statemap["selected"] = AbstractWidget::SELECTED;
		statemap["selected_over"] = AbstractWidget::SELECTED_OVER;
		statemap["pressed"] = AbstractWidget::PRESSED;
	}
	if (pl->hasKey("backgroundColors") && (pl->get("backgroundColors")->getType() == cefix::AbstractProperty::PROPERTY_LIST)) {
		cefix::PropertyList* subpl = pl->get("backgroundColors")->asPropertyList();
		for(std::map<std::string, cefix::AbstractWidget::States>::iterator i = statemap.begin(); i != statemap.end(); ++i) {
			if (subpl->hasKey(i->first))
				setBackgroundColorForState(i->second, subpl->get(i->first)->asVec4());
		}
	}
	

}

}