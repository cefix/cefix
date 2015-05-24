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
 
#ifndef VIEWPOINT_MATRIX_MANIPULATOR_HEADER
#define VIEWPOINT_MATRIX_MANIPULATOR_HEADER



#include <iostream>
#include <osgGA/CameraManipulator>
#include <osg/Matrix>

#include <cefix/ViewPointNode.h>

namespace cefix {

/** 
 * this class implements a Matrixmanipulator, which sets its matrix to the world-matrix of a node of the scenegraph
 */
class CEFIX_EXPORT ViewPointManipulator : public osgGA::CameraManipulator {
    public:
        ViewPointManipulator();
        bool handle (const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
        
        virtual void setByMatrix(const osg::Matrixd& mat);
        virtual void setByInverseMatrix(const osg::Matrixd& mat);
        virtual osg::Matrixd getInverseMatrix() const;
        virtual osg::Matrixd getMatrix() const;
        void setViewPointNode(osg::Node* node, bool useOffset = true);
        
        virtual void init(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
        
        osg::Node* getViewPointNode() { return _node.get(); }
        bool hasAttachedNode() { return _node.valid(); }
		
		void setOffset(const osg::Matrix& m) {_offset = m; }
    
    protected:
        virtual ~ViewPointManipulator() {}
        osg::ref_ptr<osg::Node>     _node;
        osg::NodePath               _nodePath;
        osg::Matrixd                _matrix, _offset;
};

}

#endif // VIEWPOINT_MATRIX_MANIPULATOR_HEADER