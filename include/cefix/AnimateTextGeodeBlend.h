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

#ifndef ANIMATE_TEXT_GEODE_BLEND_
#define ANIMATE_TEXT_GEODE_BLEND_

#include <cefix/AnimationBasedOnDuration.h>
#include <cefix/TextGeode.h>

namespace cefix {

/** this class animates/fades the transparency of a text-geode */
template <class T> class AnimateTextGeodeBlend : public cefix::AnimationBasedOnDuration {

    public:
		/** ctor */
        AnimateTextGeodeBlend(  float startTime, float duration, T* textgeode, 
                                float startBlend, float endBlend, bool initTextColor = true)
            : AnimationBasedOnDuration(startTime, duration),
            _textgeode(textgeode),
            _startBlend(startBlend),
            _endBlend(endBlend) 
        {
            
            osg::Vec4 c = _textgeode->getTextColor();
            _orgBlend = c[3];
			if (initTextColor) {
				c[3] = startBlend;
				_textgeode->setTextColor(c);
			}
        }
                
    protected:
        /** do the animation */
        virtual void animate(float elapsed_time) {
            
            cefix::AnimationBasedOnDuration::animate(elapsed_time);
            float blend = _orgBlend * interpolate<float>(_startBlend, _endBlend);
            _textgeode->setAlpha(blend);
        
        }
		
		virtual ~AnimateTextGeodeBlend() {}
        
    
        osg::ref_ptr<T>							_textgeode;
        float                                   _startBlend, _endBlend, _orgBlend;
};

}

#endif