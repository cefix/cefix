/*
 *  SplineGeomtry.cpp
 *  Splines
 *
 *  Created by Stephan Huber on 02.08.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "SplineGeometry.h"
#include <cefix/DebugGeometryFactory.h>
#include <cefix/StringUtils.h>

namespace cefix {


SplineGeometry::SplineGeometry(SplineMode mode, RenderMode rendermode)
:	osg::Geometry(),
	_spline(),
	_splineIsDirty(true),
	_renderMode(rendermode),
	_subdivision(50.0)
{
	setMode(mode);
	_vertices = new osg::Vec3Array();
	setVertexArray(_vertices);
	_da = new osg::DrawArrays(GL_LINE_STRIP, 0, 0);
	addPrimitiveSet(_da);
}
	
void SplineGeometry::updateRendering()
{
	
	/*
	for(unsigned int i= 0; i < _spline.controlPoints.size(); ++i) {
		cefix::DebugGeometryFactory::get("spline")->addPoint(_spline.controlPoints[i], osg::Vec4(1,0,0,1));
		cefix::DebugGeometryFactory::get("spline")->addText(_spline.controlPoints[i]+osg::Vec3(10,0,0), cefix::intToString(i), osg::Vec4(1,0,0,1));
	}
	cefix::DebugGeometryFactory::get("spline")->finish();
	*/
	
	if (_spline.controlPoints.size() < 3) return;
	
	
	_spline.buildArclenTable();     // build the arc length table
	_spline.normalizeArclenTable();
	 
	if (_renderMode == Approximated) {
		_vertices->reserve(_spline.arclentable.size());
		_vertices->resize(0);
		Spline3D::ArclenTable::iterator ai= _spline.arclentable.begin();
		while (ai !=_spline.arclentable.end())
		{
			_vertices->push_back(ai->pos);
			++ai;
		}
	}
	else {
		int startDelta = _spline.getStartDelta();
		int endDelta   = _spline.getEndDelta();
		
		unsigned int max= _spline.controlPoints.size() - endDelta;		
		unsigned int size= _spline.controlPoints.size();
		_vertices->reserve(_subdivision * max);
		_vertices->resize(0);
		for (unsigned int i= startDelta; i < max; i+= _spline.getStep()) {
			_vertices->push_back( _spline.getPointOnSpline(
				_spline.controlPoints[i%size], 
				_spline.controlPoints[(i+1)%size], 
				_spline.controlPoints[(i+2)%size], 
				_spline.controlPoints[(i+3)%size], 0));
				
			double stepsize = 1/_subdivision;
			
			for (double t= stepsize; t < 1; t+=stepsize)
			{
				_vertices->push_back(_spline.getPointOnSpline(
					_spline.controlPoints[i%size], 
					_spline.controlPoints[(i+1)%size], 
					_spline.controlPoints[(i+2)%size], 
					_spline.controlPoints[(i+3)%size], t));
			}
		}
	}
	
	_da->setCount(_vertices->size());
	dirtyDisplayList();
	dirtyBound();
	_vertices->dirty();
}

}