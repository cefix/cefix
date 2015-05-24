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

#ifndef ANIMATION_CONTROLLER_HEADER_
#define ANIMATION_CONTROLLER_HEADER_

#include <cefix/Export.h>
#include <cefix/Functor.h>
#include <list>
#include <vector>
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/Timer>
#include <cefix/Log.h>
#include <cefix/AnimationBase.h>
#include <cefix/AnimationTimeBase.h>

namespace cefix {
	
	template<class T>
	class CallFunctorAnimation : public cefix::AnimationBase {
	
		
	public:
		CallFunctorAnimation(T*t, void(T::*fpt)(AnimationBase*), float starttime = 0.0f) : cefix::AnimationBase(starttime), _t(t), _fpt(fpt) {}
		
	protected:
		virtual void animate(float elapsed) {
			(*_t.*_fpt)(this);
		}
	private:
		osg::ref_ptr<T> _t;
		void(T::*_fpt)(AnimationBase*);
	};
	
	template<class T>
	class CallBoolFunctorAnimation : public cefix::AnimationBase {
	
		
	public:
		CallBoolFunctorAnimation(T*t, bool(T::*fpt)(), float starttime = 0.0f) : cefix::AnimationBase(starttime), _t(t), _fpt(fpt) {}
		
	protected:
		virtual void animate(float elapsed) {
			if ( (*_t.*_fpt)())
				setFinishedFlag();
		}
	private:
		osg::ref_ptr<T> _t;
		bool(T::*_fpt)();
	};
	
	template<class T>
	class CallFunctorWithTimeAnimation : public cefix::AnimationBase {
	
		
	public:
		CallFunctorWithTimeAnimation(T*t, bool(T::*fpt)(float), float starttime = 0.0f) : cefix::AnimationBase(starttime), _t(t), _fpt(fpt) {}
		
	protected:
		virtual void animate(float elapsed) {
			if ( (*_t.*_fpt)(elapsed))
				setFinishedFlag();
		}
	private:
		osg::ref_ptr<T> _t;
		bool(T::*_fpt)(float);
	};


	/** this class helps to control a bunch of Animations. Every animation has a 
	 *  starttime, and gets started if it's time to.
	 *  finished animations get cleaned automatically.
	 *  A typical workflow would be:
	 *  create a controller, add a bunch of animations with different starttimes to it
	 *  and add the controller to the AnimationFactory.
	 *  You can even attach an AnimationController to another AnimationController, 
	 *  so you can construct a full hierarchy of Animations, and they get started 
	 *  automatically at the right time.
	 */
    class CEFIX_EXPORT AnimationController : public AnimationBase {
        
        public:
			/** cleanupFunctor-class*/

			template <class TClass> class CleanupFunctorT : public FunctorT<void, TClass>	{
				public:
					CleanupFunctorT(TClass* object, void(TClass::*fpt)()): FunctorT<void, TClass>(object, fpt) {}
			};
				
            /** a list full of Animations */
            typedef std::list<osg::ref_ptr< AnimationBase > >  AnimationList;
            
            /** constructs an animationController
             * @param starttime optional starttime
             */
            AnimationController(float startTime = 0) : 
				AnimationBase(startTime), 
				_timebase(AnimationTimeBase::instance()),
				_currentTime(0.0),
				_timeMultiplier(1.0f)
			{
			}
            
            /** adds an aniation-object */
            void add(AnimationBase* ab) { 
				OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
                ab->setParent(this);
				_list.push_back(ab); 
			}
            
            /** adds an aniation-object */
            void addPrioritized(AnimationBase* ab) { 
				OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
				_list.push_front(ab); 
			}
		
			template<class T> void add(T* t, void(T::*fpt)(AnimationBase*), float starttime = 0.0f) {
				add(new cefix::CallFunctorAnimation<T>(t, fpt, starttime));
			}
			
			template<class T> void add(T* t, bool(T::*fpt)(), float starttime = 0.0f) {
				add(new cefix::CallBoolFunctorAnimation<T>(t, fpt, starttime));
			}
			
			
			template<class T> void add(T* t, bool(T::*fpt)(float), float starttime = 0.0f) {
				add(new cefix::CallFunctorWithTimeAnimation<T>(t, fpt, starttime));
			}
            
            /** init the animation-process */
            virtual void init() {
				AnimationBase::init();
				_timebase->update();
                _startTime = _timebase->getCurrentTime();
            }
			
			/** sets the time-multiplier normal speed = 1.0 faster > 1.0, slower < 1.0 */
			void setTimeMultiplier(float m) { _timeMultiplier = m; }
			/** gets the current time mutiplier */
			float getTimeMultiplier() const { return _timeMultiplier; }
            
            /** animate all children */
            virtual void animate(float unused_delta_s) {
                
                _currentTime = (_timebase->getCurrentTime() - _startTime) * _timeMultiplier;
				animate_implementation(_currentTime);
			}
            
            /** clean up when the animation is finished, call all cleanup-handlers */
            virtual void cleanUp() {
                for(AnimationList::iterator i = _list.begin(); i != _list.end(); ++i) {
                    (*i)->cleanUp();
                }
                _list.clear();
                
                // call all cleanupHandler
                if (_cleanupHandler.size() > 0) {
                    
                    for(CleanupHandlerVector::iterator i = _cleanupHandler.begin(); i != _cleanupHandler.end(); ++i) {
                        if ((*i).get()->valid()) (*i).get()->call();
                    }
                    _cleanupHandler.clear();
                }
            }
			
			/** removes an animation from the controller */
			bool remove(AnimationBase* animation) {
				for(AnimationList::iterator i = _list.begin(); i != _list.end(); ) {
                    if ((*i) == animation) {
						animation->cancel();
						animation->cleanUp();
						 i = _list.erase(i);
						return true;
					} else
						++i;
				}
				return false;						
			}
			
			/** cancel all animation of this controller, the cleanUp-method is called on
			 *  all animation-objects
			 */
			virtual  void cancel() {
				for(AnimationList::iterator i = _list.begin(); i != _list.end(); i++) {
                    (*i)->cancel();
                }
				cleanUp();
				_finished = true;
			}
            /** adds a cleanup-handler which gets called, when the controller is finished with the animation */
            void addCleanupFunctor(FunctorAbstractT<void>* nf) {
                _cleanupHandler.push_back(nf);
            }
			
			template<class T> void addCleanupFunctor(T* t, void(T::*fpt)()) {addCleanupFunctor(new CleanupFunctorT<T>(t,fpt)); }
            
			double getCurrentTime() { return _currentTime; }
            
            void dump(int delta=0);
            
        protected:
			/** implementation of the animate-function, so it can be called by subclasses of this class */
			void animate_implementation(float delta_s);

            virtual ~AnimationController() { cleanUp(); }
            AnimationList   _list;
			osg::ref_ptr<AnimationTimeBase> _timebase;
            double			_startTime, _currentTime;
			float			_timeMultiplier;
            
            typedef std::vector<osg::ref_ptr<FunctorAbstractT<void> > > CleanupHandlerVector;
            CleanupHandlerVector _cleanupHandler;

			static int stat_num_animated;
			static int stat_num_deleted;
			
			OpenThreads::Mutex _mutex;
        
    
    };
    



} // end of namespace
#endif
