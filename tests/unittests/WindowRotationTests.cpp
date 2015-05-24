/*
 *  WindowRotationTests.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 28.01.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/WindowRotation.h>
#include "catch.hpp"

using namespace cefix;

TEST_CASE( "WindowRotation/windowRotation", "windowRotation" )
{
	{
		WindowRotation rot(WindowRotation::TILTED_LEFT);
		rot.setViewport(0,0,400,200);
		REQUIRE(rot.getWidth() == 200);
		REQUIRE(rot.getHeight() == 400);
	}
	{
		WindowRotation rot(WindowRotation::TILTED_RIGHT);
		rot.setViewport(0,0,400,200);
		REQUIRE(rot.getWidth() == 200);
		REQUIRE(rot.getHeight() == 400);
		REQUIRE(rot.isTilted());
	}
	
}
