/*
 *  NotificationListener.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 25.10.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "NotificationListener.h"

namespace cefix {

void NotificationListener::unsubscribeFromAll() 
{
	cefix::MessageHandler* mh = cefix::NotificationCenter::instance();
    if(mh) {
        for(SubscribedList::iterator i = _subscribed.begin(); i != _subscribed.end(); ++i)
        {
            (*i)->unsubscribeFrom(mh);
        }
    }
	_subscribed.clear();
}

}