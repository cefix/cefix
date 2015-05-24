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

#ifndef VECTOR_FIELD_GEOMETRY_HEADER
#define VECTOR_FIELD_GEOMETRY_HEADER

#include <osg/Geometry>

#include <cefix/VectorField.h>
#include <cefix/Export.h>

namespace cefix {
/** this class renders a given vectorfield */
class CEFIX_EXPORT VectorFieldGeometry : public osg::Geometry {

	public:
		/** ctor
		 *  @param field the vector-field to use
		 *  @param subdivision subdivide the vectorfield, so you get a finer resolution in display 
		 *  @param startcolor color for the start-point of a vector 
		 *  @param endcolor color for the endpoint of a vector, use differen alpha-values to illustrate the direction of a vector */
	VectorFieldGeometry(VectorFieldT<osg::Vec3>* field, unsigned subdivision = 1, osg::Vec4 startcolor = osg::Vec4(1,1,0,1), osg::Vec4 endcolor = osg::Vec4(1,1,0,0.2), float scale = 1);
		
		/** updates the geometry, call this, when you changed the vectorfield */
		void update(bool recomputeBounds = false);
		
		void setScale(float s) { _scale = s; }
		void setLengthScale(float s) {_lengthScale = s; }
		void setAutoUpdate(bool f);
	private:
	osg::ref_ptr<VectorFieldT<osg::Vec3> >	_field;
		unsigned int				_subdivision;
		osg::Vec4					_startColor, _endColor;
		osg::ref_ptr<osg::Vec3Array>	_vertices;
		osg::ref_ptr<osg::Vec4Array>	_colors;
		float							_scale, _lengthScale;
};

}
#endif