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

#ifndef NODE_MANIPULATION_TIMELINE_ACTIONS_HEADER
#define NODE_MANIPULATION_TIMELINE_ACTIONS_HEADER

#include <cefix/MatrixTransform.h>
#include <cefix/KeyframeBasedAction.h>
#include <cefix/NodeManipulationTimeline.h>
#include <osg/io_utils>

namespace cefix {
	
	template<> inline osg::Quat interpolate<osg::Quat>(const osg::Quat& a, const osg::Quat& b, float t) 
	{
		osg::Quat result;
		
		result.slerp(t,a,b);
		
        osg::Quat::value_type len2 = result.length2();
        if ( len2 != 1.0 && len2 != 0.0) 
            result *= 1.0/sqrt(len2);
		
		return result;
    }	




class TranslateMatrixTransformAction : public cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>
{
public:

    virtual void within(double scalar)
	{
		getNode()->setPosition(sample(scalar));
	}
	
	virtual void readFrom(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>::readFrom(serializer);
	}
	
	virtual void writeTo(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>::writeTo(serializer);
	}
	
	
	
};

class SetPivotPointMatrixTransformAction : public cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>
{
public:

    virtual void within(double scalar)
	{
		getNode()->setPivotPoint(sample(scalar));
	}
	
	virtual void readFrom(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>::readFrom(serializer);
	}
	
	virtual void writeTo(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>::writeTo(serializer);
	}
	
	
	
};

class RotateMatrixTransformAction : public cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Quat>, cefix::MatrixTransform>
{
public:

    virtual void within(double scalar)
	{
		osg::Quat q = sample(scalar);
		getNode()->setRotation(q);
	}
	
	virtual void readFrom(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Quat>, cefix::MatrixTransform>::readFrom(serializer);
	}
	
	virtual void writeTo(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Quat>, cefix::MatrixTransform>::writeTo(serializer);
	}

};

class RotateEulerDegreesMatrixTransformAction : public cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>
{
public:

    virtual void within(double scalar)
	{
		osg::Vec3d from, to;
		double sample_scalar = getSampleScalar(from, to, scalar);
		
		osg::Quat q_from, q_to;
		const double to_rad(osg::PI/180.0);
		q_from.makeRotate(from[0] * to_rad, osg::X_AXIS, from[1] * to_rad, osg::Y_AXIS, from[2] * to_rad, osg::Z_AXIS);
		q_to.makeRotate(to[0] * to_rad, osg::X_AXIS, to[1] * to_rad, osg::Y_AXIS, to[2] * to_rad, osg::Z_AXIS);
		q_from.slerp(sample_scalar, q_from, q_to);
		getNode()->setRotation(q_from);
	}
	
	virtual void readFrom(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>::readFrom(serializer);
	}
	
	virtual void writeTo(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>::writeTo(serializer);
	}

};


class RotateHPBDegreesMatrixTransformAction : public cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>
{
public:

    virtual void within(double scalar)
	{
		osg::Vec3d from, to;
		double sample_scalar = getSampleScalar(from, to, scalar);
		//std::cout << from << " -> " << to << "=" << cefix::interpolate(from, to, sample_scalar) << std::endl;
		osg::Quat q_from, q_to;
		const double to_rad(osg::PI/180.0);
		
		q_from.makeRotate(from[1] * to_rad, osg::X_AXIS, from[2] * to_rad, osg::Y_AXIS, from[0] * to_rad, osg::Z_AXIS);
		q_to.makeRotate(to[1] * to_rad, osg::X_AXIS, to[2] * to_rad, osg::Y_AXIS, to[0] * to_rad, osg::Z_AXIS);
		q_from.slerp(sample_scalar, q_from, q_to);
		
		getNode()->setRotation(q_from);
	}
	
	virtual void readFrom(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>::readFrom(serializer);
	}
	
	virtual void writeTo(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>::writeTo(serializer);
	}

};


class RotateAroundAxisMatrixTransformAction : public cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<double>, cefix::MatrixTransform>
{
public:
    RotateAroundAxisMatrixTransformAction(const osg::Vec3& axis) 
    :   cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<double>, cefix::MatrixTransform>(),
        _axis(axis)
    {
    }
    
    virtual void within(double scalar)
	{
		osg::Quat q(sample(scalar), _axis);
		getNode()->setRotation(q);
	}
	
	virtual void readFrom(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<double>, cefix::MatrixTransform>::readFrom(serializer);
	}
	
	virtual void writeTo(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<double>, cefix::MatrixTransform>::writeTo(serializer);
	}
	
private:
    osg::Vec3 _axis;
};


class ScaleMatrixTransformAction : public cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>
{
public:

    virtual void within(double scalar)
	{
		getNode()->setScale(sample(scalar));
	}
	
	virtual void readFrom(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>::readFrom(serializer);
	}
	
	virtual void writeTo(cefix::Serializer& serializer) {
		cefix::ManipulateNodeAction< cefix::KeyframeBasedAction<osg::Vec3d>, cefix::MatrixTransform>::writeTo(serializer);
	}
};


}

#endif
