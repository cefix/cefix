/*
 *  LineParticle.cpp
 *  ForceFields
 *
 *  Created by Stephan Huber on 06.04.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "LineParticle.h"



LineParticle::LineParticle(unsigned int verticesCount) : cefix::Particle(), _verticesCount(verticesCount)
{
	_linestrip = new osg::Vec3Array();
}

