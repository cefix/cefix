/*
 *  AbstractStatistics.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 29.03.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "AbstractStatistics.h"
#include <cefix/TextGeode.h>

namespace cefix {


void AbstractStatistics::GroupedCaptionLocations::get(const osg::Vec3& loc, cefix::Utf8TextGeode* textgeode) 
{
	// std::cout << "get: " << loc << std::endl;
	_locations.insert(std::make_pair(loc, textgeode));
}


void AbstractStatistics::GroupedCaptionLocations::clear() {
	_locations.clear();
	// std::cout << "clear" << std::endl;
}


void AbstractStatistics::GroupedCaptionLocations::adjustLocations() 
{
	if (_locations.size() == 0)
		return;
	// std::cout << "start" << std::endl;
	osg::Vec3 last = osg::Vec3(0,-1000,0);

	for (LocationMap::iterator i = _locations.begin(); i != _locations.end(); ++i) 
	{
		osg::Vec3 p = i->first;
		//osg::Vec3 oldp = p;
		
		float delta = (p[1] - last[1]);
		
		if (delta < _max) {
			p[1] = last[1] + _max;
			i->second.get()->setPosition(p);
		} else {
			i->second.get()->setPosition(p);
		}
		
		last = p;
		
		//std::cout << oldp << " -> " << p << std::endl;
	}
	_locations.clear();
}
}