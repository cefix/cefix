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

#ifndef ANIMATION_FACTORY_HEADER_
#define ANIMATION_FACTORY_HEADER_

#include <cefix/Export.h>
#include <osg/ref_ptr>
#include <osg/Referenced>

#include <cefix/AnimationBase.h>
#include <cefix/AnimationController.h>

namespace cefix {
	/** the AnimationFactory manages all animations and animationControllers.
	 *  You should call animate() every frame, so the animations gets a chance to 
	 *  execute.*/
	 
    class CEFIX_EXPORT AnimationFactory : public AnimationController {
        public: 
            using AnimationController::animate;
            
            /** get the singleton-instance */
            static AnimationFactory* instance();
            
            /** creates an AnimationController, adds it to the factory and returns it */
            AnimationController* createController() {
                osg::ref_ptr<AnimationController> ctrl = new AnimationController();
                add(ctrl.get());
                return ctrl.get();
            }
			
			            
            /** animates all registered animation-objects */
            inline void animate() {
				_timebase->advance();
				++_frame_count;
				if (_collectStatistics)  {
					osg::Timer t;
					if (t.delta_s(_lastTimeStatisticsCollected, t.tick()) >= 1.0f) {
						osg::notify(osg::ALWAYS) 
							<< "AnimationFactory::animated " 
							<< stat_num_animated 
							<< " (" << (stat_num_animated - _last_animated_count) << ") "
							<< " (" << (stat_num_animated / (float)(_frame_count)) << " a/s) "
							<< "deleted: " 
							<< stat_num_deleted 
							<< " (" << (stat_num_deleted - _last_deleted_count) << ") "
							<< " (" << (stat_num_deleted / (float)(_frame_count)) << " a/s) "
							<< std::endl;
						_last_animated_count = stat_num_animated;
						_last_deleted_count = stat_num_deleted;

						stat_num_animated = 0;
						stat_num_deleted = 0;
						_frame_count = 0;

						_lastTimeStatisticsCollected = t.tick();
					}
				}

                AnimationController::animate(0.0f);
            }
			/** if set to true, then the count of animated objects is dumped to the console every second */
			inline void setCollectStatistics(bool flag) { _collectStatistics = flag; }
                     
        
        protected:
            virtual ~AnimationFactory() {}
            
            
        private:
            
            
            AnimationFactory() : 
				AnimationController(0), 
				_collectStatistics(false), 
				_last_animated_count(0), 
				_last_deleted_count(0), 
				_frame_count(0)
			{ 
				init(); 
			}

			bool _collectStatistics;
			osg::Timer_t _lastTimeStatisticsCollected;
			int _last_animated_count, _last_deleted_count, _frame_count;
        friend struct AnimationfactoryIniter;

    };

}


#endif

