/*
 *  QuadTree.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 01.05.10.
 *  Copyright 2010 Digital Mind. All rights reserved.
 *
 */

#include "QuadTree.h"
namespace cefix {


QuadTreeGeometry::QuadTreeGeometry(float locZ)
:	osg::Geometry(),
	_locZ(locZ)
{
	_vertices = new osg::Vec3Array(2);
	setVertexArray(_vertices);
	_da = new osg::DrawArrays(GL_LINES, 0, _vertices->size());
	addPrimitiveSet(_da);
	osg::Vec4Array* c = new osg::Vec4Array();
	c->push_back(osg::Vec4(1,0,0,0.5));
	setColorArray(c);
	setColorBinding(BIND_OVERALL);
	getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
}

void QuadTreeGeometry::drawRect(float x1, float y1,float  x2,float  y2) 
{
	_vertices->push_back(osg::Vec3(x1, y1, _locZ));
	_vertices->push_back(osg::Vec3(x2, y1, _locZ));
	
	_vertices->push_back(osg::Vec3(x2, y1, _locZ));
	_vertices->push_back(osg::Vec3(x2, y2, _locZ));
	
	_vertices->push_back(osg::Vec3(x2, y2, _locZ));
	_vertices->push_back(osg::Vec3(x1, y2, _locZ));
	
	_vertices->push_back(osg::Vec3(x1, y2, _locZ));
	_vertices->push_back(osg::Vec3(x1, y1, _locZ));
	
}

}