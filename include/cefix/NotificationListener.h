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

#ifndef NOTIFICATION_LISTENER_HEADER
#define NOTIFICATION_LISTENER_HEADER

#include <cefix/NotificationCenter.h>

namespace cefix {

namespace priv {

struct Subscribed : public osg::Referenced  {
	virtual void unsubscribeFrom(MessageHandler* mh)  = 0;
};

template<class T, class DataType> class SubscribedT : public Subscribed {

	public:
		SubscribedT(T* obj, void(T::*fpt)( const DataType& t)) : _obj(obj), _fpt(fpt) {}
		SubscribedT();
	
		virtual void unsubscribeFrom(MessageHandler* mh) {
			mh->unsubscribeHandler(_obj, _fpt);
		}
	
	protected:
		T* _obj;
		void(T::*_fpt)( const DataType& t);
	
	

}; 

template<class T> class SubscribedT<T, void> : public Subscribed {

	public:
		SubscribedT(T* obj, void(T::*fpt)()) : _obj(obj), _fpt(fpt) {}
		SubscribedT();
	
		virtual void unsubscribeFrom(MessageHandler* mh) {
			mh->unsubscribeHandler(_obj, _fpt);
		}
	
	protected:
		T* _obj;
		void(T::*_fpt)();
	
	

};

template<class T, class DataType> class SubscribedT<T, DataType*> : public Subscribed {

	public:
		SubscribedT(T* obj, void(T::*fpt)(DataType* t)) : _obj(obj), _fpt(fpt) {}
		SubscribedT();
	
		virtual void unsubscribeFrom(MessageHandler* mh) {
			mh->unsubscribeHandler(_obj, _fpt);
		}
	
	protected:
		T* _obj;
		void(T::*_fpt)(DataType* t);
	
	

}; 



}

class NotificationListener {

	public:
	
			
		typedef std::list< osg::ref_ptr<priv::Subscribed> > SubscribedList;
		
		template <typename T, typename DataType>void subscribeTo(const std::string& messageType, T* obj, void(T::*fpt)( const DataType& t) ) 
		{
			cefix::NotificationCenter::instance()->subscribe(messageType, obj, fpt);
			_subscribed.push_back(new priv::SubscribedT<T, DataType>(obj, fpt));
		}
		
		template <typename T, typename DataType>void subscribeTo(const std::string& messageType, T* obj, void(T::*fpt)(DataType* t) ) 
		{
			cefix::NotificationCenter::instance()->subscribe(messageType, obj, fpt);
			_subscribed.push_back(new priv::SubscribedT<T, DataType*>(obj, fpt));
		}
		
		
		template <typename T>void subscribeTo(const std::string& messageType, T* obj, void(T::*fpt)() ) {
			cefix::NotificationCenter::instance()->subscribe(messageType, obj, fpt);
			_subscribed.push_back(new priv::SubscribedT<T, void>(obj, fpt));
		}
		
		void unsubscribeFromAll();
		
		virtual ~NotificationListener() {
			unsubscribeFromAll();
		}
		
	private:
		SubscribedList _subscribed;
		
};


}


#endif