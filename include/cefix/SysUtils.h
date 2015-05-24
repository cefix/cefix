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


#ifndef SYS_UTILS_HEADER
#define SYS_UTILS_HEADER

#include <osg/GraphicsContext>
#include <cefix/Export.h>

namespace cefix {

class SysUtils {

	public:
		/// initiate a reboot (mac/win only)
		static bool reboot();
		/// initiate a shutdown (mac/win only)
		static bool shutdown();
		/// go to sleep (mac only)
		static bool sleep();
		
		/// get some system memory stats like free and used memory (mac/ios only)
		static bool getSystemMemoryUsage(unsigned long &free_mem, unsigned long &used_mem, unsigned long &mem_size);
		
		/// get used memory by the current task/application (mac/ios only)
		static bool getTaskMemoryUsage(unsigned long &used_virtual_mem, unsigned long &used_resident_mem);

        static bool isWindows();
        static bool isMac();
        static bool isIOS();
        static bool isIphone();
        static bool isIpad();
        static float getUIScale(const osg::GraphicsContext::ScreenIdentifier& si = 0);
		
	private:
		SysUtils() {}
};

};

#endif