/*
 *  SoundManager.cpp
 *  render_tests
 *
 *  Created by Stephan Huber on 13.10.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */
#include <cefix/Export.h>

#include "SoundManager.h"
#include <iostream>
#ifdef CEFIX_FOR_IPHONE
#include "OpenALSoundImplementation.h"
#else
#include "QuicktimeSoundImplementation.h"
#endif

#include <cefix/Log.h>
#include <cefix/Version.h>


namespace cefix {

static osg::NotifySeverity s_notify_level = osg::INFO;
static osg::ref_ptr<SoundManager::SoundImplementationFactory> s_factory = NULL;


void SoundManager::setImplementation(SoundImplementationFactory* implfac)
{
    s_factory = implfac;
}



cefix::Sound* SoundManager::readSoundFile(const cefix::Sound::Source& source)
{
	if(!s_factory.valid()) {
        #ifdef CEFIX_FOR_IPHONE
            setImplementation<OpenALSoundImplementation>();
        #endif
        
        #ifdef CEFIX_QUICKTIME_AVAILABLE
            setImplementation<QuicktimeSoundImplementation>();
        #endif
    }
    
    osg::ref_ptr<cefix::Sound::Implementation> impl = s_factory.valid() ? s_factory->createImplementation() : NULL;
	
    if (impl.valid()) {
    
        cefix::Sound* sound = new cefix::Sound(impl, source);
        sound->open();
		
        return sound;
    } else {
        cefix::log::info("SoundManager") << "could not create SoundImplementation for " << cefixGetVersionDetailed() << std::endl;
    }
    
    return NULL;
}


void SoundManager::setNotifyLevel(osg::NotifySeverity s)
{
	s_notify_level = s;
}

osg::NotifySeverity SoundManager::getNotifyLevel() {
	return s_notify_level;
}

}