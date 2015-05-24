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

#ifndef CEFIX_LOOK_AT_TRANSFORM_HEADER
#define CEFIX_LOOK_AT_TRANSFORM_HEADER

#include <osg/Transform>
#include <osg/Matrix>

namespace cefix 
{

    class LookAtTransform : public osg::Transform {
        
    public:
        LookAtTransform();
        LookAtTransform(const LookAtTransform& m, const osg::CopyOp &copyop=osg::CopyOp::SHALLOW_COPY);
        
        void setEye(const osg::Vec3& eye) {
            _eye = eye; 
            _dirty = true;
        }
        
        void setCenter(const osg::Vec3& center) {
            _center = center;
            _dirty = true;
        }
        
        void setUp(const osg::Vec3& up) {
            _up = up;
            _dirty = true;
        }
        
        const osg::Vec3& getEye() const { return _eye; }
        const osg::Vec3& getCenter() const { return _center; }
        const osg::Vec3& getUp() const { return _up; }
        
        
        virtual bool computeLocalToWorldMatrix (osg::Matrix &matrix, osg::NodeVisitor *) const;
        virtual bool computeWorldToLocalMatrix (osg::Matrix &matrix, osg::NodeVisitor *) const;
    
    protected:
        void compute() const;
        osg::Vec3 _eye, _center, _up;
        osg::Matrix _m;
        bool _dirty;
    };
}
 

#endif