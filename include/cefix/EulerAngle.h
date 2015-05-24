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

#ifndef EULER_ANGLE_HEADER
#define EULER_ANGLE_HEADER

#include <osg/Quat>
#include <osg/Vec3>

namespace cefix {

class Serializer;

struct EulerAngle {

	EulerAngle(double in_rho, double in_phi, double in_theta) : rho(in_rho), phi(in_phi), theta(in_theta) { updateQuat(); }
	EulerAngle() : rho(0), phi(0), theta(0) { updateQuat(); }
    EulerAngle(const osg::Vec3& p) { set(p); }
    
	double rho, phi, theta;
	osg::Quat quat;
	
	EulerAngle toRadians() { return EulerAngle(osg::DegreesToRadians(rho), osg::DegreesToRadians(phi), osg::DegreesToRadians(theta)); }
	EulerAngle toDegrees() { return EulerAngle(osg::RadiansToDegrees(rho), osg::RadiansToDegrees(phi), osg::RadiansToDegrees(theta)); }
	
	void readFrom(cefix::Serializer& s);
	void writeTo(cefix::Serializer& s);
	
	inline void set(const osg::Vec3& v) { rho = v[0]; phi = v[1]; theta = v[2]; updateQuat(); }
	inline void set(double in_rho, double in_phi, double in_theta) { rho = in_rho; phi = in_phi; theta = in_theta; updateQuat(); }

	void updateQuat();
	
};

}

#endif