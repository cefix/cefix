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

#ifndef ANIMATION_BASED_ON_DURATION_HEADER_
#define ANIMATION_BASED_ON_DURATION_HEADER_

#include <cefix/Export.h>
#include <osg/Math>
#include <osg/Matrix>
#include <cefix/Log.h>
#include <cefix/AnimationBase.h>
#include <cefix/MathUtils.h>

namespace cefix {
	/** this class implements a time-based animation. it has a starttime and a duration
	 * it calculates automatically the scalar (0..1)
	 * you can even specify easyinEasyOut */
    class CEFIX_EXPORT AnimationBasedOnDuration : public AnimationBase {
    
        public:
            /** default constructor
             * @param startTime staring time for this animation object 0 = as soon as possible
             * @param duration ime in seconds for this animation
			 * @param easyInOut uses EasyIn EasyOut-Animation
             */
            AnimationBasedOnDuration(float startTime = 0, float duration = 1.0f, bool easyInOut = true);
            
            /** calculates the scalar and if the animation is finished. Call this in your derived classes! 
			 *  @param delta_s time in seconds since the start of the animation
			 */
            virtual void animate(float delta_s) {
                
                _scalar = osg::minimum(1.0f, delta_s/_duration);
                if (_easyInOut)
                    _scalar = cefix::easyInOut(_scalar);
                
                _finished = (_duration <= delta_s);
            }
            /** helper method to interpolate between two values*/
            template< class T> T interpolate(T a, T b) { return a * (1.0f-_scalar) + b * (_scalar); }
            
            
            

        protected: 
			/** set the easyinout-flag */
			inline void useEasyInOut(bool f) { _easyInOut = f; }
			/** gets the current scalar 0 = start 1 = finished */
			inline float getScalar() { return _scalar; }
			
			virtual ~AnimationBasedOnDuration() {}
			
            float _duration;
            float _scalar;
            bool _easyInOut;
    
    
    };


}



#endif
