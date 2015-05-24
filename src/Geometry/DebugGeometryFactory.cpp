/*
 *  DebugGeometry.cpp
 *  o2 Aura
 *
 *  Created by Stephan Huber on 10.10.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "DebugGeometryFactory.h"
#include <cefix/TextGeode.h>

namespace cefix {

DebugGeometry::DebugGeometry()
:	_points(new DrawElements(GL_POINTS)),
	_lines(new DrawElements(GL_LINES)),
	_vertices(new osg::Vec3Array()),
	_colors(new osg::Vec4Array()),
	_geode(NULL)
{
	_group = new osg::Group();
	_geo = new osg::Geometry();
	_geo->setDataVariance(osg::Object::DYNAMIC);
	_geo->setVertexArray(_vertices);
	_geo->setColorArray(_colors);
	_geo->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
		
	_geo->setSupportsDisplayList(false);
	
	setPointSize(4);
	
}


osg::Node* DebugGeometry::getOrCreateNode() {

	if (_geode.valid()) return _group.get();
	
	_geode = new osg::Geode();
	_geode->addDrawable( _geo.get() );
	_geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	_geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	_geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	_geode->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin",  osg::StateSet::USE_RENDERBIN_DETAILS);
	_group->addChild(_geode.get());
	return _group.get();
}

void DebugGeometry::addText(const osg::Vec3& pos, const std::string& text, const osg::Vec4& color, const std::string& font, float fontsize) 
{
	cefix::Utf8TextGeode* tg = new cefix::Utf8TextGeode(font, fontsize, text);
	tg->setPosition(pos);
	tg->setTextColor(color);
	tg->refresh();
	_group->addChild(tg);
}


DebugGeometryFactory* DebugGeometryFactory::getInstance() 
{
	static osg::ref_ptr<DebugGeometryFactory> s_ptr = new DebugGeometryFactory();	
	return s_ptr.get();
}

DebugGeometryFactory::~DebugGeometryFactory(){
	osg::notify(osg::INFO) << "destructing DebugGeometryFactory" << std::endl;
}

}

