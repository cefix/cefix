/*
 *  MatrixUtils.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 24.04.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "NodePathUtils.h"
#include <cefix/Log.h>
#include <osg/Camera>
namespace cefix {

// ----------------------------------------------------------------------------------------------------------
// getLastCamera
// ----------------------------------------------------------------------------------------------------------

osg::Camera* NodePathUtils::getLastCamera(const osg::NodePath& path) 
{
	for (osg::NodePath::const_reverse_iterator i = path.rbegin(); i!= path.rend(); ++i) {
		osg::Node* node = *i;
		osg::Camera* cam = (node) ? dynamic_cast<osg::Camera*>(node) : NULL;
		if (cam != NULL)
			return cam;
	}
	return NULL;
}

osg::Camera* NodePathUtils::getFirstCamera(const osg::NodePath& path) 
{
	for (osg::NodePath::const_iterator i = path.begin(); i!= path.end(); ++i) {
		osg::Node* node = *i;
		osg::Camera* cam = (node) ? dynamic_cast<osg::Camera*>(node) : NULL;
		if (cam != NULL)
			return cam;
	}
	return NULL;
}

// ----------------------------------------------------------------------------------------------------------
// getSpaceTransform
// ----------------------------------------------------------------------------------------------------------

osg::Matrixd NodePathUtils::getSpaceTransform(const osg::NodePath& fromWorld, const osg::NodePath& toWorld, bool ignoreRootCamera) {
	osg::Matrixd m;
	osg::Camera* fromCamera = getLastCamera(fromWorld);
	if (!fromCamera) {
		osg::notify(osg::INFO) << "MatrixUtils::getSpaceTransform no valid from-nodepath given" << std::endl;
		return m;
	}
	
	osg::Matrixd fromLocalToEye = osg::computeLocalToWorld(fromWorld, ignoreRootCamera) * fromCamera->getViewMatrix() * fromCamera->getProjectionMatrix();
	
	osg::Camera* toCamera = getLastCamera(toWorld);
	if (!toCamera) {
		osg::notify(osg::INFO) << "MatrixUtils::getSpaceTransform no valid to-nodepath given" << std::endl;
		return m;
	}
	
	osg::Matrixd toEyeToLocal;
	toEyeToLocal.invert(osg::computeLocalToWorld(toWorld, ignoreRootCamera) * toCamera->getViewMatrix() * toCamera->getProjectionMatrix());
	
	m = fromLocalToEye * toEyeToLocal;
	
	return m;

}


osg::Vec3 NodePathUtils::transformSpaces(const osg::NodePath& fromWorld, const osg::NodePath& toWorld, osg::Vec3 v, bool ignorecameras)
{
	return getSpaceTransform(fromWorld, toWorld, ignorecameras) * v;
}



osg::Matrixd NodePathUtils::getLocalToViewportTransform(osg::Viewport* vp, const osg::NodePath& path, bool ignorecameras) {

	osg::Camera* cam = getLastCamera(path);
	
	osg::Matrixd MVPW =  osg::computeLocalToWorld(path) * cam->getViewMatrix() * cam->getProjectionMatrix();
	if (vp) MVPW.postMult(vp->computeWindowMatrix());
	
	return MVPW;
}


osg::Matrixd NodePathUtils::getLocalToWindowTransform(const osg::NodePath& path, bool ignorecameras) {

	osg::Camera* cam = getLastCamera(path);
	if (cam->getViewport()) 
		return getLocalToViewportTransform(cam->getViewport(), path, ignorecameras);
	else {
		// diese camera hat keinen viewport, deswegen die erste Kamera ŸberprŸfen, ob diese einen hat, und diesn benutzen
		osg::Camera* firstcam = getFirstCamera(path);
		if (firstcam->getViewport())
			return  getLocalToViewportTransform(firstcam->getViewport(), path, ignorecameras);
	}
	
	return getLocalToViewportTransform(NULL, path, ignorecameras);;
}

// ----------------------------------------------------------------------------------------------------------
// getWindowToLocalTransform
// ----------------------------------------------------------------------------------------------------------

osg::Matrixd NodePathUtils::getWindowToLocalTransform(const osg::NodePath& path, bool ignorecameras) 
{
	osg::Matrixd MVPW(getLocalToWindowTransform(path, ignorecameras));	
	osg::Matrixd inverseMVPW;
	inverseMVPW.invert(MVPW);
	
	return inverseMVPW;
}


// ----------------------------------------------------------------------------------------------------------
// transformFromWindowToLocal
// ----------------------------------------------------------------------------------------------------------

osg::Vec3 NodePathUtils::transformFromWindowToLocal(const osg::NodePath& path, float tx, float ty, float tz) {

	return osg::Vec3(tx,ty,tz) * getWindowToLocalTransform(path);

}

// ----------------------------------------------------------------------------------------------------------
// transformMouseRayToLocal
// ----------------------------------------------------------------------------------------------------------



} // end of namespace 