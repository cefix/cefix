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

#ifndef CEFIX_SOUND_MANAGER_HEADER
#define CEFIX_SOUND_MANAGER_HEADER

#include "Sound.h"

#include <osg/Referenced>
#include <osg/Notify>


namespace cefix {



class SoundManager : public osg::Referenced {

public:
    /// base-class for creating a sound-implementation-instance
    class SoundImplementationFactory : public osg::Referenced {
    public:
        virtual Sound::Implementation* createImplementation() { return NULL; }
        virtual ~SoundImplementationFactory() {}
    };

    /// templated class for creating a sound-implementation-class
    template <class T> 
    class SoundImplementationFactoryT : public SoundImplementationFactory {
    public:
       virtual Sound::Implementation* createImplementation() { return new T(); } 
    };

    /// read sound file from string, streaming not supported by all implementations
    static inline Sound* readSoundFile(const std::string& filename, bool streaming = false) 
    {
        return readSoundFile(cefix::Sound::Source(filename, streaming));
    }
    
    /// read sound from source 
    static Sound* readSoundFile(const cefix::Sound::Source& source);

    
    /// set notify level for the SoundManager
    static void setNotifyLevel(osg::NotifySeverity);
    
    /// get the notify level for the sound manager
    static osg::NotifySeverity getNotifyLevel();
    
    
    /// set the sound-implementation to a specific class
    template<class T> static void setImplementation() { setImplementation(new SoundImplementationFactoryT<T>()); }
    
    /// set the sound-implementation to a specific class
    static void setImplementation(SoundImplementationFactory* implfac);

private:
    SoundManager() : osg::Referenced() {}
};

}

#endif
