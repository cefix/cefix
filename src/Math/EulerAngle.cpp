/*
 *  EulerAngle.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 06.11.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "EulerAngle.h"
#include <cefix/Serializer.h>


namespace cefix {

void EulerAngle::updateQuat()
{
    /*
	osg::Matrix m = 
        osg::Matrix::rotate(rho,1.0,0.0,0.0) * 
        osg::Matrix::rotate(phi,0.0,1.0,0.0) *
        osg::Matrix::rotate(theta,0.0,0.0,1.0);
	quat = m.getRotate();
	*/
	quat.makeRotate(rho, osg::X_AXIS, phi, osg::Y_AXIS, theta, osg::Z_AXIS);
}

void EulerAngle::readFrom(cefix::Serializer& s)
{
	s >> "rho" >> rho;
	s >> "phi" >> phi;
	s >> "theta" >> theta;
	
	rho = osg::DegreesToRadians(rho);
	phi = osg::DegreesToRadians(phi);
	theta = osg::DegreesToRadians(theta);
}



void EulerAngle::writeTo(cefix::Serializer& s)
{
	s << "rho" << osg::RadiansToDegrees(rho);
	s << "phi" << osg::RadiansToDegrees(phi);
	s << "theta" << osg::RadiansToDegrees(theta);
}

}