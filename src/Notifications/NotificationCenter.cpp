/*
 *  NotificationCenter.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 10.10.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "NotificationCenter.h"


namespace cefix {
// ---------------------------------------------------------------------------
// instance
// liefert globale Instanz des managers zurück
// ---------------------------------------------------------------------------


NotificationCenter* NotificationCenter::instance()
{
	static osg::ref_ptr<NotificationCenter> s_notificationCenter = new NotificationCenter;
	return s_notificationCenter.get();
}

}