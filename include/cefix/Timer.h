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

#ifndef CEFIX_TIMER_HEADER
#define CEFIX_TIMER_HEADER

#include <osg/observer_ptr>

#include <cefix/Export.h>
#include <cefix/AnimationBase.h>
namespace cefix {

/** a simple timer-class -- calls timer() at regulare intervalls, every frequency ms. To use this class subclass from it and implement the timer-method */
class CEFIX_EXPORT Timer : public cefix::AnimationBase {

	public:
		/** ctor
		 *  @param ms ms between calls
		 *  @param maxCount call the timer maxCount and stop the timer afterwards
		 */
		Timer(float ms, unsigned int maxCount=0) : cefix::AnimationBase(0), _freq(ms/1000.0), _maxCount(maxCount), _count(0),_inited(false), _stopped(false), _debug(false) {}
		
		/** start the timer */
		void start();
		/** stop it */
		inline void stop() { setFinishedFlag(); _stopped = true; }
		
		/** implement this method by a subclass. */
		virtual void timer() = 0;
		
		/** get the count */
		inline unsigned int getCount() const { return _count; }
		
		/** get the frequency */
		inline double getFrequency() const { return _freq*1000; }
		
		/** get the maxcount */
		inline unsigned int getMaxCount() const { return _maxCount; }

		inline void setDebug(bool debug) { _debug = debug;}
		
	protected:
		/** do the math and call timer() */
		inline virtual void animate(float elapsed) 
		{
			if (!_inited) {
				_startTick = elapsed + _freq;
				_inited = true;
				return;
			}

			if (_debug)
				osg::notify(osg::ALWAYS) << "e: " << elapsed << "starttime " << _startTick << " freq: " << _freq << " count: " << _count << " max: " << _maxCount << " stopped: " << _stopped << std::endl;
			
			if (_startTick > elapsed || _stopped) {
                if (_stopped) setFinishedFlag();
				return;
            }
			
			timer();
			
			_startTick += _freq;
			++_count;
			if ((_maxCount != 0) && (_count >= _maxCount))
				setFinishedFlag();	
		}
		
	private:
		double _freq;
		float  _startTick;
		unsigned int _maxCount, _count;
		bool	_inited, _stopped, _debug;
};

template <class T, class TArg>
class FunctorTimerT : public Timer {
	public:
		FunctorTimerT(float ms, T* t, void (T::*fpt)(TArg), unsigned int maxCount=0) 
		:	Timer(ms, maxCount), 
			_t(t), 
			_fpt(fpt) 
		{
		}
		
		
		inline virtual void timer() 
		{
			if (_t.valid())
				(*_t.*_fpt)(static_cast<TArg>(this));
			else
				stop();
		}
		
	private:
		osg::observer_ptr<T> _t;
		void (T::*_fpt)(TArg);
		
};

template <class T>
class FunctorTimerT<T,void> : public Timer {
	public:
		FunctorTimerT<T,void>(float ms, T* t, void (T::*fpt)(), unsigned int maxCount=0) 
		:	Timer(ms, maxCount), 
			_t(t), 
			_fpt(fpt) 
		{
		}
		
		
		inline virtual void timer() 
		{
			if (_t.valid())
				(*_t.*_fpt)();
			else
				stop();
		}
		
	private:
		osg::observer_ptr<T> _t;
		void (T::*_fpt)();
		
};


class FunctorTimer {
	public:
	template<class T>  static FunctorTimerT<T,Timer*>* create(float millisec, T* t, void (T::*fpt)(Timer*), unsigned int maxCount=0) {
		return new FunctorTimerT<T, Timer*>(millisec, t, fpt, maxCount);
	}
	
	template<class T>  static FunctorTimerT<T, void>* create(float millisec, T* t, void (T::*fpt)(), unsigned int maxCount=0) {
		return new FunctorTimerT<T, void>(millisec, t, fpt, maxCount);
	}
};

}

#endif