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
 
 /* loosely based on:
 ****************************************
 * Spline Class
 * By Bill Perone (billperone@yahoo.com)
 * Original: 21-10-2003
 * Revised:  05-11-2003
 *
 * This code is provided "as is" and you can use it freely as long as 
 * credit is given to Bill Perone in the application it is used in
 *
 * Dependancies: matrix4, vector2 class, STL vector, STL list
 * 
 ****************************************/


#ifndef CEFIX_SPLINE_HEADER
#define CEFIX_SPLINE_HEADER



#include <vector>
#include <list>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Matrix>
#include <cefix/Serializer.h>


#ifdef WIN32
#pragma warning(disable:4786)			// STL expands to beyond 256 chars in windows
#endif



namespace cefix {

enum SplineMode { BezierSpline, HermiteSpline, CatMullRomSpline, BSpline }; //!< default mode is bezier


template <class Vector>
class Spline
{
public:
	typedef Vector value_type;
	typedef std::vector<Vector> ControlPoints;
	
	struct ArclenTableEntry
	{
		float    time;   //!< the timestamp
		float    arclen; //!< the arclength at this time (monotonically growing in the table)
		Vector   pos;    //!< the space curve position at this point
	};

	
protected:
	SplineMode		_mode;   //!< the current spline mode
	osg::Matrix		_basis;  //!< the current basis matrix
	double			_maxError;
	unsigned int    _startDelta, _endDelta;
	bool			_closedSpline;

	unsigned int	_step; //!< the stepping value for control point interpolation

	//! returns nearest index into the arclength table matching arc_val
	inline int getArcTableBisectionSearch(float arc_val) const;
    
    //! returns nearest index into the arclength table matching time
	inline int getArcTableBisectionSearchForTime(float t) const;


public:
	ControlPoints  controlPoints;	//!< the control points
	
	typedef std::vector<ArclenTableEntry> ArclenTable;
	ArclenTable arclentable; //!< the arclength table	


	void setMode(SplineMode m); //!< set the spline mode 	
	SplineMode getMode() const  //!< get the current spline mode
	{  return _mode;  }

	unsigned int getStep() const { return _step; } //!< returns the stepping value
	
	Spline(SplineMode mode = BezierSpline) { setMode(mode); setMaxError(0.001); setSplineClosed(false); }
	virtual ~Spline() {}

	//! returns the interpolated value of the spline at a time value in a particular dimension
	inline Vector getPointOnSpline(const Vector& p1, const Vector& p2, const Vector& p3, const Vector& p4, float t);

	//! normalizes the arclength table so that the final arclength = 1
	void  normalizeArclenTable(void)
	{
		for (unsigned int i= 0; i < arclentable.size(); ++i)
			 arclentable[i].arclen/=arclentable.back().arclen;	
	}

	//! returns the time on the space curve given a partial arclength value
	float getArcTime(float arc_val) const; 
	

	
	inline Vector getApproximatedPointOnSpline(float t) const;
    inline Vector getApproximatedPointOnSplineByArcLen(float arclen) const;

	
	double getMaxError() const { return _maxError; }  
	void setMaxError(double err) { _maxError = err; }
	
	void setSplineClosed(bool c) { _closedSpline = c; }
	bool getSplineClosed() const { return _closedSpline; }
	
	int getStartDelta() const { return (_closedSpline) ? 0 : _startDelta; }
	int getEndDelta() const { return (_closedSpline) ? _step-1 : _endDelta; }
	
	void writeTo(Serializer& serializer);
	void readFrom(Serializer& serializer);
		
};


//! 1D spline class
class Spline1D: public Spline<float>
{	
	
private:
	std::list<ArclenTableEntry>::iterator internalBuildArclenTable(std::list<ArclenTableEntry> &templist, std::list<ArclenTableEntry>::iterator curr, unsigned int ctrlpt_offset);

public:
	void  buildArclenTable();     	
	float getArcPoint(float arc_val);
	
	void readFrom(Serializer& serializer) {
		Spline<float>::readFrom(serializer);
		buildArclenTable();
		normalizeArclenTable();
	}
	
	void writeTo(Serializer& serializer) {
		Spline<float>::writeTo(serializer);
	}
};


//! 2D spline class
class Spline2D: public Spline<osg::Vec2>
{		
private:
	std::list<ArclenTableEntry>::iterator internalBuildArclenTable(std::list<ArclenTableEntry> &templist, std::list<ArclenTableEntry>::iterator curr, unsigned int ctrlpt_offset);

public:
	void  buildArclenTable();     	
	osg::Vec2 getArcPoint(float arc_val);
	
	void readFrom(Serializer& serializer) {
		Spline<osg::Vec2>::readFrom(serializer);
		buildArclenTable();
		normalizeArclenTable();
	}
	
	void writeTo(Serializer& serializer) {
		Spline<osg::Vec2>::writeTo(serializer);
	}
	
};

//! 2D spline class
class Spline3D: public Spline<osg::Vec3>
{		
private:
	std::list<ArclenTableEntry>::iterator internalBuildArclenTable(std::list<ArclenTableEntry> &templist, std::list<ArclenTableEntry>::iterator curr, unsigned int ctrlpt_offset);

public:
	void  buildArclenTable();     	
	osg::Vec3 getArcPoint(float arc_val);
	
	void readFrom(Serializer& serializer) {
		Spline<osg::Vec3>::readFrom(serializer);
		buildArclenTable();
		normalizeArclenTable();	
	}
	
	void writeTo(Serializer& serializer) {
		Spline<osg::Vec3>::writeTo(serializer);
	}
};


// basis matrices


// B= [P(i) P(i+1) P(i+2) P(i+3)
// C0 continuous, if tangents are aligned then C1 continuous
// step = 3
static osg::Matrix MatBezier(-1, 3, -3, 1,
						  3, -6, 3, 0,
						 -3, 3, 0, 0,
						  1, 0, 0, 0);						  

// B= [P(i) P(i+1) P'(i) P'(i+1)]
// C0 continuous
// step = 1
static osg::Matrix MatHermite  (2, -2, 1, 1,
						   -3, 3, -2, -1,
							0, 0, 1, 0,
							1, 0, 0, 0);

// B= [P(i-1) P(i) P(i+1) P(i+2)]
// C0 continuous
// curve guaranteed to go through all control points
// each 4 consecutive control points define a curve (ex: 0123, 1234, 2345...)
// step = 1


static osg::Matrix MatCatMullRom(-.5f,  1.5f,  -1.5f,  .5f,
									1, -2.5f,      2, -.5f,
								 -.5f,     0,    .5f,    0,
									0,     1,      0,    0);

/*static osg::Matrix MatCatMullRom(-.5f,   1.f,  -0.5f,  0.f,
								 1.5f, -2.5f,      0,    1,
							    -1.5f,     2,    .5f,    0,
								  .5f,  -.5f,      0,    0);
*/
// B= [P(i) P(i+1) P(i+2) P(i+3)
// C2 continuous
// each 4 consecutive control points define a curve (ex: 0123, 1234, 2345...)
// step = 1
static osg::Matrix MatBSpline(-1.f/6,  3.f/6, -3.f/6, 1.f/6,  
						   3.f/6, -6.f/6,  3.f/6, 0,
						  -3.f/6,      0,  3.f/6, 0,
						   1.f/6,  4.f/6,  1.f/6, 0);



// returns the interpolated value of a control point in a particular dimension
template <class Vector>
inline Vector Spline<Vector>::getPointOnSpline(const Vector& p1, const Vector& p2, const Vector& p3, const Vector& p4, float t)
{
	
	float t2, t3;
	osg::Matrix::value_type *ptr;
	
	t2= t*t; // precalculations
	t3= t2*t;	
		
	ptr= _basis.ptr();	
	--ptr;
	
	/*return (p1**++ptr + p2**++ptr + p3**++ptr + p4**++ptr) * t3 + 
		   (p1**++ptr + p2**++ptr + p3**++ptr + p4**++ptr) * t2 + 
		   (p1**++ptr + p2**++ptr + p3**++ptr + p4**++ptr) * t + 
		      (p1**++ptr + p2**++ptr + p3**++ptr + p4**++ptr);*/		
	return p1 * (_basis(0,0)*t3 + _basis(1,0)*t2 + _basis(2,0)*t + _basis(3,0)) + 
	 	   p2 * (_basis(0,1)*t3 + _basis(1,1)*t2 + _basis(2,1)*t + _basis(3,1)) + 
		   p3 * (_basis(0,2)*t3 + _basis(1,2)*t2 + _basis(2,2)*t + _basis(3,2)) + 
		   p4 * (_basis(0,3)*t3 + _basis(1,3)*t2 + _basis(2,3)*t + _basis(3,3));	
	/*return t3*(basis[0][0]*p1 + basis[0][1]*p2 + basis[0][2]*p3 + basis[0][3]*p4) + 
		   t2*(basis[1][0]*p1 + basis[1][1]*p2 + basis[1][2]*p3 + basis[1][3]*p4) + 
		   t* (basis[2][0]*p1 + basis[2][1]*p2 + basis[2][2]*p3 + basis[2][3]*p4) + 
		      (basis[3][0]*p1 + basis[3][1]*p2 + basis[3][2]*p3 + basis[3][3]*p4);*/
}


template <class Vector>
inline int Spline<Vector>::getArcTableBisectionSearch(float arc_val) const
{
	unsigned int upper, middle, lower;
	bool ascnd;

	lower= 0;
	upper= arclentable.size();
	ascnd= (arclentable.back().arclen >= arclentable.front().arclen);
	while (upper-lower > 1)
	{
		middle= (upper+lower) >> 1;
		if ((arc_val >= arclentable[middle].arclen) == ascnd) lower= middle;
										               else upper= middle;
	}

	return lower;
}


template <class Vector>
inline int Spline<Vector>::getArcTableBisectionSearchForTime(float t) const
{
	unsigned int upper, middle, lower;
	bool ascnd;

	lower= 0;
	upper= arclentable.size();
	ascnd= (arclentable.back().time >= arclentable.front().time);
	while (upper-lower > 1)
	{
		middle= (upper+lower) >> 1;
		if ((t >= arclentable[middle].time) == ascnd) lower= middle;
										               else upper= middle;
	}

	return lower;
}


template <class Vector>
inline void Spline<Vector>::setMode(SplineMode m)
{
	_mode= m;
	switch(_mode)
	{
	case BezierSpline:      _step= 3; _startDelta = 0; _endDelta = 2; _basis= MatBezier; break;
	case HermiteSpline:     _step= 1; _startDelta = 1; _endDelta = 2; _basis= MatHermite; break;
	case CatMullRomSpline:  _step= 1; _startDelta = 0; _endDelta = 3; _basis= MatCatMullRom; break;
	case BSpline:			_step= 1; _startDelta = 0; _endDelta = 3; _basis= MatBSpline; break;
	}
}


template <class Vector>
float Spline<Vector>::getArcTime(float arc_val) const
{
	// trivial cases
	if (arc_val==arclentable.front().arclen) return arclentable.front().time;
	else 
	if (arc_val==arclentable.back().arclen) return arclentable.back().time;
	
	unsigned int lower= getArcTableBisectionSearch(arc_val);	
		
	// perform a linear interpolation of 2 nearest time values
	return ( (arclentable[lower+1].arclen-arc_val)*arclentable[lower].time + 
		     (arc_val-arclentable[lower].arclen)*arclentable[lower+1].time ) / 
			 (arclentable[lower+1].time - arclentable[lower].time);
}


template <class Vector>
inline Vector Spline<Vector>::getApproximatedPointOnSpline(float t) const
{
	if (t==arclentable.front().time) return arclentable.front().pos;
	else 
	if (t==arclentable.back().time) return arclentable.back().pos;
		
	unsigned int lower= getArcTableBisectionSearchForTime(t);	
		
	// perform a linear interpolation of 2 nearest pos values
	
	return cefix::interpolate(arclentable[lower+1].pos, arclentable[lower].pos, (arclentable[lower+1].time-t) / (arclentable[lower+1].time - arclentable[lower].time));

}

template <class Vector>
inline Vector Spline<Vector>::getApproximatedPointOnSplineByArcLen(float arclen) const
{
	if (arclen==arclentable.front().arclen) return arclentable.front().pos;
	else 
	if (arclen==arclentable.back().arclen) return arclentable.back().pos;
		
	unsigned int lower= getArcTableBisectionSearch(arclen);	
		
	// perform a linear interpolation of 2 nearest pos values
	
	return cefix::interpolate(arclentable[lower+1].pos, arclentable[lower].pos, (arclentable[lower+1].arclen-arclen) / (arclentable[lower+1].arclen - arclentable[lower].arclen));

}


template <class Vector>
void Spline<Vector>::writeTo(Serializer& serializer) 
{
	serializer << "mode" << static_cast<int>(_mode);
	serializer << "step" << _step;
	serializer << "closed" << _closedSpline;
	serializer << "max_error" << _maxError;
	serializer << "control_points" << controlPoints;
}

template <class Vector>
void Spline<Vector>::readFrom(Serializer& serializer)
{
	int mode;
	serializer >> "mode" >> mode;
	serializer >> "step" >> _step;
	serializer >> "closed" >> _closedSpline;
	serializer >> "max_error" >> _maxError;
	serializer >> "control_points" >> controlPoints;
	setMode(static_cast<SplineMode>(mode));
}

}

#endif
