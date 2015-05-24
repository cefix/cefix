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

#ifndef CEFIX_NOTIFY_HEADER
#define CEFIX_NOTIFY_HEADER

#include <cefix/NotificationCenter.h>
#include <cefix/AnimationFactory.h>
#include <cefix/AnimateSendNotification.h>

namespace cefix {


/// sends a notification 'message' to all subscribed objects
template <class T>inline  bool notify(const std::string& message, const T& data) {
    if (cefix::NotificationCenter* nc = cefix::NotificationCenter::instance()) return nc->notify(message, data);
	return false;
}

/// sends a notification 'message' to all subscribed objects
template <class T> inline bool notify(const std::string& message, T* data) {
    if (cefix::NotificationCenter* nc = cefix::NotificationCenter::instance()) return nc->notify(message, data);
	return false;
}

/// sends a notification 'message' to all subscribed objects
inline bool notify(const std::string& message) {
    if (cefix::NotificationCenter* nc = cefix::NotificationCenter::instance()) return nc->notify(message);
	return false;
}

/// sends a notification 'message' deferred to all subscribed objects. the notification is sent from the main-thread
template <class T> inline void notifyDeferred(const std::string& message, const T& data) {
    cefix::AnimationFactory::instance()->add(new AnimateSendNotification<T>(0, message, data));
}

/// sends a notification 'message' deferred to all subscribed objects. the notification is sent from the main-thread
template <class T> inline  void notifyDeferred(const std::string& message, T* data) {
    cefix::AnimationFactory::instance()->add(new AnimateSendNotification<T*>(0, message, data));
}

/// sends a notification 'message' deferred to all subscribed objects. the notification is sent from the main-thread
inline void notifyDeferred(const std::string& message) {
    cefix::AnimationFactory::instance()->add(new AnimateSendNotification<void>(0, message));
}

}
#endif
