/*
 *  Bezier.cpp
 *  Splines
 *
 *  Created by Stephan Huber on 29.08.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Bezier.h"

namespace cefix { 
namespace spline {

static inline unsigned int faculty(unsigned int x)
{
	if(x == 0)
		return 1;

	unsigned int ans = 1;

	do 
	{
		ans *= x;
	}while(--x);

	return ans;
}

// ---------------------------------------------------------------------
static inline double pow(double x, unsigned int n)
{
	if(n == 0)
		return 1.0;
	if(n == 1)
		return x;

	--n;

	float m = x;

	do
	{
		x *= m;
	}while(--n);

	return x;
}


static inline double getBernsteinValue(double t, unsigned int i,  int max) {
	return (faculty(max) * pow(t, i) * pow(1.0-t, max-i)) /
			   (faculty(i) * faculty(max-i));
}


Bezier::Bezier(ControlPoint::List* list) : Curve(list)
{
	calculateLength(1000.0);
}


// -------------------------------------------------------------------------
void Bezier::getWorldPosition(osg::Vec3& position, double distance, unsigned int& segment)
{
	getPositionOnSegment(position, distance / _length, 0);
}

// -------------------------------------------------------------------------
void Bezier::getTangent(osg::Vec3& tangent, double distance)
{
	double resolution = 0.01f;
	osg::Vec3 position_a;
	osg::Vec3 position_b;
	unsigned int segment(0);
	getWorldPosition(position_a, distance + resolution, segment );
	getWorldPosition(position_b, distance - resolution, segment );

	position_a -= position_b;
	
	position_a.normalize();

	tangent = position_a;
}

// -------------------------------------------------------------------------
void Bezier::getPositionOnSegment(osg::Vec3& position, double distance, unsigned int segment)
{
	position.set(0,0,0);

	double b;
	unsigned int m = _segments->size();
	
	for(unsigned int i = 0; i < m ; ++i)
	{
		b = getBernsteinValue(distance, i , m);
		position += ((*_segments)[i].getBegin().position * b);
	}

	b = getBernsteinValue(distance, m,m);
	position += ((*_segments)[_segments->size()-1].getEnd().position * b);
}

// -------------------------------------------------------------------------
void Bezier::getTangentOnSegment(osg::Vec3& tangent, double distance, unsigned int segment)
{
}




}
}
