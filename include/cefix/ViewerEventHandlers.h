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

#ifndef CEFIX_VIEWER_EVENT_HANDLERS
#define CEFIX_VIEWER_EVENT_HANDLERS 1


#include <osgGA/GUIEventHandler>
#include <cefix/Export.h>


namespace cefix {

class CEFIX_EXPORT ViewerEventHandler : public osgGA::GUIEventHandler
{
	public:
		ViewerEventHandler() : osgGA::GUIEventHandler(), _showMouseCursor(true) {}
		
		virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	
	private:
		std::string getLastUsedFileIndex(const std::string& file, const std::string& ext);
		bool _showMouseCursor;

        float _x,_y;	
};

}

#endif
