/*
 *  Curve.cpp
 *  Splines
 *
 *  Created by Stephan Huber on 28.08.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */
#include <osg/Depth>
#include <osg/Point>
#include "Curve.h"

namespace cefix { 
namespace spline {


// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

Curve::Curve(ControlPoint::List* list) : 
	osg::Referenced(),
	_segments(NULL), 
	_length(0)
{
	setPoints(list);
}



// ----------------------------------------------------------------------------------------------------------
// setPoints
// ----------------------------------------------------------------------------------------------------------

void Curve::setPoints(ControlPoint::List* list)
{
	if(!_segments.valid()) 
		_segments = new Segment::List();
	_segments->resize(list->size()-1);
	
	for(unsigned int i = 0; i < list->size()-1; ++i) 
	{
		(*_segments)[i] = Segment((*list)[i], (*list)[i+1]);
	}
}


// ----------------------------------------------------------------------------------------------------------
// calculateLength
// ----------------------------------------------------------------------------------------------------------

void Curve::calculateLength(float resolution)
{
	osg::Vec3 positionA;
	osg::Vec3 positionB;

	_length = 0;
	double stepLength = 1.0 / resolution;

	for(unsigned int i = 0; i < _segments->size(); ++i)
	{
		double segmentLength = 0;

		for(float j = 0; j < 1.0f; j += stepLength)
		{
			if(j > (1.0f - stepLength))
				j = 1.0f - stepLength;

			getPositionOnSegment(positionA, j + stepLength, i);
			getPositionOnSegment(positionB, j, i);
			

			segmentLength += (positionA-positionB).length();
		}

		(*_segments)[i].setLength(segmentLength);
		(*_segments)[i].setLengthFromBeginning(_length);

		_length += segmentLength;
	}
}
	

// ----------------------------------------------------------------------------------------------------------
// updatedebugGeometry
// ----------------------------------------------------------------------------------------------------------
	
void Curve::updateDebugGeometry() {

	if (_debugGeometry->getNumPrimitiveSets() > 0)
		_debugGeometry->removePrimitiveSet(0, _debugGeometry->getNumPrimitiveSets());
	osg::Geometry* geo = _debugGeometry.get();
	
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::DrawElementsUShort* tangentlines = new osg::DrawElementsUShort(osg::PrimitiveSet::LINES);
	osg::DrawElementsUShort* tangentbeginpoints = new osg::DrawElementsUShort(osg::PrimitiveSet::POINTS);
	osg::DrawElementsUShort* tangentendpoints = new osg::DrawElementsUShort(osg::PrimitiveSet::POINTS);
	osg::DrawElementsUShort* controlpoints = new osg::DrawElementsUShort(osg::PrimitiveSet::POINTS);
	unsigned int ndx = 0;
	for(unsigned int i = 0; i < _segments->size(); ++i)
	{
		
		// Start+EndPunkte
		ndx = vertices->size();
		vertices->push_back((*_segments)[i].getBegin().position);
		vertices->push_back((*_segments)[i].getEnd().position);
		
		controlpoints->push_back(ndx);
		controlpoints->push_back(ndx+1);
		
		// Tangenten
		
		ndx = vertices->size();
		vertices->push_back((*_segments)[i].getBegin().position + (*_segments)[i].getBegin().tangent);
		vertices->push_back((*_segments)[i].getEnd().position - (*_segments)[i].getEnd().tangent);
		
		tangentbeginpoints->push_back(ndx);
		tangentendpoints->push_back(ndx+1);
		
		
		tangentlines->push_back(ndx -2);
		tangentlines->push_back(ndx);
		
		tangentlines->push_back(ndx - 1);
		tangentlines->push_back(ndx + 1);
		
	}
	
	geo->addPrimitiveSet(tangentbeginpoints);
	geo->addPrimitiveSet(tangentendpoints);
	geo->addPrimitiveSet(tangentlines);
	geo->addPrimitiveSet(controlpoints);
	
	osg::Vec4Array* colors = new osg::Vec4Array(4);
	
	(*colors)[0] = osg::Vec4(0,1,0,1);
	(*colors)[1] = osg::Vec4(1,0,0,1);
	(*colors)[2] = osg::Vec4(0,0,1,1.0f);
	(*colors)[3] = osg::Vec4(1,1,1,1);
	geo->setColorArray(colors);
	geo->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);
	
	geo->setVertexArray(vertices);
	
	osg::Depth* depth = new osg::Depth();
	depth->setWriteMask(false);
	geo->getOrCreateStateSet()->setAttribute(depth);
	osg::Point* point = new osg::Point;
	point->setSize(4);
	
	geo->getOrCreateStateSet()->setAttribute(point);
	geo->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);	
	geo->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);	
	geo->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);	

}
	

}
}