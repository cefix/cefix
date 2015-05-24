/*
 *  ErrorDispatcher.cpp
 *  AudiParis
 *
 *  Created by Stephan Huber on 07.08.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "ErrorDispatcher.h"
#include <cefix/Notify.h>
#include <cefix/AnimationFactory.h>
#include <cefix/AnimateSendNotification.h>

namespace cefix {

// --------------------------------------------------------------------------------------------------------------------
// handle an error-message
// --------------------------------------------------------------------------------------------------------------------

void ErrorDispatcher::handle(std::string msg) const {
	std::string err = (_identifier.empty()) ? msg : _identifier + " // " + msg;
	if (_mode == THREAD_SAFE)
		cefix::notifyDeferred("addToErrorLog",err);
	else
		cefix::notify("addToErrorLog",err);
}



}