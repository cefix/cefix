/*
 *  LineStatistics.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 29.03.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <sstream>
#include <iomanip>
#include "LineStatistics.h"

namespace cefix {


// ----------------------------------------------------------------------------------------------------------
// createGeometry
// ----------------------------------------------------------------------------------------------------------

void LineStatistics::createGeometry() 
{

	osg::Geode* geode = new osg::Geode();
	_geometry = new osg::Geometry(); 
	_geometry->setDataVariance(osg::Object::DYNAMIC);
	
	_vertices = new osg::Vec3Array();
	_vertices->resize(getNumValues() + 2);
	for(unsigned int i = 0; i < getNumValues() + 2; ++i) {
		(*_vertices)[i] = osg::Vec3(i*getXStepSize(), 0,0);
	}
	_geometry->setVertexArray(_vertices.get());
	_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, getNumValues()));
	_colors = new osg::Vec4Array();
	_colors->resize(getNumValues()+2);
	computeColors();
	_geometry->setColorArray(_colors.get());
	_geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	
	_geometry->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	
	_textgeode = new Utf8TextGeode("system.xml", 0, "");
	_textgeode->setDataVariance(osg::Object::DYNAMIC);
	_textgeode->setTextColor(getColor());
	_textgeode->setSubPixelAlignment(false);
	geode->addDrawable(_geometry.get());
	geode->setName("dont_optimize");
	addChild(geode);
	addChild(_textgeode.get());
	
}

// ----------------------------------------------------------------------------------------------------------
// updateGeometry
// ----------------------------------------------------------------------------------------------------------
	
void LineStatistics::updateGeometry() 
{
	
	float v = getCurrentValue();
	unsigned int ndx = getCurrentIndex();
		
	(*_vertices)[ndx] = osg::Vec3(ndx * getXStepSize(), v  * getYScale(),0);
	(*_vertices)[ndx+1] = osg::Vec3(ndx * getXStepSize(), v  * getYScale(),0);
	(*_vertices)[ndx+2] = osg::Vec3(ndx * getXStepSize(), v  * getYScale(),0);

	computeColors();

	_geometry->dirtyBound();
	_geometry->dirtyDisplayList();
}


// ----------------------------------------------------------------------------------------------------------
// updateCaption
// ----------------------------------------------------------------------------------------------------------

void LineStatistics::updateCaption() {
	float v = getCurrentValue();
	
	std::ostringstream ss;
	ss.str("");
	ss.clear();
	
	ss << std::fixed << std::setprecision(3) << getCaption() << ": " << v;
	
	_textgeode->setText(ss.str());
	osg::Vec3 pos((getNumValues()+1) * getXStepSize(), v  * getYScale(),0);
	if (_locations.valid()) 
		_locations->get(pos, _textgeode.get());
	else {
		_textgeode->setPosition(pos);
		_textgeode->refresh();
	}

	_textgeode->setTextColor(getColor());
	


}

// ----------------------------------------------------------------------------------------------------------
// computeColors
// ----------------------------------------------------------------------------------------------------------

void LineStatistics::computeColors() {
	osg::Vec4 c = getColor();
	float maxblend = c[3];
	float minblend = 0.05f;
	int ndx = getCurrentIndex();
	int cnt = getNumValues();
	
	int j,k(0);
	for(int i = ndx; i > ndx - cnt; --i,++k) {
		j = (i>=0) ? i: i+cnt;
		(*_colors)[j] = c;
		(*_colors)[j][3] = minblend + (1.0f - (k/(float)cnt)) * (maxblend - minblend);
	}

	(*_colors)[ndx+1] = (*_colors)[ndx+2] = osg::Vec4(0,0,0,0);
}


}