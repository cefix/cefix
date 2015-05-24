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

#ifndef ANIMATE_START_AND_FADE_QUICKIME_HEADER_
#define ANIMATE_START_AND_FADE_QUICKIME_HEADER_

#include <cefix/Video.h>
#include <cefix/AnimationBasedOnDuration.h>

namespace cefix {

class CEFIX_EXPORT AnimateStartAndFadeVideo : public cefix::AnimationBasedOnDuration {

	public:
		/** ctor */
		AnimateStartAndFadeVideo(
			float starttime, 
			float duration, 
			cefix::Video* movie
		) :
			cefix::AnimationBasedOnDuration(starttime, duration),
			_movie(movie)
		{
			_volume = _movie->getVolume();
			_movie->pause();
			_movie->rewind();
		}
		
		/** stops the movie and restore the volume-settings */
		void cleanUp() {
			cefix::AnimationBasedOnDuration::cleanUp();

			_movie->setVolume(_volume);
		}
		
	protected:
		/** do the fade */
		virtual void animate(float elapsed_time) {
			cefix::AnimationBasedOnDuration::animate(elapsed_time);
			
			_movie->setVolume(_volume * (_scalar));
			if (!_movie->isPlaying())
				_movie->play();
		}
		
		virtual ~AnimateStartAndFadeVideo() {}
		
		osg::ref_ptr<cefix::Video> _movie;
		float								_volume;

};

}
#endif