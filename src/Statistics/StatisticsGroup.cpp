/*
 *  StatisticsGroup.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 29.03.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "StatisticsGroup.h"
#include <cefix/ColorUtils.h>

namespace cefix {

void StatisticsGroup::computeColors() {
	float h =  360.0f / _stats.size();
	int j = 0;
	for(StatsMap::iterator i = _stats.begin(); i != _stats.end(); ++i, ++j) {
		osg::Vec4 c = HSVtoRGB(j*h, 1,1);
		c[3] = 0.7f;
		i->second->setColor(c);
	}
	
}

void StatisticsGroup::computePlacement() {
	clear();
	unsigned int k(0);
	float x(0), y(0), z(0);
	for(unsigned int i=0; i < getNumChildren(); ++i) {
		osg::MatrixTransform* mat = dynamic_cast<osg::MatrixTransform*>(getChild(i));
		if(mat) {
			cefix::AbstractStatistics* s = dynamic_cast<cefix::AbstractStatistics*>(mat->getChild(0));
			if (s) {
				s->setXStepSize( (float) _maxx / s->getNumValues());
				s->recomputeGeometry();
			}
			++k;
			switch (_placement) {
				case OVERLAY:
					break;
				case STACK:
					x+=_dx;
					y+=_dy;
				
					break;
				case TILE:
					y+=_dy;
					break;
				case PILE:
					z += _dz;
					break;
			}
			mat->setMatrix(osg::Matrix::translate(x,y,z));
		}
	}
	_locations->adjustLocations();
}

void StatisticsGroup::adjustYScale() {
	
	/*
	static unsigned int cnt = 0;
	++cnt;
	if (cnt < 3)
		return;		
	cnt = 0;
	*/
	

	float max = 0;
	for(StatsMap::iterator i = _stats.begin(); i != _stats.end(); ++i) {
	
		max = osg::maximum(i->second->getMax(), max);
		
	}
	if (max == 0) 
		return;
	
	float scale = _maxy / (float)max;
	if (_lastYScale != scale) {
		_locations->clear();
		_lastYScale = scale;
		for(StatsMap::iterator i = _stats.begin(); i != _stats.end(); ++i) {	
			i->second->setYScale(scale);
			i->second->updateCaption();	
		}
		_locations->adjustLocations();
	}
	

	
}

}