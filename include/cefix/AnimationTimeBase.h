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

#ifndef ANIMATION_TIME_BASE_HEADER
#define ANIMATION_TIME_BASE_HEADER

#include <osg/Referenced>
#include <osg/Timer>
#include <osg/ref_ptr>
#include <cefix/Export.h>

namespace cefix {
/** this classe provides a "time" for the animationfactory and their classes, it can be feed with absolute time-deltas, or from a "real-time" */
class CEFIX_EXPORT AnimationTimeBase : public osg::Referenced {
	public:
		/** base implementation class */
		class Implementation : public osg::Referenced {
			
			public:
				/** advance the current time */
				virtual double advance() = 0;
				
				/** update the current time, if approbiate */
				virtual double update() { return 0; }
		};
		
		/** realtime implementation */
		class RealTimeImplementation : public Implementation {
		
			public:
				RealTimeImplementation() : Implementation(), _current(0) { _last = _t.tick(); }
				inline virtual double advance() { _current = _t.delta_s(_last, _t.tick()); return _current; }
				inline virtual double update() { return advance(); }
			
			private:
				double	_current;
				osg::Timer_t _last;
				osg::Timer	 _t;
		};
		
		/** fixed step implementation */
		class FixedStepImplementation : public Implementation {
			public:
				FixedStepImplementation(double step) : Implementation(), _current(0), _step(step), _paused(false) {}
				
				inline virtual double advance() 
				{ 
					if (!_paused) 
						_current += _step; 
					return _current; 
				}
				
				inline virtual double update() { return _current; }
				
				bool isPaused() const { return _paused; }
				void setPausedFlag(bool b) { _paused = b; }
			private:
				double _current, _step;
				bool _paused;
		};
		
		/** singleton-getter */
		static AnimationTimeBase* instance();
		
		/** returns the current time */
		inline double getCurrentTime() { return _current; }
		
		inline void update() { _current = _implementation->update(); }
		
		inline void advance() { _current = _implementation->advance(); }
		
		/** sets the implementation, providing the current time */
		void setImplementation(Implementation* impl) { if (impl) _implementation = impl; update(); }
		/** returns the current uses implementation */
		Implementation* getImplementation() { return _implementation.get(); }
		
	protected:
		AnimationTimeBase() : osg::Referenced(), _current(0), _implementation(new RealTimeImplementation()) {}
		
	private:
		double _current;
		osg::ref_ptr<Implementation> _implementation;
};

}

#endif