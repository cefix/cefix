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
 
#ifndef _ANIMATE_QUAD_2D_TRANSPARENCY_HEADER_
#define _ANIMATE_QUAD_2D_TRANSPARENCY_HEADER_

#include <cefix/AnimationBasedOnDuration.h>
#include <cefix/Quad2DGeometry.h>

namespace cefix {

/** this class implements an animation-class which animates the transpaency of a given Quad2DGeometry */
class CEFIX_EXPORT AnimateQuad2DTransparency : public cefix::AnimationBasedOnDuration {

    public:
        /** ctor,
         *  @param starttime starttime to start the fading
         *  @param duratin duration of the fade
         *  @param geo the quad2Dgeometry to fade
         *  @param startblend the transparency-value to begin with
         *  @param endblend the transparency-value to end with
         */
        AnimateQuad2DTransparency(float starttime, float duration, Quad2DGeometry* geo, float startblend, float endblend, bool doWipe = true);
        
		virtual void cleanUp() {
			osg::Vec4 c = _geo->getColor();
			c[3] = _endblend;
			_geo->setColor(c);
		}
        
    protected:
        /** do the fade */
        virtual void animate(float elapsed) {
            AnimationBasedOnDuration::animate(elapsed);
            
			if (_doWipe == false) {
				osg::Vec4 c = _geo->getColor();
				c[3] = cefix::interpolate(_startblend,_endblend, _scalar);
				_geo->setColor(c);
				return;
			}
			
			float rightscalar = 0;
			float leftscalar = 0;
			float d = 1/3.0f;
			float scalar = elapsed/_duration;
			if (scalar >= d) {
				rightscalar = cefix::easyInOut((scalar - d) / (1-d));
			}
			leftscalar = cefix::easyInOut(osg::minimum((scalar) / (1-d), 1.0f));
			//std::cout <<" scalar: " << _scalar << " left: " << leftscalar << " right: " << rightscalar << std::endl;
			(*_colors)[0][3] = cefix::interpolate(_startblend,_endblend, leftscalar);
			(*_colors)[3][3] = cefix::interpolate(_startblend,_endblend, leftscalar);
			
			(*_colors)[1][3] = cefix::interpolate(_startblend,_endblend, rightscalar);
			(*_colors)[2][3] = cefix::interpolate(_startblend,_endblend, rightscalar);
			
            
            _geo->dirtyDisplayList();
        }
		
		virtual ~AnimateQuad2DTransparency() {}
        
        osg::ref_ptr<Quad2DGeometry>    _geo;
        float                           _startblend, _endblend;
		osg::ref_ptr<osg::Vec4Array>	_colors;
		bool							_doWipe;
};

}

#endif
