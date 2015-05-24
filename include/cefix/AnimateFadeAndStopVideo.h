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

#ifndef ANIMATE_FADE_ANDSTOP_QUICKTIME_MOVIE_HEADER_
#define ANIMATE_FADE_ANDSTOP_QUICKTIME_MOVIE_HEADER_
#include <cefix/Video.h>
#include <cefix/AnimationBasedOnDuration.h>

namespace cefix {

/** 
 * this class fades the sound of a quicktime-movie to silence, 
 * and when finished stops the playback.
 */
class CEFIX_EXPORT AnimateFadeAndStopVideo : public cefix::AnimationBasedOnDuration {

	public:
		/** ctor */
		AnimateFadeAndStopVideo(
			float starttime, 
			float duration, 
			cefix::Video* movie
		) :
			cefix::AnimationBasedOnDuration(starttime, duration),
			_movie(movie)
		{
			_volume = _movie->getVolume();
		}
		
		/** stops the movie and restore the volume-settings */
		void cleanUp() {
			cefix::AnimationBasedOnDuration::cleanUp();
			
			_movie->pause();
			_movie->rewind();
			_movie->setVolume(_volume);
		}
		
	protected:
		/** do the fade */
		virtual void animate(float elapsed_time) {
			cefix::AnimationBasedOnDuration::animate(elapsed_time);
			
			_movie->setVolume(_volume * (1 - _scalar));
		}
		virtual ~AnimateFadeAndStopVideo() {}
		
		osg::ref_ptr<cefix::Video> _movie;
		float								_volume;

};

}

#endif