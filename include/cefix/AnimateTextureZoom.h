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

#ifndef ANIMATE_TEXTURE_ZOOM_HEADER
#define ANIMATE_TEXTURE_ZOOM_HEADER

#include <cefix/AnimationBasedOnDuration.h>
#include <osg/Math>

#include <cefix/Quad2DGeometry.h>

namespace cefix {

class CEFIX_EXPORT AnimateTextureZoom : public cefix::AnimationBasedOnDuration {

	public:
		AnimateTextureZoom(	float starttime, 
							float duration, 
							Quad2DGeometry* geometry, float startzoom, float endZoom) :
			cefix::AnimationBasedOnDuration(starttime, duration),
			_geometry(geometry),
			_startZoom(startzoom),
			_endZoom(endZoom),
			_rect(geometry->getTextureRect())
		{
			
		}
		
		virtual void cleanUp() {
			//std::cout << "AnimateTextureZoom::cleanup" << std::endl;
			computeRect(_endZoom);
		}
		
	protected:
		virtual void animate(float elapsed_time) {
			cefix::AnimationBasedOnDuration::animate(elapsed_time);
			computeRect(interpolate(_startZoom, _endZoom));
		}
		
		virtual ~AnimateTextureZoom() {}

	
	private:
		
		inline void computeRect(float zoom) {
			float w = (_rect[2] - _rect[0]);
			float h = (_rect[3] - _rect[1]);
			float x = w / 2.0f;
			float y = fabs(h / 2.0f);
			
			float dw = w / zoom / 2.0f;
			float dh = h / zoom / 2.0f;
			
			_geometry->setTextureRect(osg::Vec4( x - dw, y-dh, x+dw, y+dh));
		}
		osg::ref_ptr<Quad2DGeometry> _geometry;
		float _startZoom, _endZoom;
		osg::Vec4 _rect;
};

}

#endif