/*
 *  Hermite.cpp
 *  Splines
 *
 *  Created by Stephan Huber on 29.08.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Hermite.h"

namespace cefix { 
namespace spline {




// -------------------------------------------------------------------------
void Hermite::getWorldPosition(osg::Vec3& position, double distance, unsigned int& segment)
{
	unsigned int max = _segments->size() - 1;
	do 
	{
		if(segment > max)
		{
			getPositionOnSegment(position, (*_segments)[max].getLengthFromBeginning() / (*_segments)[max].getLength(), max);
			return;
		}

		if(((*_segments)[segment].getLengthFromBeginning() + (*_segments)[segment].getLength()) >= distance)
			break;
	}while(++segment);

	getPositionOnSegment(position, (distance - (*_segments)[segment].getLengthFromBeginning()) / (*_segments)[segment].getLength(), segment);
}

// -------------------------------------------------------------------------
void Hermite::getTangent(osg::Vec3& tangent, double distance)
{
	unsigned int segment = 0;
	unsigned int max = _segments->size() - 1;
	
	do 
	{
		if(segment > max)
		{
			getTangentOnSegment(tangent, (*_segments)[max].getLengthFromBeginning() / (*_segments)[max].getLength(), max);
			return;
		}

		if(((*_segments)[segment].getLengthFromBeginning() + (*_segments)[segment].getLength()) >= distance)
			break;
	}while(++segment);

	getTangentOnSegment(tangent, (distance - (*_segments)[segment].getLengthFromBeginning()) / (*_segments)[segment].getLength(), segment);	
}

// -------------------------------------------------------------------------
void Hermite::getPositionOnSegment(osg::Vec3& position,	double distance,	unsigned int segment)
{
	float d2 = distance * distance;
	float fH1 = (2.0f * distance - 3.0f) * d2 + 1.0f;
	float fH2 = (distance - 2.0f) * d2 + distance;
	float fH3 = (d2 - distance) * distance;
	float fH4 = (-2.0f * distance + 3.0f) * d2;

	ControlPoint begin = (*_segments)[segment].getBegin();
	ControlPoint end   = (*_segments)[segment].getEnd();

	position = begin.position * fH1 + begin.tangent * fH2 + end.tangent * fH3 + end.position * fH4;
	/*
	position[0] = begin.position.x() * fH1 +
		begin.tangent.x()  * fH2 +
		end.tangent.x()    * fH3 +
		end.position.x()   * fH4;
	position[1] = begin.position.y() * fH1 +
		begin.tangent.y  * fH2 +
		end.tangent.y    * fH3 +
		end.>position.y   * fH4;
	position[2] = begin->position.z * fH1 +
		begin->tangent.z  * fH2 +
		end->tangent.z    * fH3 +
		end->position.z   * fH4;
	
	*/
}

// -------------------------------------------------------------------------
void Hermite::getTangentOnSegment(osg::Vec3& tangent, double distance, unsigned int segment)
{
	float fH1 = ( 6.0f * distance - 6.0f) * distance;
	float fH2 = ( 3.0f * distance - 4.0f) * distance + 1.0f;
	float fH3 = ( 3.0f * distance - 2.0f) * distance;
	float fH4 = (-6.0f * distance + 6.0f) * distance;

	const ControlPoint& begin = (*_segments)[segment].getBegin();
	const ControlPoint& end   = (*_segments)[segment].getEnd();

	tangent = begin.position * fH1 + begin.tangent * fH2 + end.tangent * fH3 + end.position * fH4;
	tangent.normalize();
	
	/*
	tangent[0] = begin->position.x * fH1 +
		begin->tangent.x  * fH2 +
		end->tangent.x    * fH3 +
		end->position.x   * fH4;
	tangent[1] = begin->position.y * fH1 +
		begin->tangent.y  * fH2 +
		end->tangent.y    * fH3 +
		end->position.y   * fH4;
	tangent[2] = begin->position.z * fH1 +
		begin->tangent.z  * fH2 +
		end->tangent.z    * fH3 +
		end->position.z   * fH4;

	Math::normalize(tangent);
	*/
	
}
	
	
}
}
