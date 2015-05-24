/*
 *
 *      (\(\
 *     (='.')           cefix --
 *    o(_")")           a multipurpose library on top of OpenSceneGraph
 *  
 *
 *                      Copyright 2001-2011, stephanmaximilianhuber.com 
 *                      Stephan Maximilian Huber. 
 *
 *                      All rights reserved.
 *
 */

#ifndef CEFIX_SPLINE_GEOMETRY_HEADER
#define CEFIX_SPLINE_GEOMETRY_HEADER

#include <osg/Geometry>
#include "Spline.h"

namespace cefix {

/** geometry class which can render splines */

class SplineGeometry : public osg::Geometry 
{
public:

	enum RenderMode {
		Normal, /// accurate render mode
		Approximated /// approximated render mode
	};
	
	/// ctor
	SplineGeometry(SplineMode mode = BezierSpline, RenderMode rendermode = Normal);
	
	/// get the mode of the spline: CatMull, BSpline, Bezier, etc
	SplineMode getMode() const { return _spline.getMode(); }
	/// set the mode of the spline
	void setMode(SplineMode mode) { _spline.setMode(mode); dirtySpline(); }
	/// set the rendermode
	void setRenderMode(RenderMode mode) { _renderMode = mode; dirtySpline(); }
	/// get the render mode
	RenderMode getRenderMode() const { return _renderMode; }
	
	/// set max error for approximated render-mode
	void setMaxError(double err) { _spline.setMaxError(err); }
	/// get max error for approximated render-mode
	double getMaxError() const { return _spline.getMaxError(); }
	
	/// get the list of controlpoints
	Spline3D::ControlPoints& getControlPoints() { return _spline.controlPoints; }
	/// get the list of controlpoints
	const Spline3D::ControlPoints& getControlPoints() const { return _spline.controlPoints; }
	
	void addControlPoint(const osg::Vec3& p) {_spline.controlPoints.push_back(p); dirtySpline(); }
    void setControlPoint(unsigned int ndx, const osg::Vec3& p) { _spline.controlPoints[ndx] = p; dirtySpline(); }
	
	/// get the spline-object
	Spline3D& getSpline() { return _spline; }
	/// get the spline object
	const Spline3D& getSpline() const { return _spline; }
	
	/// update rendering, bbox and displaylists
	void update() {
		if (_splineIsDirty) {
			_splineIsDirty = false;
			updateRendering();
		}
	}
	
	/// set subdivision for normal render-mode
	void setSubdivision( double d) { _subdivision = d; dirtySpline(); }
	/// get subdivision
	double getSubdivision() const { return _subdivision; }
	
	/// if true, the spline is closed
	void setSplineClosed(bool c) { _spline.setSplineClosed(c); }
	/// return true, if spline is closed
	bool getSplineClosed() const { return _spline.getSplineClosed(); }
	
	void markDirty() { dirtySpline(); }
    
    void setSpline(const Spline3D& spline) { _spline = spline; dirtySpline(); }
	
protected:
	void dirtySpline() { _splineIsDirty = true; }
	
	virtual void updateRendering();
	
	Spline3D	_spline;
	bool		_splineIsDirty;
	RenderMode	_renderMode;
	double		_subdivision;
	
	osg::Vec3Array* _vertices;
	osg::DrawArrays* _da;
	
};


}

#endif