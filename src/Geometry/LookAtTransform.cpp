/*
 *  LookAtMatrixTransform.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 08.11.09.
 *  Copyright 2009 Digital Mind. All rights reserved.
 *
 */

#include "LookAtTransform.h"

namespace cefix {
    
LookAtTransform::LookAtTransform() 
:   osg::Transform(),
    _eye(0,-1,0),
    _center(0,0,0),
    _up(osg::Z_AXIS),
    _m(),
    _dirty(true)

{
    setReferenceFrame(RELATIVE_RF);
}

LookAtTransform::LookAtTransform(const LookAtTransform& m, const osg::CopyOp &copyop)
:   osg::Transform(m, copyop),
    _eye(m._eye),
    _center(m._center),
    _up(m._up),
    _m(m._m),
    _dirty(true)
{
}

bool LookAtTransform::computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor*) const
{
    if (_dirty) compute();
    
    if (_referenceFrame==RELATIVE_RF)
    {
        matrix.preMult(_m);
    }
    else // absolute
    {
        matrix = _m;
    }
    return true;
}

bool LookAtTransform::computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor*) const
{
    if (_dirty) compute();
    
    osg::Matrix inverse = osg::Matrix::inverse(_m);

    if (_referenceFrame==RELATIVE_RF)
    {
        matrix.postMult(inverse);
    }
    else // absolute
    {
        matrix = inverse;
    }
    return true;
}


void LookAtTransform::compute() const
{
    osg::Matrix& m = const_cast<osg::Matrix&>(_m);
    
	osg::Vec3d f(_center-_eye);
    f.normalize();
    osg::Vec3d s(f^_up);
    s.normalize();
    osg::Vec3d u(s^f);
    u.normalize();

    m.set(
        -f[0],     -f[1],     -f[2],     0.0,
        s[0],     s[1],     s[2],     0.0,
        u[0],     u[1],      u[2],     0.0,
        0.0,     0.0,     0.0,      1.0);

	//m.postMultRotate(osg::Quat(osg::PI_2, osg::Z_AXIS));
    m.postMultTranslate(_eye);

	/*
	m.makeIdentity();
   
	osg::Quat q;
	q.makeRotate(osg::X_AXIS, _eye-_center);
	m.postMultRotate(q);
    */
    const_cast<bool &>(_dirty) = false;
}

}