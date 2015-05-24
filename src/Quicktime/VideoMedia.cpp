/*
 *  VideoMedia.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 02.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */

#include <cefix/VideoMedia.h>
#include <cefix/VideoMediaManager.h>

namespace cefix {

// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

VideoMedia::VideoMedia() : osg::Referenced() , _isRunning(false) {
	
	static int s_id = 0;
	_id = s_id++;
	VideoMediaManager::instance()->add(this);
}


// ----------------------------------------------------------------------------------------------------------
// dtor
// ----------------------------------------------------------------------------------------------------------

VideoMedia::~VideoMedia() {
	VideoMediaManager* qtmm = VideoMediaManager::instance();
	if (qtmm) qtmm->remove(this);
}
        

}

