/*
 *  SequenceGrabberManager.cpp
 *  MoviePlayer
 *
 *  Created by Stephan Huber on 30.01.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/SequenceGrabberManager.h>
#ifndef CEFIX_FOR_IPHONE
#if defined(CEFIX_QUICKTIME_AVAILABLE)
    #include <cefix/QuicktimeSequenceGrabber.h>
#endif
#endif
#include <osg/ref_ptr>
#include <cefix/Log.h>
#include <cefix/StringUtils.h>


namespace cefix {



 SequenceGrabberManager* SequenceGrabberManager::instance() {
	
	static osg::ref_ptr<SequenceGrabberManager> s_sqm = new SequenceGrabberManager();
	return s_sqm.get();
	
}

SequenceGrabberManager::SequenceGrabberManager() 
:	osg::Referenced(),
	_lastAcquiredDevice(NULL)
{	
    _factory = new SequenceGrabberFactory();
    
    #ifndef CEFIX_FOR_IPHONE
        
        #ifdef __LP64__
            
        #else
            #ifdef CEFIX_QUICKTIME_AVAILABLE
                setDefaultGrabberName(cefix::QuicktimeSequenceGrabber::getGrabberId());
                registerClass<QuicktimeSequenceGrabber>();
            #endif
        #endif
            
    #endif
}





SequenceGrabber* SequenceGrabberManager::get(std::string key, unsigned int w, unsigned int h, unsigned int r) 
{

	SequenceGrabberMap::iterator itr = _map.find(key);

	
	if (itr != _map.end()) {
		_lastAcquiredDevice = itr->second.get();
		return itr->second.get();
	} 
	else 
	{
		// _factory->dumpTypes();
		osg::ref_ptr<SequenceGrabber> s = NULL;
		
		if (_factory->hasObjectClass(key))
			s = _factory->create(key, SequenceGrabberCreateData("", w, h, r));
		else 
		{
			std::vector<std::string> strs;
			cefix::strTokenize(key, strs, "/");
			if (strs.size() == 2) 
			{
				s = _factory->create(strs[0], SequenceGrabberCreateData(strs[1], w,h, r));
			}
		}
		if (!s.valid()) 
		{
			log::error("SequenceGrabberManager::get") << "could not find any sequencegrabber for key " << key << ", returning first found grabber" << std::endl;
			const SequenceGrabberDeviceInfoList& l = getDeviceInfoList();
			for (SequenceGrabberDeviceInfoList::const_iterator i = l.begin(); i != l.end(); ++i) 
			{
				if(getDefaultGrabberName() == (*i).getGrabberId()) 
				{
					s = _factory->create((*i).getGrabberId(), SequenceGrabberCreateData((*i).getId(), w, h, r));
					key = (*i).getKey();
					
					break;
				}
			}
		}
		
		if (s.valid()) {
			_map.insert(std::make_pair(key, s.get()));
			s->setIdentifier(key);
		}
		_lastAcquiredDevice = s.get();
		return s.get();
	}
}



void SequenceGrabberManager::populateDeviceInfoList() 
{
    _devices.clear();
	for(PopulateDeviceListCallbackMap::iterator i = _deviceListCallbackMap.begin(); i != _deviceListCallbackMap.end(); ++i) {
		osg::notify(osg::INFO) << "populating devices from grabber " << i->first << std::endl;
		(*i->second)(_devices);
	}
}

}    
        
std::ostream& operator<<(std::ostream& os, const cefix::SequenceGrabberDeviceInfoList& device_list) 
{
    for(cefix::SequenceGrabberDeviceInfoList::const_iterator i = device_list.begin(); i != device_list.end(); ++i) {
        os << std::setw(20) << (*i).getGrabberId() << ": " << (*i).getId() << std::endl;
    }
    
    return os;
}

