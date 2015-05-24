/*
 *  VectorFieldGeometry.cpp
 *  ForceFields
 *
 *  Created by Stephan Huber on 31.03.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <osg/StateSet>
#include "VectorFieldGeometry.h"

namespace cefix {

	VectorFieldGeometry::VectorFieldGeometry(VectorFieldT<osg::Vec3>* field, unsigned int subdivision, osg::Vec4 startcolor, osg::Vec4 endcolor, float scale) : 
	osg::Geometry(), 
	_field(field),
	_subdivision(subdivision),
	_startColor(startcolor),
	_endColor(endcolor),
	_scale(scale),
	_lengthScale(0.5f)
{
	
	_vertices = new osg::Vec3Array();
	_vertices->resize(2 * _field->getDimZ() * _subdivision * _field->getDimY() * _subdivision * _field->getDimX() * _subdivision);
	_colors = new osg::Vec4Array();
	_colors->resize(_vertices->size());
	
	setColorArray(_colors.get());
	setColorBinding(BIND_PER_VERTEX);
	
	setVertexArray(_vertices.get());
	
	update(true);
	
	addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, _vertices->size()));
	
	// blend an, licht aus, in den depth-buffer schreiben auch aus
	getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	
	setAutoUpdate(true);
}

class VFUpdateCallback : public osg::Drawable::UpdateCallback {
public:
	VFUpdateCallback(VectorFieldGeometry* geo) : osg::Drawable::UpdateCallback(), _vf(geo) {}
	
	virtual void update(osg::NodeVisitor *, osg::Drawable *)
	{
		_vf->update(true);
	}
private:
	VectorFieldGeometry* _vf;
};


void VectorFieldGeometry::setAutoUpdate(bool f)
{
	setUpdateCallback(f ? new VFUpdateCallback(this) : NULL);
}

void VectorFieldGeometry::update(bool recomputeBounds)
{
	float fsub = _subdivision;
	osg::Vec3 v;
	unsigned int ndx;
	osg::Vec4 startc(_startColor), endc(_endColor);
	for (unsigned int z = 0; z < _field->getDimZ() * _subdivision; ++z) {
		for (unsigned int y = 0; y < _field->getDimY() * _subdivision; ++y)  {
			for (unsigned int x = 0; x < _field->getDimX() * _subdivision; ++x) {
				v = _field->get(x / float(_subdivision),y / float(_subdivision),z / float(_subdivision)) * _lengthScale;
				ndx = (z*_field->getDimX() * _subdivision * _field->getDimY()  * _subdivision + y*_field->getDimX() * _subdivision + x);
				
				
				if ((x / _subdivision) == (x / fsub) && (y / _subdivision) == (y / fsub)) {
					(*_vertices)[ndx * 2]   = osg::Vec3(x / fsub * _scale,y / fsub * _scale ,z / fsub * _scale)  - v / 2.0f;
					(*_vertices)[ndx * 2+1] = osg::Vec3(x / fsub * _scale,y / fsub * _scale ,z / fsub * _scale)  + v / 2.0f;

					startc[3] = _startColor[3];
					endc[3] = _endColor[3];
				}
				else {
					(*_vertices)[ndx * 2]   = osg::Vec3(x / fsub * _scale,y / fsub * _scale ,z / fsub * _scale)  - v  / fsub * 2.0f;
					(*_vertices)[ndx * 2+1] = osg::Vec3(x / fsub * _scale,y / fsub * _scale ,z / fsub * _scale)  + v  / fsub * 2.0f;
					
					startc[3] = _startColor[3] / 2.0f;
					endc[3] = _endColor[3] / 2.0f;
				}
				
				(*_colors)[ndx * 2 + 1] = startc;
				(*_colors)[ndx * 2    ] = endc;
			}
		}
	}
	dirtyDisplayList();
	if (recomputeBounds) dirtyBound();

}

}