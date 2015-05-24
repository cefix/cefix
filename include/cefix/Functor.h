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

#ifndef FUNCTOR_HEADER
#define FUNCTOR_HEADER

#include <osg/Referenced>
#include <osg/observer_ptr>

namespace cefix {

template< typename ReturnType> class FunctorAbstractT : public osg::Referenced {

	public: 
		FunctorAbstractT() : osg::Referenced() {} 
		
		virtual ReturnType operator()() = 0;
		
		virtual ReturnType call() = 0;
		
		virtual bool valid() const = 0;
	protected:
		virtual ~FunctorAbstractT() {}
		

};


template <typename ReturnType, class TClass> class FunctorT : public FunctorAbstractT<ReturnType> {
	 
	 private:
	  osg::observer_ptr<TClass> _object; 
      ReturnType (TClass::*_fpt)();   
	  
	public:
		FunctorT(TClass* object, ReturnType(TClass::*fpt)()) : 
			FunctorAbstractT<ReturnType>(),
			_object(object),
			_fpt(fpt)
		{
		}
		virtual ReturnType operator()() { return (*_object.*_fpt)(); }
		virtual ReturnType call() { return (*_object.*_fpt)(); }
		bool isEqual(TClass* object, ReturnType (TClass::*fpt)() ) {
			return ((_object == object) && ( fpt == _fpt));
		}
		
		virtual bool valid() const { return _object.valid(); }
		
		bool isSameObject(TClass* obj) { return _object == obj; }
	protected:
		virtual ~FunctorT() {}

};


template< typename ReturnType, typename Param1Type> class Functor1ParamAbstractT : public osg::Referenced {

	public: 
		Functor1ParamAbstractT() : osg::Referenced() {} 
		
		virtual ReturnType operator()(Param1Type t) = 0;
		
		virtual ReturnType call(Param1Type t) = 0;
		
		virtual bool valid() const = 0;
	
	protected:
		virtual ~Functor1ParamAbstractT() {}
		

};


template <typename ReturnType, typename Param1Type, class TClass> class Functor1ParamT : public Functor1ParamAbstractT<ReturnType, Param1Type> {
	 
	 private:
	  osg::observer_ptr<TClass> _object; 
      ReturnType (TClass::*_fpt)(Param1Type t);   
	  
	public:
		Functor1ParamT(TClass* object, ReturnType(TClass::*fpt)(Param1Type t)) : 
			Functor1ParamAbstractT<ReturnType, Param1Type>(),
			_object(object),
			_fpt(fpt)
		{
		}
		virtual ReturnType operator()(Param1Type t) { return (*_object.*_fpt)(t); }
		virtual ReturnType call(Param1Type t) { return (*_object.*_fpt)(t); }
		
		bool isEqual(TClass* object, ReturnType (TClass::*fpt)(Param1Type t) ) {
			return ((_object == object) && ( fpt == _fpt));
		}
		
		bool isSameObject(TClass* obj) { return _object == obj; }
		
		virtual bool valid() const { return _object.valid(); }
	
	protected:
		virtual ~Functor1ParamT() {}

};

template< typename ReturnType, typename Param1Type> class Functor1ParamEmbeddedAbstractT : public osg::Referenced {

	public: 
		explicit Functor1ParamEmbeddedAbstractT(Param1Type p) : osg::Referenced(), _p(p) {} 
		
		virtual ReturnType operator()(Param1Type t) = 0;
		virtual ReturnType call(Param1Type t) = 0;
		
		inline ReturnType operator()()  { return operator()(_p); }
		inline ReturnType call() { return call(_p); }
		
		virtual bool valid() const = 0;
	
	protected:
		virtual ~Functor1ParamEmbeddedAbstractT() {}
		
		Param1Type _p;
		

};


template <typename ReturnType, typename Param1Type, class TClass> class Functor1ParamEmbeddedT : public Functor1ParamEmbeddedAbstractT<ReturnType, Param1Type> {
	 
	 private:
	  osg::observer_ptr<TClass> _object; 
      ReturnType (TClass::*_fpt)(Param1Type t);   
	  
	public:
		Functor1ParamEmbeddedT(TClass* object, ReturnType(TClass::*fpt)(Param1Type t), Param1Type p) : 
			Functor1ParamEmbeddedAbstractT<ReturnType, Param1Type>(p),
			_object(object),
			_fpt(fpt)
		{
		}
		virtual ReturnType operator()(Param1Type t) { return (*_object.*_fpt)(t); }
		virtual ReturnType call(Param1Type t) { return (*_object.*_fpt)(t); }
		
		inline ReturnType operator()()  { return operator()(Functor1ParamEmbeddedAbstractT<ReturnType, Param1Type>::_p); }
		inline ReturnType call() { return call(Functor1ParamEmbeddedAbstractT<ReturnType, Param1Type>::_p); }

		virtual bool valid() const { return _object.valid(); }
		
	protected:
		virtual ~Functor1ParamEmbeddedT() {}

};

class Functor {
	
	public:
		template<class TClass, typename ReturnType> static FunctorT<ReturnType, TClass>* create(TClass* object, ReturnType(TClass::*fpt)()) {
			return new FunctorT<ReturnType, TClass>(object, fpt);
		}
		
		template<class TClass, typename ReturnType, typename Param1Type> static Functor1ParamT<ReturnType, Param1Type, TClass>* create(TClass* object, ReturnType(TClass::*fpt)(Param1Type t)) {
			return new Functor1ParamT<ReturnType, Param1Type, TClass>(object, fpt);
		}
		
		template<class TClass, typename ReturnType, typename Param1Type> static Functor1ParamT<ReturnType, Param1Type, TClass>* create(const TClass*  object, ReturnType(TClass::*fpt)(Param1Type t)) {
			return new Functor1ParamT<ReturnType, Param1Type, TClass>(object, fpt);
		}
		
		template<class TClass, typename ReturnType, typename Param1Type> static Functor1ParamT<ReturnType, Param1Type, TClass>* create(const TClass*  object, ReturnType(TClass::*fpt)(Param1Type t), Param1Type param) {
			return new Functor1ParamT<ReturnType, Param1Type, TClass>(object, fpt, param);
		}
		
			

};



}

#endif