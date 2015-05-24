/*
 *  MatrixTransform.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 15.11.09.
 *  Copyright 2009 Digital Mind. All rights reserved.
 *
 */

#include "MatrixTransform.h"
#include <cefix/MathUtils.h>

namespace cefix {


MatrixTransform::MatrixTransform() 
:   osg::MatrixTransform(), 
    _position(), 
	_pivotPoint(0, 0, 0),
    _scale(1,1,1), 
    _rotation(), 
    _dirty(true) 
{
}


MatrixTransform::MatrixTransform(const osg::Matrix& mat)
:   osg::MatrixTransform(mat)
{
    _scale = mat.getScale();
    _position = mat.getTrans();
	_rotation = mat.getRotate();
	_pivotPoint.set(0,0,0);
	_dirty = true;
}

MatrixTransform::MatrixTransform(const MatrixTransform& mat, const osg::CopyOp& copyop)
:   osg::MatrixTransform(mat, copyop),
    _position(mat._position),
	_pivotPoint(mat._pivotPoint),
    _scale(mat._scale),
    _rotation(mat._rotation),
    _dirty(true)
{
}



void MatrixTransform::update() 
{
	if (_pivotPoint.length2() > 0) {
		setMatrix(  osg::Matrix::scale(_scale) * 
				osg::Matrix::translate(-_pivotPoint) *
                osg::Matrix::rotate(_rotation) *
                osg::Matrix::translate(_pivotPoint + _position));
	} else {
		setMatrix(  osg::Matrix::scale(_scale) * 
                osg::Matrix::rotate(_rotation) *
                osg::Matrix::translate(_position));
	}
    dirtyBound();

    _dirty = false;
}


bool MatrixTransform::computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
{
    if (_dirty) const_cast<MatrixTransform*>(this)->update();
    
    return osg::MatrixTransform::computeLocalToWorldMatrix(matrix, nv);
}

bool MatrixTransform::computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
{
    if (_dirty) const_cast<MatrixTransform*>(this)->update();
    
    return osg::MatrixTransform::computeWorldToLocalMatrix(matrix, nv);
}


}