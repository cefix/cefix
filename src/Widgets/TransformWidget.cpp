/*
 *  TransformWidget.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 05.11.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TransformWidget.h"
#include <cefix/WidgetFactory.h>

namespace cefix {

TransformWidget::TransformWidget(const std::string& identifier, const osg::Matrix& matrix) : 
	GroupWidget(identifier)
{
	_mat = new osg::MatrixTransform();
	_mat->setMatrix(matrix);
	_mat->setName(identifier+ " TransformWidget");
	_group = _mat.get();
}

TransformWidget::TransformWidget(cefix::PropertyList* pl) : 
	GroupWidget(pl),
	_mat(new osg::MatrixTransform()) 
{
	_mat->setName(pl->get("id")->asString()+" TransformWidget");
	osg::Matrix m;
	if (pl->hasKey("translation"))
		m *= osg::Matrix::translate(pl->get("translation")->asVec3());
	if (pl->hasKey("rotation")) {
		osg::Vec3 r = pl->get("rotation")->asVec3();
		m *= osg::Matrix::rotate(osg::DegreesToRadians(r[0]), osg::X_AXIS);
		m *= osg::Matrix::rotate(osg::DegreesToRadians(r[1]), osg::Y_AXIS);
		m *= osg::Matrix::rotate(osg::DegreesToRadians(r[2]), osg::Z_AXIS);
	}
	if (pl->hasKey("pivot"))
		m *= osg::Matrix::translate(pl->get("pivot")->asVec3());

	
	setMatrix(m);
	_group = _mat.get();

}

}

