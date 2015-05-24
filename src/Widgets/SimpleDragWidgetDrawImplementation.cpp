/*
 *  SimpleDragWidgetDrawImplementation.cpp
 *  cefixWidgets
 *
 *  Created by Stephan Huber on 19.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "SimpleDragWidgetDrawImplementation.h"
#include <cefix/DragWidget.h>
namespace cefix {


SimpleDragWidgetDrawImplementation::SimpleDragWidgetDrawImplementation():
	osg::Geode()
{

	_geo = new osg::Geometry();
	_vertices = new osg::Vec3Array();
	_vertices->resize(8);
	_geo->setVertexArray(_vertices.get());
	
	_colors = new osg::Vec4Array();
	_colors->resize(2);
	_geo->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
	_geo->setColorArray(_colors.get());
	_geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));
	_geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 4));
	
	setFillColor(osg::Vec4(0.5,0.5,0.5,0.5));
	setStrokeColor(osg::Vec4(0.5,0.5,0.5,0.7));
	
	addDrawable(_geo.get());
	getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	
}




void SimpleDragWidgetDrawImplementation::update(DragWidget* widget) 
{
	_size = widget->getSize();
	_pos = widget->getPosition();
	
	(*_vertices)[0] = _pos + osg::Vec3(0,0,0);
	(*_vertices)[1] = _pos + osg::Vec3(_size[0],0,0);
	(*_vertices)[2] = _pos + osg::Vec3(_size[0],_size[1],0);
	(*_vertices)[3] = _pos + osg::Vec3(0,_size[1],0);
	
	_geo->dirtyBound();
	_geo->dirtyDisplayList();
	//if (getDragImplementation()) getDragImplementation()->setOrigin(p);
}

}