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

#ifndef ANIMATE_SEND_NOTIFICATION_HEADER_
#define ANIMATE_SEND_NOTIFICATION_HEADER_

#include <cefix/AnimationBase.h>
#include <cefix/NotificationCenter.h>

namespace cefix {
/** 
 * this class sends a notification after a speicified period of time. It should
 * be part of an animation
 */
template<class DataType>class CEFIX_EXPORT AnimateSendNotification : public cefix::AnimationBase {

	public:		
		
		/** Constructor 
		 *  @param starttime time in sec, when the notifcation should be sent 
		 *  @param n the notification to send
		 */
		AnimateSendNotification(float starttime, const std::string& msg, const DataType& data) :
			cefix::AnimationBase(starttime),
			_msg(msg),
			_data(data)
		{
		}
			
		virtual void cleanUp() 
		{
			cefix::NotificationCenter* nc = cefix::NotificationCenter::instance();
			if (nc) nc->notify(_msg, _data);
		}
			
	protected:
		virtual ~AnimateSendNotification() {}
		
		virtual void animate(float elapsed_time) {
			setFinishedFlag();
		}
		
		std::string _msg;
		DataType	_data;
};

template<class DataType>class CEFIX_EXPORT AnimateSendNotification<DataType*> : public cefix::AnimationBase {

	public:		
		
		/** Constructor 
		 *  @param starttime time in sec, when the notifcation should be sent 
		 *  @param n the notification to send
		 */
		AnimateSendNotification(float starttime, const std::string& msg, DataType* data) :
			cefix::AnimationBase(starttime),
			_msg(msg),
			_data(data)
		{
		}
			
		virtual void cleanUp() 
		{
			cefix::NotificationCenter* nc = cefix::NotificationCenter::instance();
			if (nc) nc->notify(_msg, _data.get());
		}
			
	protected:
		virtual ~AnimateSendNotification() {}
		
		virtual void animate(float elapsed_time) {
			setFinishedFlag();
		}
		
		std::string _msg;
		osg::ref_ptr<DataType>	_data;
};

template<>class CEFIX_EXPORT AnimateSendNotification<void> : public cefix::AnimationBase {

	public:		
		
		/** Constructor 
		 *  @param starttime time in sec, when the notifcation should be sent 
		 *  @param n the notification to send
		 */
		AnimateSendNotification(float starttime, const std::string& msg) :
			cefix::AnimationBase(starttime),
			_msg(msg)
		{
		}
			
		virtual void cleanUp() 
		{
			cefix::NotificationCenter* nc = cefix::NotificationCenter::instance();
			if (nc) nc->notify(_msg);
		}
			
	protected:
		virtual ~AnimateSendNotification() {}
		
		virtual void animate(float elapsed_time) {
			setFinishedFlag();
		}
		
		std::string _msg;
};


}
#endif