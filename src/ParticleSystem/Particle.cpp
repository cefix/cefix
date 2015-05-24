/*
 *  Particle.cpp
 *  ForceFields
 *
 *  Created by Stephan Huber on 06.04.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Particle.h"

#include <cefix/PropertyList.h>
#include <cefix/Settings.h>
#include <cefix/AllocationObserver.h>
namespace cefix {

Particle::Particle() : 
	osg::Referenced(),
	_position(osg::Vec3(0,0,0)),
	_velocity(osg::Vec3(0,0,0)),
	_acceleration(osg::Vec3(0,0,0)),
	_rotation(osg::Quat()),
	_angularVelocity(0,0,0,0),
	_isAlive(true),
	_mayRotate(false),
	_t(0),
	_lifetime(-1),
	_mass(0.1f),
    _massInv(10.0f),
	_radius(0.2f),
	_isFixed(false)
{
	//cefix::AllocationObserver::instance()->observe(this);
}

Particle::~Particle() {
	// std::cout << "destructing particle?" << std::endl;
	
	
}

}