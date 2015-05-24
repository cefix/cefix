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

#ifndef MESSAGE_HANDLER_HEADER
#define MESSAGE_HANDLER_HEADER

#include <cefix/Functor.h>
#include <cefix/StringUtils.h>
#include <list>
#include <map>
#include <osg/ref_ptr>
#include <cefix/Log.h>

namespace cefix {

namespace priv {

template <class T> class Message {

	public:
		Message(const std::string& type, const T& t) : _type(cefix::strToLower(type)), _t(t) {}
		
		const std::string& getType() const { return _type; }
		
		const T& getData() const { return _t; }
		
	private:
		std::string	_type;
		T			_t;
	
};

template <class T> class Message<T*> {

	public:
		Message(const std::string& type, T* t) : _type(cefix::strToLower(type)), _t(t) {}
		
		const std::string& getType() const { return _type; }
		
		T* getData() const { return _t; }
		
		
	private:
		std::string	_type;
		T*	_t;
	
};



template <> class Message<void> {

	public:
		Message(const std::string& type) : _type(cefix::strToLower(type)) {}
		
		const std::string& getType() const { return _type; }
		
		
	private:
		std::string	_type;
	
};

/** base class for a class encapsulating all handlers specific to a message type */
struct MessageHandlerListBase : public osg::Referenced {

};

/** template class which combines a datatype with a list of handlers */
template<class DataType> class MessageHandlerList : public MessageHandlerListBase {
	
	public:
		/** definition of a handler */
		typedef cefix::Functor1ParamAbstractT<void, const DataType& > Handler;
		
		/** definition of a handler-list */
		typedef std::list< osg::ref_ptr< Handler > > HandlerList;
		
		/** adds a handler to the list */
		void add(Handler* handler) { _handlers.push_back(handler); }
		
		/** sets a message-object to get the data from for the subscribed handlers */
		void setMessage(const Message<DataType>& message) { _message = &message; }
		
		/** call all handlers of this list */
		void operator()() 
		{
			for(typename HandlerList::iterator i = _handlers.begin(); i != _handlers.end(); ++i) {
				(*i)->call(_message->getData());
			}
		}
		
		template <typename TClass>void remove(TClass* obj, void(TClass::*fpt)( const DataType& t)) {
		
			for(typename HandlerList::iterator i = _handlers.begin(); i != _handlers.end(); ) 
			{
				cefix::Functor1ParamT<void, const DataType &, TClass>* f = dynamic_cast< cefix::Functor1ParamT<void, const DataType&, TClass>* >((*i).get());
					if (f && f->isEqual(obj, fpt)) {
						i = _handlers.erase(i);
					}
					else
						++i;
			}
			
		}
		
	private:
		const Message<DataType>*	_message;
		HandlerList			_handlers;
};


/** template class which combines a datatype with a list of handlers */
template<class DataType> class MessageHandlerList<DataType*> : public MessageHandlerListBase {
	
	public:
		/** definition of a handler */
		typedef cefix::Functor1ParamAbstractT<void, DataType* > Handler;
		
		/** definition of a handler-list */
		typedef std::list< osg::ref_ptr< Handler > > HandlerList;
		
		/** adds a handler to the list */
		void add(Handler* handler) { _handlers.push_back(handler); }
		
		/** sets a message-object to get the data from for the subscribed handlers */
		void setMessage(const Message<DataType*>& message) { _message = &message; }
		
		/** call all handlers of this list */
		void operator()() 
		{
			for(typename HandlerList::iterator i = _handlers.begin(); i != _handlers.end(); ++i) {
				(*i)->call(_message->getData());
			}
		}
		
		template <typename TClass>void remove(TClass* obj, void(TClass::*fpt)(DataType* t)) {
		
			for(typename HandlerList::iterator i = _handlers.begin(); i != _handlers.end(); ) 
			{
				cefix::Functor1ParamT<void, DataType*, TClass>* f = dynamic_cast< cefix::Functor1ParamT<void, DataType*, TClass>* >((*i).get());
					if (f && f->isEqual(obj, fpt)) {
						i = _handlers.erase(i);
					}
					else
						++i;
			}
			
		}
		
	private:
		const Message<DataType*>*	_message;
		HandlerList			_handlers;
};


template<> class MessageHandlerList<void> : public MessageHandlerListBase {
			
	public:
		/** definition of a handler */
		typedef cefix::FunctorAbstractT<void> Handler;
		/** definition of a handler-list */
		typedef std::list< osg::ref_ptr< Handler > > HandlerList;
		
		/** adds a handler to the list */
		void add(Handler* handler) { _handlers.push_back(handler); }
		
		/** sets a message-object to get the data from for the subscribed handlers */
		void setMessage(const Message<void>& message) {}

		
		/** call all handlers of this list */
		void operator()() 
		{
			for(HandlerList::iterator i = _handlers.begin(); i != _handlers.end(); ++i) {
				(*i)->call();
			}
		}
		
		
		/** remove a handler from this list */
		template <typename TClass, typename DataType>void remove(TClass* obj, void(TClass::*fpt)()) 
		{
			for(typename HandlerList::iterator i = _handlers.begin(); i != _handlers.end(); ) 
			{
				cefix::FunctorT<void, TClass>* f = dynamic_cast< cefix::FunctorT<void, TClass>* >((*i).get());
					if (f && f->isEqual(obj, fpt)) {
						i = _handlers.erase(i);
					}
					else
						++i;
			}
			
		}
		
		/** remove a handler from this list */
		template <typename TClass>void remove(TClass* obj, void(TClass::*fpt)()) {
		
			for(typename HandlerList::iterator i = _handlers.begin(); i != _handlers.end(); ) 
			{
				cefix::FunctorT<void, TClass>* f = dynamic_cast< cefix::FunctorT<void, TClass>* >((*i).get());
					if (f && f->isEqual(obj, fpt)) {
						i = _handlers.erase(i);
					}
					else
						++i;
			}
			
		}

		
	private:
		HandlerList			_handlers;
};


}

/** The messageHandler class provides as subscription pattern, objects can register handler with this class, 
 *  and call all subscribed objects via notify. */
class MessageHandler : public osg::Referenced {
	public:
		/** ctor */
		MessageHandler() : osg::Referenced() {}
		
		/** subscribe an object/handler pair with a specific message type. Please note: avoid multiple handlers wit differen types */
		template <typename T, typename DataType>void subscribe(const std::string& messageType, T* obj, void(T::*fpt)( const DataType& t) ) 
		{
			osg::notify(osg::DEBUG_INFO) << "MessageHandler::subscribe " << messageType << " " << typeid(fpt).name() << std::endl;
			
			MessageHandlerMap::iterator itr = _handler.find(cefix::strToLower(messageType));
			if (itr == _handler.end())
				_handler.insert(std::make_pair(cefix::strToLower(messageType), new priv::MessageHandlerList<DataType>()));
			
			priv::MessageHandlerList<DataType>* mhl = dynamic_cast< priv::MessageHandlerList<DataType>* >(_handler[cefix::strToLower(messageType)].get());
			if (mhl)
				mhl->add(cefix::Functor::create(obj, fpt));
			else 
			{
				log::error("MessageHandler") << "could not subscribe to '" << messageType << "', mismatched type: " << typeid(mhl).name() << std::endl;
			}
		}
		
		
		template <typename T, typename DataType>void subscribe(const std::string& messageType, T* obj, void(T::*fpt)(DataType* t) ) 
		{
			osg::notify(osg::DEBUG_INFO) << "MessageHandler::subscribe " << messageType << " " << typeid(fpt).name() << std::endl;
			
			if (_handler.find(cefix::strToLower(messageType)) == _handler.end())
				_handler.insert(std::make_pair(cefix::strToLower(messageType), new priv::MessageHandlerList<DataType*>()));
				
			priv::MessageHandlerList<DataType*>* mhl = dynamic_cast< priv::MessageHandlerList<DataType*>* >(_handler[cefix::strToLower(messageType)].get());
			if (mhl)
				mhl->add(cefix::Functor::create(obj, fpt));
			else 
			{
				log::error("MessageHandler") << "could not subscribe to '" << messageType << "', mismatched type: " << typeid(mhl).name() << std::endl;
			}
		}
		

		/** subscribe an object/handler pair with a specific message type. Please note: avoid multiple handlers wit differen types */
		template <typename T>void subscribe(const std::string& messageType, T* obj, void(T::*fpt)() ) 
		{
			osg::notify(osg::DEBUG_INFO) << "MessageHandler::subscribe " << messageType << " " << typeid(fpt).name() << std::endl;
			
			if (_handler.find(cefix::strToLower(messageType)) == _handler.end())
				_handler.insert(std::make_pair(cefix::strToLower(messageType), new priv::MessageHandlerList<void>()));
				
			priv::MessageHandlerList<void>* mhl = dynamic_cast< priv::MessageHandlerList<void>* >(_handler[cefix::strToLower(messageType)].get());
			if (mhl)
				mhl->add(cefix::Functor::create(obj, fpt));
			else 
			{
				log::error("MessageHandler") << "could not subscribe to '" << messageType << "', mismatched type: " << typeid(mhl).name() << std::endl;
			}
		}
		
		
		
		/** unsubscribe an object from a message-type */
		template<typename T, typename DataType>void unsubscribe(const std::string& messageType, T* obj, void(T::*fpt)( const DataType& t)) 
		{
			MessageHandlerMap::iterator i = _handler.find(cefix::strToLower(messageType));
			if (i != _handler.end())		
				dynamic_cast< priv::MessageHandlerList<DataType>* >(_handler[cefix::strToLower(messageType)].get())->remove(obj, fpt);
		}
		
		
		
		/** unsubscribe an object from a message-type */
		template<typename T, typename DataType>void unsubscribe(const std::string& messageType, T* obj, void(T::*fpt)(DataType* t)) 
		{
			MessageHandlerMap::iterator i = _handler.find(cefix::strToLower(messageType));
			if (i != _handler.end())		
				dynamic_cast< priv::MessageHandlerList<DataType*>* >(_handler[cefix::strToLower(messageType)].get())->remove(obj, fpt);
		}
		
		
		
		/** unsubscribe an object from a message-type */
		template<typename T>void unsubscribe(const std::string& messageType, T* obj, void(T::*fpt)()) 
		{
			MessageHandlerMap::iterator i = _handler.find(cefix::strToLower(messageType));
			if (i != _handler.end()) {	
				priv::MessageHandlerList<void>* mhl = dynamic_cast< priv::MessageHandlerList<void>* >(_handler[cefix::strToLower(messageType)].get());
				if (mhl) mhl->remove(obj, fpt);
			}
		}
		
				
		template <class T, class DataType>void unsubscribeHandler(T* obj, void(T::*fpt)( const DataType& t)) 
		{
			for(MessageHandlerMap::iterator i = _handler.begin(); i!=_handler.end(); ++i)
			{
				priv::MessageHandlerList<DataType>* mhl = dynamic_cast< priv::MessageHandlerList<DataType>* >(i->second.get());
				if (mhl) {
					mhl->remove(obj, fpt);
				}
			}
		}
		
		template <class T, class DataType>void unsubscribeHandler(T* obj, void(T::*fpt)(DataType* t)) 
		{
			for(MessageHandlerMap::iterator i = _handler.begin(); i!=_handler.end(); ++i)
			{
				priv::MessageHandlerList<DataType*>* mhl = dynamic_cast< priv::MessageHandlerList<DataType*>* >(i->second.get());
				if (mhl) {
					mhl->remove(obj, fpt);
				}
			}
		}
		
		template <class T>void unsubscribeHandler(T* obj, void(T::*fpt)()) 
		{
			for(MessageHandlerMap::iterator i = _handler.begin(); i!=_handler.end(); ++i)
			{
				priv::MessageHandlerList<void>* mhl = dynamic_cast< priv::MessageHandlerList<void>* >(i->second.get());
				if (mhl) {
					mhl->remove(obj, fpt);
				}
			}
		}
		
		/** notify all registeres object of a specific message-type */
		template <class DataType>bool notify(const std::string& messageType, const DataType& data) 
		{
			return dispatchMessage(priv::Message<DataType>(messageType, data));
		}
		
		/** notify all registeres object of a specific message-type */
		template <class DataType>bool notify(const std::string& messageType, DataType* data) 
		{
			return dispatchMessage(priv::Message<DataType*>(messageType, data));
		}
		
		bool notify(const std::string& messageType) 
		{
			return dispatchMessage(priv::Message<void>(messageType));
		}
		
		
	protected:
		
		/** dispatch the message and do the notification */
		template <class DataType> bool dispatchMessage(const priv::Message<DataType>& message) {
			MessageHandlerMap::iterator itr = _handler.find(message.getType());
			if (itr == _handler.end()) {
				osg::notify(osg::INFO) << "MessageHandler :: no handlers registered for message " << message.getType() << std::endl;
				return true;
			}
			priv::MessageHandlerList<DataType>* mhl = dynamic_cast< priv::MessageHandlerList<DataType>* >(itr->second.get());
			if (mhl) {
				mhl->setMessage(message);
				(*mhl)();
				return true;
			} else {
				log::error("MessageHandler") << "could not dispatch message '" << message.getType() << "'; possible wrong type" << std::endl;
			}
			return false;
		}
		
	private:
		
		typedef std::map<std::string, osg::ref_ptr<priv::MessageHandlerListBase> > MessageHandlerMap;
		MessageHandlerMap			_handler;

};




} 
#endif