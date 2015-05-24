/*
 *  TiledRenderingHelper.cpp
 *  fourWindows
 *
 *  Created by Stephan Huber on 04.02.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "TiledProjectionHelper.h"

namespace cefix {

osg::Matrix TiledProjectionHelper::getOffsetForProjection(float ax, float ay, float aw, float ah, bool flipy)
{
	float x(ax),y(ay), w(aw), h(ah);
	
	
	
	// see http://cplusplus.codefetch.com/example/nw/hqaa/hqaa.cpp?qy=malloc for more info.
	
	osg::Matrix scr2ndc(
		0.5,  0,  0, 0,
		  0, -0.5, 0, 0,
		  0,   0,  1, 0,
		 0.5, 0.5, 0, 1);
		 
	osg::Matrix ndc2ras(
			getWidth(),  0,             0, 0,
			 0,            getHeight(), 0, 0,
			 0,            0,       1, 0,
			 0,            0,       0, 1);
	
	osg::Matrix cam2ras = scr2ndc * ndc2ras;
	
	float flipper = flipy ? -1.0f : 1.0f;
	float offset  = flipy ?  1.0f  : 0.0f;
	

	
	
	osg::Matrix ras2ndc(
		1.0f/getWidth(),	0,					0, 0,
		0,				flipper/getHeight(),	0, 0,
		0,				0,					1, 0,
		0,				offset,				0, 1
	);
	float xndc =  x / getWidth();
	float yndc =  y / getHeight();
	
	float inv_wndc = getWidth()  / w;
	float inv_hndc = getHeight() / h;
	
	osg::Matrix t0 = osg::Matrix::translate(osg::Vec3(-xndc, -yndc,0));
	osg::Matrix s  = osg::Matrix::scale(2*inv_wndc, -2*inv_hndc, 1);
	osg::Matrix t1 = osg::Matrix::translate(osg::Vec3(-1,1,0));
	osg::Matrix ndx2tile = t0 * s * t1;
	osg::Matrix cam2tile = cam2ras * ras2ndc * ndx2tile;
	
	
	return cam2tile;
	
}



osg::Matrix TiledProjectionHelper::getRotatedOffsetForProjection(float ax, float ay, float aw, float ah,  WindowRotation::Rotation rotation,bool flipy) {
	
	// std::cout << "getRotatedOffsetForProjection for " << ax << "/" << ay << " - " << ax+aw << "/" << ay+ah << " out of " << getWidth() << "x" << getHeight() << std::endl;
	// std::cout << "result: " << getOffsetForProjection(ax,ay,aw,ah, flipy) << std::endl;
	WindowRotation rot( rotation );
	

	return getOffsetForProjection(ax,ay,aw,ah, flipy) * osg::Matrix::rotate(rot.getAngleForRotation(), osg::Vec3(0,0,1));
	
	}



osg::Matrix TiledProjectionHelper::getOffsetForOrtho(float ax, float ay, float aw, float ah, WindowRotation::Rotation arotation) 
	{		
		float x(ax), y(ay),  h(ah);
		float fh(getHeight());
		
		switch (arotation) {

			case WindowRotation::TILTED_LEFT:
				return osg::Matrix::translate(fh-h-y,-x,0);
			
			case WindowRotation::TILTED_RIGHT:
				return osg::Matrix::translate(+y-fh+h,x,0);
			
			case WindowRotation::UPSIDE_DOWN:
				return osg::Matrix::translate(x,fh-y-h,0);
			default:
				return osg::Matrix::translate(-x,y+h-fh,0);
		}
	}


}