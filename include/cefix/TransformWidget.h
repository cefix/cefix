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

#ifndef TRANSFORM_WIDGET_HEADER
#define TRANSFORM_WIDGET_HEADER

#include <cefix/GroupWidget.h>
#include <osg/MatrixTransform>

namespace cefix {

/** A transformWidget is a GroupWidget with a transform, you can translate, rotate, scale, whatever you like all the group-items of this widget */
class CEFIX_EXPORT TransformWidget : public GroupWidget {
	public:
		/** ctor */
		TransformWidget(const std::string& identifier, const osg::Matrix& matrix = osg::Matrix::identity());
		/** ctor for the widget factory */
		TransformWidget(cefix::PropertyList* pl);
		
		/** set the matrix describing the transform */
		void setMatrix(const osg::Matrix& m) { _mat->setMatrix(m); }
		
		
		/** get the node */
		virtual osg::Node* getNode() { return _mat.get(); }
		
	protected:
		osg::MatrixTransform*  getMatrixTransform() { return _mat.get(); }
		
	private:
		osg::ref_ptr<osg::MatrixTransform>	_mat;

};

}

#endif