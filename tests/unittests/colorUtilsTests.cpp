/*
 *  colorUtils.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 10.10.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include <osg/Vec4>
#include <cefix/ColorUtils.h>
#include "catch.hpp"


TEST_CASE( "ColorUtils/rgb", "rgb + rgba tests" )
{	// test rgb + rgba

	REQUIRE(osg::Vec4(1,0,0,1) == cefix::rgb(0xff0000));
	REQUIRE(osg::Vec4(0,1,0,1) == cefix::rgb(0x00ff00));
	REQUIRE(osg::Vec4(0,0,1,1) == cefix::rgb(0x0000ff));
	
	REQUIRE(osg::Vec4(1,0,0,1) == cefix::rgba(0xff0000FF));
	REQUIRE(osg::Vec4(0,1,0,1) == cefix::rgba(0x00ff00FF));
	REQUIRE(osg::Vec4(0,0,1,1) == cefix::rgba(0x0000ffFF));
	
	REQUIRE(osg::Vec4(1,1,1,1) == cefix::rgb(0xffffff)); 
	REQUIRE(osg::Vec4(1,1,1,1) == cefix::rgba(0xffffffff)); 
	
	REQUIRE(osg::Vec4(0,0,0,1) == cefix::rgb(0x000000)); 
	REQUIRE(osg::Vec4(0,0,0,0) == cefix::rgba(0x00000000)); 
			
}
