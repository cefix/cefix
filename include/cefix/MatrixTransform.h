/*
 *
 *      (\(\
 *     (='.')           cefix --
 *    o(_")")           a multipurpose library on top of OpenSceneGraph
 *  
 *
 *                      Copyright 2001-2011, stephanmaximilianhuber.com 
 *                      Stephan Maximilian Huber. 
 *
 *                      All rights reserved.
 *
 */

#ifndef CEFIX_MATRIX_TRANSFORM_HEADER
#define CEFIX_MATRIX_TRANSFORM_HEADER

#include <osg/MatrixTransform>
#include <cefix/EulerAngle.h>
namespace cefix {

class MatrixTransform: public osg::MatrixTransform {

public:
    MatrixTransform();
    MatrixTransform(const osg::Matrix& mat);
    MatrixTransform(const MatrixTransform& mat, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);    
    const osg::Vec3& getPosition() const { return _position; }
    const osg::Vec3& getScale() const { return _scale; }
    const osg::Quat& getRotation() const { return _rotation; }
    
    void setPosition(const osg::Vec3& pos) { _position = pos; _dirty = true; }
    void setRotation(const osg::Quat& rot) { _rotation = rot; _dirty = true; }
    void setScale(const osg::Vec3& scale) { _scale = scale; _dirty = true; }
	
	void setRotationFromEuler(const osg::Vec3& rot) { cefix::EulerAngle e(rot[0], rot[1], rot[2]); setRotation(e.quat); }
	
	void setPivotPoint(const osg::Vec3& p) { _pivotPoint = p; _dirty = true; }
	const osg::Vec3& getPivotPoint() const { return _pivotPoint; }
    void update();   
	
private:
    

    virtual bool computeLocalToWorldMatrix (osg::Matrix &matrix, osg::NodeVisitor *) const;
    virtual bool computeWorldToLocalMatrix (osg::Matrix &matrix, osg::NodeVisitor *) const;
    
      
protected:
    osg::Vec3 _position, _pivotPoint;
    osg::Vec3 _scale;
    osg::Quat _rotation;
    bool      _dirty;

};



}

#endif