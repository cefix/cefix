/*
 *  ScreenRotation.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 28.01.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "WindowRotation.h"
#include <cefix/DataFactory.h>
#include <cefix/StringUtils.h>
#include <osg/Camera>
#include <cefix/EnumUtils.h>

namespace osg {
	class Camera;
}

namespace cefix {

WindowRotation::WindowRotation(Rotation rotation)
: _rotation(rotation)
{
	if (_rotation == UNKNOWN) {
		_rotation = readFromPrefs("/Viewer/mainwindow/rotation");
	}
}

WindowRotation::WindowRotation(const std::string& prefskey)
:	_rotation(readFromPrefs(prefskey))
{
}


EnumAsString<WindowRotation::Rotation, 5>& WindowRotation::getRotationEnumsAsString()
{
	static Rotation enums[5] = {UNKNOWN, NONE, TILTED_LEFT, TILTED_RIGHT, UPSIDE_DOWN };
	static const char* enum_strs[5] = {"UNKNOWN", "NONE", "TILTED_LEFT", "TILTED_RIGHT", "UPSIDE_DOWN" };
	static cefix::EnumAsString<Rotation, 5> enum_as_string(enums, enum_strs);
	
	return enum_as_string;
}


WindowRotation::Rotation WindowRotation::readFromPrefs(const std::string prefsKey) 
{
	// Rotation ermitteln:
	
	std::string key = cefix::strToLower(DataFactory::instance()->getPreferencesFor<std::string>(prefsKey, "none"));
	
	return getRotationEnumsAsString().value(key, UNKNOWN);
	/*
	
	static std::map<std::string, Rotation>  dr_map;
	if (dr_map.size() == 0) 
	{
		dr_map["none"]			= NONE;
		dr_map["tilted_left"]	= TILTED_LEFT;
		dr_map["tilted_right"]	= TILTED_RIGHT;
		dr_map["upside_down"]	= UPSIDE_DOWN;
	}
	if (dr_map.find(key) == dr_map.end()) 
	{
		osg::notify(osg::WARN) << "ScreenRotation:: unknown rotation " << key << ", disregarding... " << std::endl;
		return UNKNOWN;
	}
	else 
		return dr_map[key];
	*/
}


// ----------------------------------------------------------------------------------------------------------
// getDisplayRotationAngle
// ----------------------------------------------------------------------------------------------------------

double WindowRotation::getAngleForRotation() {
	
	switch(_rotation) {
		case UNKNOWN:
		case NONE:
			return 0;
			break;
		case TILTED_LEFT:
			return osg::PI_2;
			break;
			
		case UPSIDE_DOWN:
			return osg::PI;
			break;
			
		case TILTED_RIGHT:
			return 3*osg::PI_2;
			break;
	}
	
	return 0;
}


osg::Matrix WindowRotation::computeProjectionMatrix( double hfov, double vfov, float nearplane, float farplane) 
{

	osg::Matrix drm = osg::Matrix::rotate(getAngleForRotation(), osg::Vec3(0,0,1));
	
	double ratio = _height/(double)_width;
	
	if (isTilted())
			ratio = _width/(double)_height;	
			
	if ((hfov < 0) &&  (vfov < 0))
		hfov = 60;
		
	// ggf FieldOfViews berechnen 
	if ((hfov > 0) || (vfov > 0)) {
	
		if ((hfov > 0) &&  (vfov > 0)) {
			if (isTilted())
				std::swap(hfov,vfov);
					
		}
		if (hfov < 0)
			hfov = osg::RadiansToDegrees(2.0 * atan (tan(osg::DegreesToRadians(vfov)/2.0) * (1/ratio) ));
		else if (vfov < 0)
			vfov = osg::RadiansToDegrees(2.0 * atan (tan(osg::DegreesToRadians(hfov)/2.0) * (ratio) ));
    
	} else {
		hfov = 0;
		vfov = 0;
	}
	
	if ((hfov > 0) || (vfov > 0)) {
		
		float _hfov, _vfov, _left, _right, _bottom, _top;
		double _aspect_ratio;
		_hfov = osg::DegreesToRadians(hfov);
		_vfov = osg::DegreesToRadians(vfov);
		_aspect_ratio = tan(0.5*_hfov)/tan(0.5*_vfov);

		_left   = -nearplane * tan(_hfov/2.0); 
		_right  =  nearplane * tan(_hfov/2.0); 
		_bottom = -nearplane * tan(_vfov/2.0);
		_top    =  nearplane * tan(_vfov/2.0);

		return drm * osg::Matrix::frustum(_left, _right, _bottom,_top,  nearplane, farplane);
	}
	
	return drm * osg::Matrix::frustum(-1, 1, -ratio, ratio, nearplane, farplane);
}

osg::Camera* WindowRotation::createHudCamera(unsigned int w, unsigned int h, const osg::Matrix& offset)
{
	 osg::Camera* camera = new osg::Camera;
	 setupHudCamera(camera, w, h, offset);
	 
	 return camera;
}
		
 
void WindowRotation::setupHudCamera(osg::Camera* camera, unsigned int w, unsigned int h, const osg::Matrix& offset) 
{

    osg::Matrix drm = osg::Matrix::rotate(getAngleForRotation(), osg::Vec3(0,0,1));
	
    double half_width = w / 2.0;
	double half_height = h / 2.0;

	if (isTilted())
			std::swap(w,h);
            
    
				
    //Camera-HUD erzeugen
   
	// don't compute the near and far planes
	camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
		
    // set the projection matrix
    osg::Matrix m = osg::Matrix::ortho2D(0, w, 0,h);
	m = osg::Matrix::scale(2.0/w, 2.0/h, -2/1000.0f) * osg::Matrix::translate(-1, -1, 1);
	// std::cout << "links oben: " << (m * osg::Vec3(50,50,0)) << " rechts unten: " << (m * osg::Vec3(750, 550, 0)) << std::endl;
    camera->setProjectionMatrix(offset * m);


    // set the view matrix    
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    m = osg::Matrix::identity(); // osg::Matrix::scale(1.0f,1.0f,1/1000.0f);
	m = osg::Matrix::translate(-half_width, -half_height, 0 );
	m *= drm;
    if (isTilted())
        m *= osg::Matrix::translate(half_height, half_width, 0);
    else
        m *= osg::Matrix::translate(half_width,half_height, 0);
    camera->setViewMatrix(m);

    // only clear the depth buffer
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);

    // draw subgraph after main camera view.
    camera->setRenderOrder(osg::Camera::POST_RENDER);
}



}