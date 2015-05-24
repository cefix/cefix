//
//  QuadTreeTests.cpp
//  cefix
//
//  Created by Stephan Maximilian Huber on 30.06.12.
//  Copyright (c) 2012 stephanmaximilianhuber.com. All rights reserved.
//

#include <iostream>
#include "catch.hpp"
#include <cefix/QuadTree.h>

struct TestStruct {
    osg::Vec3 v;
    
    const osg::Vec3& getPosition() const { return v; }
};

typedef cefix::QuadTree<TestStruct, float, osg::Vec3> TestQuadTree;

TEST_CASE( "QuadTree", "add, remove + relocate" )
{
    TestQuadTree tree(640, 480);
    
    TestStruct t1, t2, t3;
    
    t1.v.set(100,100,0);
    t2.v.set(-100,-100,0);
    t3.v.set(800,800,0);
    
    tree.add(&t1);
    tree.add(&t2);
    tree.add(&t3);
    
    REQUIRE(tree.size() == 3);
    
}
