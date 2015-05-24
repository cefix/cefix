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

#ifndef NOTIFICATION_CENTER_HEADER
#define NOTIFICATION_CENTER_HEADER

#include <cefix/MessageHandler.h>
namespace cefix {


class NotificationCenter : public MessageHandler {

	public:
	
		/**
		 * @return returns a reference to the NotificationBroadcaster-singleton
		*/
		static NotificationCenter* instance();
		
	protected:
		NotificationCenter() : MessageHandler() {}



};

}

#endif