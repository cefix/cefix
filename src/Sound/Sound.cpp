/*
 *  Sound.cpp
 *  render_tests
 *
 *  Created by Stephan Huber on 13.10.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sound.h"
#include "SoundStateObserverThread.h"

namespace cefix {

bool Sound::play() {
	if (!valid()) return false;
	if (!_isPlaying) {
		_isPlaying = _impl->play();
		SoundStateObserverThread::getInstance()->addSound(this);
		
	}
	return _isPlaying;
}

}