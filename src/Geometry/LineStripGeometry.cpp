/*
 *  LineStripGeometry.cpp
 *  theseuslogo
 *
 *  Created by Stephan Huber on 07.11.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "LineStripGeometry.h"

namespace cefix {

LineStripGeometry::LineStripGeometry(bool isClosed, unsigned int maxPointsShown, float minDistance) : 
	osg::Geometry(),
	_v(new osg::Vec3Array()),
	_c(new osg::Vec4Array()),
	_da(new osg::DrawArrays(isClosed ? osg::PrimitiveSet::LINE_LOOP : osg::PrimitiveSet::LINE_STRIP, 0,0)),
	_maxPointsShown(maxPointsShown),
	_minDistance(minDistance)
{
	_c->push_back(osg::Vec4(1,1,1,1));
	setVertexArray(_v.get());
	setColorArray(_c.get());
	setColorBinding(BIND_OVERALL);
	addPrimitiveSet(_da.get());
	_lastInsertIndex = 0;
}


void LineStripGeometry::add(const osg::Vec3 v, bool cleanDirty) 
{
	if ((_last - v).length2() < _minDistance) return;
	_last = v;
	if (_lastInsertIndex >= _v->size()) {
		_v->push_back(v);
		_lastInsertIndex = _v->size();
		_da->setCount(_lastInsertIndex);
	} else {
		(*_v)[_lastInsertIndex] = v;
		_da->setCount(++_lastInsertIndex);
	}
	
	if (_lastInsertIndex > _maxPointsShown) {
		_da->setFirst(_lastInsertIndex - _maxPointsShown);
	}
	if (cleanDirty) {
		dirtyDisplayList();
		dirtyBound();
	}
}

}