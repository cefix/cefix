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

#ifndef CEFIX_HEART_BEAT_TIMER_HEADER
#define CEFIX_HEART_BEAT_TIMER_HEADER

#include <ostream>
#include <fstream>
#include <cefix/Timer.h>
#include <cefix/DataFactory.h>
#include <cefix/AnimationTimeBase.h>

class HeartBeatTimer : public cefix::Timer 
{
public:
	HeartBeatTimer() 
	:	cefix::Timer(60*1000) 
	{
		_lockFileName = cefix::DataFactory::instance()->getRootPath() + "/" + cefix::DataFactory::instance()->getApplicationName() + ".lock_file";
		osg::notify(osg::INFO) << "Installing heart beat at: " << _lockFileName << std::endl;
		write();
	}
	virtual void timer() 
	{
		write();
	}
	void write() {
		osg::notify(osg::DEBUG_INFO) << "writing heartbeat" << std::endl;
		std::ofstream f(_lockFileName.c_str(), std::ios::out | std::ios::trunc);
		f << cefix::AnimationTimeBase::instance()->getCurrentTime() << std::endl;
	}
	
	virtual ~HeartBeatTimer() 
	{
		remove(_lockFileName.c_str());
	}
private:
	std::string _lockFileName;
	
};

#endif