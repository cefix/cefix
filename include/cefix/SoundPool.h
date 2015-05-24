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

#ifndef CEFIX_SOUND_POOL_HEADER
#define CEFIX_SOUND_POOL_HEADER

#include <cefix/Sound.h>
#include <cefix/RingBuffer.h>

namespace cefix {

template <unsigned int SIZE>
class SoundPool : public osg::Referenced {

public:
	typedef cefix::RingBuffer< osg::ref_ptr<cefix::Sound>, SIZE> Pool;
	SoundPool(const std::string& soundfile) 
	:	osg::Referenced(), 
		_pool()
	{
		for(unsigned int i = 0; i < _pool.size(); ++i) {
			osg::ref_ptr<cefix::Sound> sound = cefix::SoundManager::readSoundFile(soundfile);
			
			sound->seek(0);
			sound->setVolume(0.0);
                
			_pool.add(sound);
		}
	}
	
	cefix::Sound* getNext() { return _pool.getNext().get(); }

private:
	Pool _pool;
};

}

#endif