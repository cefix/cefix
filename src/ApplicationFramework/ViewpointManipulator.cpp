/*
 *  ViewPointMatrixManipulator.cpp
 *  Spiral
 *
 *  Created by Ralph Ammer on 4/28/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "ViewPointManipulator.h"
#include <osg/Math>
#include <osg/CameraView>
#include <cefix/Log.h>

namespace cefix {


ViewPointManipulator::ViewPointManipulator() : 
	osgGA::CameraManipulator(), 
	_node(NULL) 
{ 
	_matrix.makeLookAt(osg::Vec3(0,-40,0), osg::Vec3(0,0,0), osg::Vec3(0,0,1));
}; 


 void ViewPointManipulator::init(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa) {
    if (_node.valid()) {
        ViewPointNode* vpn = dynamic_cast<ViewPointNode*>(_node.get());
        if ((vpn) && (vpn->maySwitchVisibility()))
            vpn->switchVisibility(false);
    }
}

osg::Matrixd ViewPointManipulator::getMatrix() const {
   return _matrix;
}

osg::Matrixd ViewPointManipulator::getInverseMatrix() const {
    osg::Matrix  m = osg::Matrix::inverse(_matrix);
    return m;
}

void ViewPointManipulator::setByMatrix(const osg::Matrixd& mat) {
   _matrix = mat;
}

void ViewPointManipulator::setByInverseMatrix(const osg::Matrixd& mat){
  
   _matrix = osg::Matrixd::inverse(mat);
} 


void ViewPointManipulator::setViewPointNode(osg::Node* node, bool useOffset) 
{
	_offset = osg::Matrix::identity();
	
    osg::NodePathList npl = node->getParentalNodePaths();
    if (npl.size() >= 1) {
        _node = node;
        _nodePath = npl[0];
    }
    else {
        log::error("ViewPointManipulator") << "could not get nodepath from node, not in scenegraph?" << std::endl;
    }
	
	if (useOffset) {
		_offset = osg::Matrix::rotate(osg::PI/2, 0,1,0) * osg::Matrix::rotate(osg::PI/2, 1,0,0);
    } 
}

bool ViewPointManipulator::handle
       (const osgGA::GUIEventAdapter&ea, osgGA::GUIActionAdapter&aa)
{
    switch(ea.getEventType())
    {
        case (osgGA::GUIEventAdapter::FRAME): 
        {
            if (_node.valid()) {
                _matrix = _offset * osg::computeLocalToWorld(_nodePath);
                
            }
        
            return false;
        }
		default:
			return false;
    }
    return false;
}

}
