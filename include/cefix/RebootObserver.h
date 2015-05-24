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

#ifndef REBOOT_OBSERVER_HEADER
#define REBOOT_OBSERVER_HEADER

#include <string>
#include <OpenThreads/Thread>
#include <osg/Referenced>
#include <osg/Timer>
#include <cefix/Log.h>
#include <cefix/Export.h>
#include <osg/ref_ptr>


namespace cefix {

/**
 * this small thread-class will reboot the computer at a given time
 */
 
class CEFIX_EXPORT RebootObserver : public OpenThreads::Thread, public osg::Referenced {

	public:
		enum Action { REBOOT, SHUTDOWN, SLEEP };
		
		/** callback, it's method gets called, before any reboot-action is dispatched, so you'll have a chance to handle the action by yourself.
			return true, if you handled the action, if you return false, the default dispatch get executed.
			Be aware that this callback is called from a thread which is not the main-thread.
		*/
		class DispatchActionCallback : public osg::Referenced {
		
		public:
			virtual bool operator()(Action action) = 0;
		};
		
		
		RebootObserver(Action action = REBOOT);
		
		void setAction(Action action) { _action = action; }
		Action getAction() const { return _action; }
		
		void setRebootHour(int h) { _rebootHour = h; }
		int getRebootHour() const { return _rebootHour; }
		bool isEnabled() {return _enabled; }
		void setEnabled(bool f) { _enabled = f; }
		
		virtual void run();
		
		void start() {
			if (_enabled) {
				osg::notify(osg::NOTICE) << "Installing reboot observer, computer will reboot daily at " << _rebootHour << "h" << std::endl;
				OpenThreads::Thread::start();
			}
		}
		
		void setDispatchActionCallback(DispatchActionCallback* cb) { _cb = cb; }
		DispatchActionCallback* getDispatchActionCallback() const { return _cb.get(); }

		virtual ~RebootObserver();
		
	private:
		bool dispatchAction();
		

		
		int				_lastRebootDay;
		int				_rebootHour;
		bool			_enabled;
		bool			_rebootInProgress;
		Action			_action;
		osg::ref_ptr<DispatchActionCallback> _cb;
		
		std::string getFileNameForTimeStamp();
		void saveTimeStamp();
		void loadTimeStamp();

};

}

#endif