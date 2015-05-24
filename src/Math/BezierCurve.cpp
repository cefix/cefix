/*
 *  BezierCurve.cpp
 *  NativeContext
 *
 *  Created by Stephan Huber on Sun Sep 15 2002.
 *  Copyright (c) 2002 digital mind. All rights reserved.
 *
 */
#include <cefix/Log.h>
#include <cefix/BezierCurve.h>

namespace cefix {

// ---------------------------------------------------------------------------
// computeVecArray
// berechnet die BezierCurve und fŸllt die Punkte in das VecArray
// ---------------------------------------------------------------------------

void BezierCurve::computeVecArray(Vec3Array* array,int steps, bool fAddFirstPoint)
{

	float stepSize = 1.0 / steps;
	float t = stepSize;
	
	// First control point is on the curve
	if (fAddFirstPoint)
		array->push_back(_start);
	
	// Calculate intermediate points
	for(int step = 1; step < steps; step++, t += stepSize) {
		// Calculate the blending functions
		array->push_back( compute(t) );
	}
	// Last control point is on the curve
	array->push_back(_end);
	
}


void BezierCurve::_computeVecArrayBySubDivision(Vec3Array* array, Vec3 start, Vec3 end, Vec3 handle1, Vec3 handle2, float maxDistance) 
	{

	// siehe http://graphics.cs.ucdavis.edu/CAGDNotes/Cubic-Bezier-Curves/Cubic-Bezier-Curves.html

	
	// Abstand der Punkte keine maxDistance?
	float distance = (end-start).length();
	if (distance<=maxDistance) {
		array->push_back(start);
		array->push_back(end);
		return;
	}
	#ifdef DEBUG
		osg::notify(osg::DEBUG_FP) << "cubic bezier, distance: "<< distance << " dividing more" << endl;

		osg::notify(osg::DEBUG_FP) << "left       :" << start   << " right      : " << end << endl;
		osg::notify(osg::DEBUG_FP) << "leftHandle :" << handle1 << " rightHandle: " << handle2 << endl;
	#endif
	float t = 0.5;

	Vec3 leftStart,leftEnd,leftHandle1,leftHandle2;
	Vec3 rightStart,rightEnd,rightHandle1,rightHandle2;

	Vec3 p11 = (handle1+start) * t;
	Vec3 p31 = (handle2+end) * (1-t);
	Vec3 p21 = (handle2+handle1) * t;
	Vec3 p22 = (p21 + p11) * t;
	Vec3 p32 = (p21 + p31) * (1-t);
	Vec3 p33 = (p32 + p22) * t;

	leftStart = start;
	leftHandle1 = p11;
	leftHandle2 = p22;
	leftEnd = p33;

	rightStart = p33;
	rightHandle1 = p32;
	rightHandle2 = p31;
	rightEnd = end;

	_computeVecArrayBySubDivision(array,leftStart,leftEnd,leftHandle1,leftHandle2,maxDistance);
	_computeVecArrayBySubDivision(array,rightStart,rightEnd,rightHandle1,rightHandle2,maxDistance);
	
	
	}


// ---------------------------------------------------------------------------
// computeVecArray
// berechnet die BezierCurve und fŸllt die Punkte in das VecArray
// ---------------------------------------------------------------------------
void BezierCurve::computeVecArrayBySubdivision(Vec3Array* array, float maxDistance, bool fAddFirstPoint)
{

	Vec3Array* resultArray;
	Vec3Array* tempArray;
	resultArray = new Vec3Array;
	tempArray = new Vec3Array;
	_computeVecArrayBySubDivision(resultArray,_start,_end,_controlStart,_controlEnd,maxDistance);
	// this is very scheisse hier:
	// it costs performance, but we need to bring the points in the right order and remove duplicates
	for (unsigned int i= 0;i<resultArray->size();i++) {
		bool found = 0;
		Vec3 v = (*resultArray)[i];
		for (unsigned int j = 0;j<tempArray->size(); j++) {
			if (v==(*tempArray)[j]) {
				found = true;
				j=tempArray->size();
			}
		}
		if (!found) {
			tempArray->push_back(v);
            if ((i>0) || (fAddFirstPoint))
                array->push_back(v);
		}
	}
}



}
