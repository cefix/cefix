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
 
#ifndef CEFIX_LOG_HEADER
#define CEFIX_LOG_HEADER

#include <string>
#include <osg/Notify>
#include <cefix/Export.h>

namespace cefix {

class log {	
public:
	static std::ostream& error(const std::string& section) 
	{ 
		osg::notify(_errorLevel) << section << " :: "; 
		return osg::notify(_errorLevel); 
	}
	
	static std::ostream& info(const std::string& section) 
	{  
		osg::notify(_infoLevel) << section << " :: "; 
		return osg::notify(_infoLevel); 
	}
	
	static void setInfoLevel(osg::NotifySeverity s) { _infoLevel = s; }
	static void setErrorLevel(osg::NotifySeverity s) { _errorLevel = s; }
	static osg::NotifySeverity _infoLevel, _errorLevel;
private:
    log() {}
};

}

#endif

