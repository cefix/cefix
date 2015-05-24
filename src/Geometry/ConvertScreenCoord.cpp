/*
 *  Convert2DCoord.cpp
 *  AudiHeritage
 *
 *  Created by Stephan Huber on 31.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include <cefix/ConvertScreenCoord.h>
#include <cefix/DataFactory.h>
#include <cefix/DisplayCollection.h>

namespace cefix {

static bool s_inited = false;
static int s_height = 300;


void ConvertScreenCoord::setHeight(float height) {
	s_height = height;
	s_inited = true;
}


void ConvertScreenCoord::applyDisplay(Display* display) {
	if (display) {
		setHeight(display->get2DConfiguration().height);
	}
}



float ConvertScreenCoord::getHeight() {
    if (s_inited) return s_height;
    applyDisplay(DisplayCollection::instance()->getDisplay("main"));
	s_inited = true;
    return s_height;
}



template<> osg::Vec4 convertScreenCoord(osg::Vec4 r) {
    float h = ConvertScreenCoord::getHeight();
    float rh = r[3] - r[1];
    return osg::Vec4(r[0], h - r[1] - rh, r[2], h - r[1] );
}

template<> osg::Vec3 convertScreenCoord(osg::Vec3 r) {
    float h = ConvertScreenCoord::getHeight();
    return osg::Vec3(r[0], h - r[1], r[2]);
}

template<> osg::Vec4 convertCoord(osg::Vec4 r, float h) {
	float rh = r[3] - r[1];
	return osg::Vec4(r[0], h - r[1] - rh, r[2], h - r[1] );
}

template<> osg::Vec3 convertCoord(osg::Vec3 r, float h){
	return osg::Vec3(r[0], h - r[1], r[2]);
}

template<> osg::Vec2 convertCoord(osg::Vec2 r, float h){
	return osg::Vec2(r[0], h - r[1]);
}

}