/*
 *  LineGridGeometry.cpp
 *  ForceFields
 *
 *  Created by Stephan Huber on 31.03.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include <osg/Geometry>
#include <osg/Depth>
#include <cefix/LineGridGeometry.h>

namespace cefix {

LineGridGeometry::LineGridGeometry(const osg::Vec3& dimensions, const osg::Vec3& delta, const osg::Vec4& hilightcolor, const osg::Vec4& color) : 
	osg::Geometry() 

{

	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec3Array* normals = new osg::Vec3Array();
	osg::Vec4Array* colors = new osg::Vec4Array();
	osg::Vec4 c;
	for (unsigned int z = 0; z <= dimensions.z(); ++z) {
		for(unsigned int y = 0; y <= dimensions.y(); ++y) {
			vertices->push_back(osg::Vec3(0                       , y*delta.y(),z*delta.z()));
			vertices->push_back(osg::Vec3(dimensions.x()*delta.x(), y*delta.y(),z*delta.z()));
			normals->push_back(osg::Vec3(1,0,0));
			normals->push_back(osg::Vec3(1,0,0));
			c = ((y%5==0) || (y == dimensions.y())) ? hilightcolor : color;
			colors->push_back(c);
			colors->push_back(c);
			
		}
		for(unsigned int x = 0; x <= dimensions.x(); ++x) {
			vertices->push_back(osg::Vec3(x*delta.x(),0                        ,z*delta.z()));
			vertices->push_back(osg::Vec3(x*delta.x(), dimensions.y()*delta.y(),z*delta.z()));
			normals->push_back(osg::Vec3(0,1,0));
			normals->push_back(osg::Vec3(0,1,0));
			c = ((x%5==0) || (x==dimensions.x())) ? hilightcolor : color;
			colors->push_back(c);
			colors->push_back(c);
		}
	}
	
	for (unsigned int x = 0; x <= dimensions.x(); ++x) {
		for(unsigned int y = 0; y <= dimensions.y(); ++y) {
			vertices->push_back(osg::Vec3(x*delta.x()                       , y*delta.y(),0));
			vertices->push_back(osg::Vec3(x*delta.x(), y*delta.y(),dimensions.z()*delta.z()));
			normals->push_back(osg::Vec3(0,0,1));
			normals->push_back(osg::Vec3(0,0,1));
			c = ((y%5==0) || (y == dimensions.y())) ? hilightcolor : color;
			colors->push_back(c);
			colors->push_back(c);
			
		}
	}
	
	
	setColorArray(colors);
	setColorBinding(BIND_PER_VERTEX);
	
	setNormalArray(normals);
	setNormalBinding(BIND_PER_VERTEX);
	
	setVertexArray(vertices);
	
	addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, vertices->size()));
	
	
	// blend an, licht aus, in den depth-buffer schreiben auch aus
	getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	osg::Depth* depth = new osg::Depth();
	depth->setWriteMask(false);
	getOrCreateStateSet()->setAttribute(depth);
			
}



}