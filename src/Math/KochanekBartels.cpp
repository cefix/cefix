/*
 *  KochanekBartels.cpp
 *  Splines
 *
 *  Created by Stephan Huber on 29.08.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "KochanekBartels.h"

namespace cefix { 
namespace spline {



void KochanekBartels::calculateTangents(float tension, float continuity, float bias) 
{

		osg::Vec3 positionA;
		osg::Vec3 positionB;
		osg::Vec3 t;
		float a, b;
		unsigned int max = _segments->size()-1;

		positionA = (*_segments)[0].getEnd().position - (*_segments)[0].getBegin().position;
		
		(*_segments)[0].setBeginTangent(positionA);

		// REWRITTEN WITHOUT TESTED
		for(register unsigned int i = 0; i < max; ++i)
		{
			positionA = (*_segments)[i].getEnd().position - (*_segments)[i].getBegin().position;
			positionB = (*_segments)[i+1].getEnd().position - (*_segments)[i+1].getBegin().position;
						
			// start tangent:
			a = 0.5f * (1.0f-tension) * (1.0f+continuity) * (1.0f+bias);
			b = 0.5f * (1.0f-tension) * (1.0f-continuity) * (1.0f-bias);
			
			t = positionA*a + positionB*b;
			
			(*_segments)[i+1].setBeginTangent(t);
			
			// end tangent
			
			a = 0.5f * (1.0f-tension) * (1.0f-continuity) * (1.0f+bias);
			b = 0.5f * (1.0f-tension) * (1.0f+continuity) * (1.0f-bias);
			
			
			t = positionA*a + positionB*b;
			(*_segments)[i].setEndTangent(t);
		}
		
		positionA = (*_segments)[max].getBegin().tangent;
		(*_segments)[max].setEndTangent(positionA);
	}



}
}