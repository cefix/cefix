/*
 *  Cardinal.cpp
 *  Splines
 *
 *  Created by Stephan Huber on 29.08.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Cardinal.h"


namespace cefix { 
namespace spline {


Cardinal::Cardinal(ControlPoint::List* controlPoints, float tangentStrength) :
	Hermite(controlPoints),
	_tangentStrength(tangentStrength)
{
	calculateTangents(tangentStrength);
	calculateLength(1000.0);
}

// -------------------------------------------------------------------------
void Cardinal::calculateTangents(float strength)
{
	osg::Vec3 tangent;
	unsigned int max = _segments->size() - 1;
	
	tangent = (*_segments)[0].getEnd().position - (*_segments)[0].getBegin().position;
	(*_segments)[0].setBeginTangent(tangent * strength);

	for(register unsigned int i = 0; i < max; ++i)
	{
		tangent = ((*_segments)[i+1].getEnd().position  - (*_segments)[i].getBegin().position) * strength;
		
		(*_segments)[i].setEndTangent(tangent);
		(*_segments)[i+1].setBeginTangent(tangent);
		//(*_segments)[i].setBeginTangent(tangent);
		
		
		
	}
	tangent = (*_segments)[max].getEnd().position - (*_segments)[max].getBegin().position;
	(*_segments)[max].setEndTangent(tangent * strength);
}

}
}