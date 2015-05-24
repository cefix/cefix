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

#ifndef NODEPATH_UTILS_HEADER
#define NODEPATH_UTILS_HEADER

#include <osg/Matrix>
#include <osg/Vec3>
#include <osg/Transform>
#include <osg/Viewport>
#include <osg/Camera>
#include <cefix/Export.h>

namespace cefix {
	
	class CEFIX_EXPORT NodePathUtils {
	
		public:
			/** get a matrix which will transform vertices from one local coordinate-system to another system, say from a 3D-view into a 2D-HUD 
			 * if fromWorld or toWorld is NULL, then transformation to world-coords is assumed*/
			static osg::Matrixd getSpaceTransform(const osg::NodePath& fromWorld, const osg::NodePath& toWorld, bool ignorecameras = true);
			
			/** apply a transform from one space to another to a vertice */
			static osg::Vec3 transformSpaces(const osg::NodePath& fromWorld, const osg::NodePath& toWorld, osg::Vec3 v, bool ignorecameras = true);
			
			static osg::Matrixd getLocalToViewportTransform(osg::Viewport* vp, const osg::NodePath& path, bool ignorecameras = true);
			static osg::Matrixd getLocalToWindowTransform(const osg::NodePath& path, bool ignorecameras = true);
			static osg::Matrixd getWindowToLocalTransform(const osg::NodePath& path, bool ignorecameras = true);
			
			/** transforms a vertice from window-space to local-space */
			static osg::Vec3 transformFromWindowToLocal(const osg::NodePath& path, float tx, float ty, float tz = 0.0f);
			
			
		private:
			static osg::Camera* getLastCamera(const osg::NodePath& path) ;
			static osg::Camera* getFirstCamera(const osg::NodePath& path) ;

	};


}


#endif