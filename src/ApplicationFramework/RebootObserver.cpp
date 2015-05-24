/*
 *  RebootObserver.cpp
 *  AudiParis
 *
 *  Created by Stephan Huber on 05.09.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */
#include <fstream>
#include <osgDB/FileNameUtils>
#include "RebootObserver.h"
#include "DataFactory.h"
#include <time.h>


#include <cefix/SysUtils.h>

namespace cefix {
// --------------------------------------------------------------------------------------------------------------------
// ctor
// --------------------------------------------------------------------------------------------------------------------

RebootObserver::RebootObserver(Action action) :
	OpenThreads::Thread(),
	osg::Referenced(),
	_rebootInProgress(false),
	_action(action)
{
	setSchedulePriority(THREAD_PRIORITY_MIN);
	
	_rebootHour = DataFactory::instance()->getPreferencesFor("/Viewer/RebootSettings/rebootHour", 23);
	_enabled	= (DataFactory::instance()->getPreferencesFor("/Viewer/RebootSettings/enabled", false));
	
	loadTimeStamp();
	
}

RebootObserver::~RebootObserver()
{
	cancel();
    join();
}


// --------------------------------------------------------------------------------------------------------------------
// getFileNameForTimeStamp
// --------------------------------------------------------------------------------------------------------------------

std::string RebootObserver::getFileNameForTimeStamp() 
{
	return osgDB::convertFileNameToNativeStyle (DataFactory::instance()->getRootPath()+"/lastreboot.txt");
}

// --------------------------------------------------------------------------------------------------------------------
// saveTimeStamp
// --------------------------------------------------------------------------------------------------------------------

void RebootObserver::saveTimeStamp() 
{
	std::string filename = getFileNameForTimeStamp();
	std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);

	file << _lastRebootDay << std::endl;
	file.close();

}


// --------------------------------------------------------------------------------------------------------------------
// loadTimeStamp
// --------------------------------------------------------------------------------------------------------------------

void RebootObserver::loadTimeStamp() 
{
	_lastRebootDay = 0;
	std::string filename = getFileNameForTimeStamp();

	std::ifstream file(filename.c_str(), std::ios::in);

	std::string line;
	std::getline(file,line);

	
	if (!line.empty()) {
		
		_lastRebootDay = atoi(line.c_str());
	}
	file.close();

}


		
		
// --------------------------------------------------------------------------------------------------------------------
// run
// --------------------------------------------------------------------------------------------------------------------

void RebootObserver::run() 
{
	bool finished = false;
	while (!finished) 
	{
		testCancel();
		
		microSleep(1000*1000*15);

		
		time_t rawtime;
		struct tm * timeinfo;

		time ( &rawtime );
		timeinfo = localtime ( &rawtime );		
		
		int currentHour = timeinfo->tm_hour;
		if (_enabled && (currentHour == _rebootHour) && (_lastRebootDay != timeinfo->tm_mday))
		{
			finished = true;
			std::cout << "initiating a reboot ..." << std::endl;
			_lastRebootDay = timeinfo->tm_mday;
			saveTimeStamp();
			dispatchAction();
			
		}
		
		else
			microSleep(1000*1000*60*10);
		
	}

}


bool RebootObserver::dispatchAction() 
{
	if (_rebootInProgress) {
		log::error("RebootObserver") << "dispatchAction already in the works" << std::endl;
		return false;
	}
		
	_rebootInProgress = true;
	if ((_cb.valid()) && (_cb->operator()(_action)))
		return true;
		
	switch (_action) {
		case SLEEP:	
			return cefix::SysUtils::sleep(); 
			break;
		case SHUTDOWN:
			return cefix::SysUtils::shutdown();
			break;
		default:
			return cefix::SysUtils::reboot();
			break;
	}
	
	return false;
};


}
