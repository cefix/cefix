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

#ifndef ANIMATION_BASE_HEADER_
#define ANIMATION_BASE_HEADER_

#include <cefix/Export.h>
#include <osg/Referenced>
#include <cefix/Log.h>

namespace cefix {
    class AnimationController;
	/** 
	 * AnimationBase is the base class for all animations. Every animation has 
	 * a starttime in seconds and a  flag, if the animation is finished, and the 
	 * object can be disposed. An instance of this class should be used only once, 
	 * create a new instance if needed. */
    class CEFIX_EXPORT AnimationBase : public virtual osg::Referenced {
    
        public:
            /** default constructor
             * @param startTime staring time for this animation object 0 = as soon as possible
             */
            AnimationBase(float startTime = 0);
			
			/** resets the object, don't call it from inside an animation, unpredicted behavior could occur */
			inline void reset() {
				_finished = _inited = _cancelled = false;
			}
            
			/** @return true, if the animation is finished */
            inline bool isFinished() const { return (_finished || _cancelled); }
			
			/** @return true if this object is already inited */
			inline bool isInited() const { return _inited; }
			
			/** @return the starttime in seconds */
            inline float getStartTime() const { return _startTime; }
			
			/** sets the starttime */
			inline void setStartTime(float starttime) { _startTime = starttime; }
            			
			/** implement this method, if you need special init-stuff. 
			 *  This method gets called before the animation is started.
			 */
            virtual void init() { _finished = false; _inited = true; _cancelled = false; }
            
			/** implement the animation inside this method. call SetFinishedFlag(),
			 *  if you are finished with this animation, and the object can be disposed
			 *  @param delta_s time in seconds since the animation started
			 */
			virtual void animate(float delta_s) = 0;
            
			/** implement this method, if you have to do any cleanup, before the object
			 *  gets removed from the animationController
			 */
			virtual void cleanUp() {}
            
            virtual void cancel() { _cancelled = true; }
            
            AnimationController* getParent() const { return _parent; }
            void setParent(AnimationController* parent) { _parent = parent; }
            
        protected: 
			 AnimationBase(float startTime, bool dontobserve);

            virtual ~AnimationBase() { /*osg::notify(osg::ALWAYS) << "AnimationBase destructed" << std::endl;*/ }
			
			/** sets the finished-flag to true */
			inline void setFinishedFlag() { _finished = true; }
            bool    _finished, _inited, _cancelled;
            float   _startTime;
            AnimationController* _parent;
			
		friend class AnimationController;
    
    };


}



#endif
