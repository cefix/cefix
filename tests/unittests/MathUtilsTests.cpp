/*
 *  MathUtilsTests.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 07.03.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/MathUtils.h>
#include "catch.hpp"
#include <ostream>
#include <iostream>
#include <osg/io_utils>



TEST_CASE( "MathUtils/rayPlaneIntersection", "rayPlaneIntersection" )
{
	
	
	{	// test auf erfolgreiches intersecten:
	
		osg::Plane plane( osg::Vec3(0,0,1), osg::Vec3(0,0,0));
		osg::Vec3 ray(osg::Vec3(0,0.1, 1));
		osg::Vec3 pointOnRay(osg::Vec3(10,0,10));
		osg::Vec3 result;
		cefix::RayPlaneIntersection::Result intersecting = cefix::getRayPlaneIntersection(plane, pointOnRay, ray, result); 
		REQUIRE(intersecting == cefix::RayPlaneIntersection::ONE_INTERSECTION);
		//std::cout << result << std::endl;
		REQUIRE(result == osg::Vec3(10,-1,0));
	}
	
	{	// test ebene + ray parallel zueinander, aber nicht ray auf ebene liegend
	
		osg::Plane plane( osg::Vec3(0,0,1), osg::Vec3(0,0,0));
		osg::Vec3 ray(osg::Vec3(0,1, 0));
		osg::Vec3 pointOnRay(osg::Vec3(10,0,10));
		osg::Vec3 result;
		cefix::RayPlaneIntersection::Result intersecting = cefix::getRayPlaneIntersection(plane, pointOnRay, ray, result); 
		REQUIRE(intersecting == cefix::RayPlaneIntersection::NO_INTERSECTION);
	}
	
	{	// test ebene + ray parallel zueinander, und ray auf ebene liegend
	
		osg::Plane plane( osg::Vec3(0,0,1), osg::Vec3(10,0,10));
		osg::Vec3 ray(osg::Vec3(0,-1, 0));
		osg::Vec3 pointOnRay(osg::Vec3(10,0,10));
		osg::Vec3 result;
		cefix::RayPlaneIntersection::Result intersecting = cefix::getRayPlaneIntersection(plane, pointOnRay, ray, result); 
		REQUIRE(intersecting == cefix::RayPlaneIntersection::RAY_ON_PLANE);
		REQUIRE(result == pointOnRay);
	}
}



TEST_CASE( "MathUtils/Rect", "Rect" )
{
	// test cefix::Rect
	REQUIRE(cefix::Rect::isIntersecting(osg::Vec4(100,100,200,200), osg::Vec4(150, 150, 250, 250)));
	osg::Vec4 result;
	REQUIRE(cefix::Rect::getIntersection(osg::Vec4(100,100,200,200), osg::Vec4(150, 150, 250, 250), result));
	REQUIRE(result == osg::Vec4(150, 150, 200, 200));
    
    result = cefix::Rect::inset(osg::Vec4(100,100,200,200), osg::Vec2(20,25));
    REQUIRE(result == osg::Vec4(120,125,180,175));
    
    result = cefix::Rect::offset(osg::Vec4(100,100,200,200), osg::Vec2(20,25));
    REQUIRE(result == osg::Vec4(120,125,220,225));
}


TEST_CASE( "MathUtils/LineLineIntersection", "LineLineIntersection" )
{
	// test cefix::getLineLineIntersection
    {
        osg::Vec3 p1(100, 100, 100);
        osg::Vec3 p2(200, 100, 100);
        osg::Vec3 p3(200, 200, 100);
        osg::Vec3 p4(300, 200, 100);
        
        osg::Vec3 result;
        
        REQUIRE(cefix::getLineLineIntersection(p1, p2, p3, p4, result) == false);
    }
    
    {
        osg::Vec3 p1(100, 100, 100);
        osg::Vec3 p2(200, 100, 100);
        osg::Vec3 p3(150, 50, 100);
        osg::Vec3 p4(150, 150, 100);
        
        osg::Vec3 result;
        
        REQUIRE(cefix::getLineLineIntersection(p1, p2, p3, p4, result) == true);
        REQUIRE(result == osg::Vec3(150, 100, 100));
    }
    
    {
        osg::Vec3 p1(100, 100, 100);
        osg::Vec3 p2(200, 100, 100);
        osg::Vec3 p3(150,   0, 100);
        osg::Vec3 p4(150,  90, 100);
        
        osg::Vec3 result;
        
        REQUIRE(cefix::getLineLineIntersection(p1, p2, p3, p4, result) == false);
        
    }
    
    {
        osg::Vec3 p1(100, 100, 100);
        osg::Vec3 p2(200, 100, 100);
        osg::Vec3 p3(150, 100, 100);
        osg::Vec3 p4(150,  90, 100);
        
        osg::Vec3 result;
        
        REQUIRE(cefix::getLineLineIntersection(p1, p2, p3, p4, result) == true);
        REQUIRE(result == osg::Vec3(150, 100, 100));
    }
    
    {
        osg::Vec2 p1(100, 100);
        osg::Vec2 p2(200, 100);
        osg::Vec2 p3(150, 100);
        osg::Vec2 p4(150,  90);
        
        osg::Vec2 result;
        
        REQUIRE(cefix::getLineLineIntersection(p1, p2, p3, p4, result) == true);
        REQUIRE(result == osg::Vec2(150, 100));
    }
    
    {
        osg::Vec2 p1(100, 100);
        osg::Vec2 p2(200, 100);
        osg::Vec2 p3(150, 110);
        osg::Vec2 p4(150,  90);
        
        osg::Vec2 result;
        
        REQUIRE(cefix::getLineLineIntersection(p1, p2, p3, p4, result) == true);
        REQUIRE(result == osg::Vec2(150, 100));
    }
    {
        osg::Vec2 p1(100, 100);
        osg::Vec2 p2(200, 100);
        osg::Vec2 p3(100, 110);
        osg::Vec2 p4(200, 110);
        
        osg::Vec2 result;
        
        REQUIRE(cefix::getLineLineIntersection(p1, p2, p3, p4, result) == false);
    }
}


TEST_CASE( "MathUtils/interpolate", "interpolate" )
{
	REQUIRE(cefix::interpolate(0.0f, 10.0f, 0.5f) == 5.0f);
	REQUIRE(cefix::interpolate(0.0f, 10.0f, 0.0f) == 0.0f);
	REQUIRE(cefix::interpolate(0.0f, 10.0f, 1.0f) == 10.0f);
}


TEST_CASE( "MathUtils/range", "range" )	
{		
	cefix::floatRange f(100.0f, 200.0f);
	REQUIRE(f.clampTo(300.0f) == 200.0f);
	REQUIRE(f.clampTo(30.0f) == 100.0f);
	REQUIRE(f.mid() == 150.0f);
	REQUIRE(f.min() == 100.0f);
	REQUIRE(f.max() == 200.0f);
	REQUIRE(f.interpolate(0.5f) == 150.0f);
	REQUIRE(f.tween<cefix::LinearTweening<float> >(0.5f) == 150.0f);
	
}
	
