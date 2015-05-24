/*
 *  OctTree.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 19.05.10.
 *  Copyright 2010 Digital Mind. All rights reserved.
 *
 */

#include "OctTree.h"

namespace cefix {


OctTreeGeometry::OctTreeGeometry()
:	osg::Geometry()
{
	_vertices = new osg::Vec3Array(2);
	setVertexArray(_vertices);
	_da = new osg::DrawArrays(GL_LINES, 0, _vertices->size());
	addPrimitiveSet(_da);
	osg::Vec4Array* c = new osg::Vec4Array();
	c->push_back(osg::Vec4(1,0,0,0.25));
	setColorArray(c);
	setColorBinding(BIND_OVERALL);
	getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}

void OctTreeGeometry::drawCube(float x1, float y1, float z1, float  x2,float  y2, float z2) 
{
	_vertices->push_back(osg::Vec3(x1, y1, z1));
	_vertices->push_back(osg::Vec3(x2, y1, z1));
	
	_vertices->push_back(osg::Vec3(x2, y1, z1));
	_vertices->push_back(osg::Vec3(x2, y2, z1));
	
	_vertices->push_back(osg::Vec3(x2, y2, z1));
	_vertices->push_back(osg::Vec3(x1, y2, z1));
	
	_vertices->push_back(osg::Vec3(x1, y2, z1));
	_vertices->push_back(osg::Vec3(x1, y1, z1));
	
	
	_vertices->push_back(osg::Vec3(x1, y1, z2));
	_vertices->push_back(osg::Vec3(x2, y1, z2));
	
	_vertices->push_back(osg::Vec3(x2, y1, z2));
	_vertices->push_back(osg::Vec3(x2, y2, z2));
	
	_vertices->push_back(osg::Vec3(x2, y2, z2));
	_vertices->push_back(osg::Vec3(x1, y2, z2));
	
	_vertices->push_back(osg::Vec3(x1, y2, z2));
	_vertices->push_back(osg::Vec3(x1, y1, z2));
	
	
	_vertices->push_back(osg::Vec3(x1, y1, z1));
	_vertices->push_back(osg::Vec3(x2, y1, z1));
	
	_vertices->push_back(osg::Vec3(x2, y1, z1));
	_vertices->push_back(osg::Vec3(x2, y1, z2));
	
	_vertices->push_back(osg::Vec3(x2, y1, z2));
	_vertices->push_back(osg::Vec3(x1, y1, z2));
	
	_vertices->push_back(osg::Vec3(x1, y1, z2));
	_vertices->push_back(osg::Vec3(x1, y1, z1));
	
	
	_vertices->push_back(osg::Vec3(x1, y2, z1));
	_vertices->push_back(osg::Vec3(x2, y2, z1));
	
	_vertices->push_back(osg::Vec3(x2, y2, z1));
	_vertices->push_back(osg::Vec3(x2, y2, z2));
	
	_vertices->push_back(osg::Vec3(x2, y2, z2));
	_vertices->push_back(osg::Vec3(x1, y2, z2));
	
	_vertices->push_back(osg::Vec3(x1, y2, z2));
	_vertices->push_back(osg::Vec3(x1, y2, z1));
	
}

}