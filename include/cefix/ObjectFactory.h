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

#ifndef OBJECT_FACTORY_HEADER_
#define OBJECT_FACTORY_HEADER_

#include <map>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <iostream>
#include <cefix/Export.h>

/** generic object-factory-template */
namespace cefix {

template
<
	class AbstractObject,
	typename ObjectType,
	typename ProductCreator,
	typename CreateParameter = int
>
class ObjectFactory : public osg::Referenced {


	public:
		typedef ObjectType ObjectIdentType;
		typedef std::map<ObjectType, osg::ref_ptr<ProductCreator> > AssocMap;
	
		/**Ctor */
		ObjectFactory() : osg::Referenced() {}
		
		/** register a class to create for a specific type */
		bool registerObjectClass(const ObjectType& type, ProductCreator* creator) {
			return _associations.insert(typename AssocMap::value_type(type, creator)).second;
		}
		/** unregisters a class for a specific type */
		bool unregisterObjectClass(const ObjectType& type) {
			return (_associations.erase(type) == 1);
		}
		
		bool hasObjectClass(const ObjectType& type) 
		{
			return (_associations.find(type) != _associations.end());
		}
		
		bool createClonedObjectClass(const ObjectType& registeredType, const ObjectType& newType) {
			
			typename AssocMap::const_iterator i = _associations.find(registeredType);
			if (i != _associations.end()) {
				_associations.insert(std::make_pair(newType, i->second.get()));
				return true;
			}
			
			return false;
		}
			
		/** gets or create an object of a specific type */
		AbstractObject* create(const ObjectType& type) {
			typename AssocMap::const_iterator i = _associations.find(type);
			if (i != _associations.end()) {
				const ProductCreator* c = i->second.get(); 
				AbstractObject* o = (*c)();
				applyAdditionalData(o,type);
				return o;
			} else
				return NULL;
		}
		
		/** gets or create an object of a specific type */
		AbstractObject* create(const ObjectType& type, CreateParameter param) {
			
			typename AssocMap::const_iterator i = _associations.find(type);
			if (i != _associations.end()) {
				const ProductCreator* c = i->second.get();  
				AbstractObject* o = (*c)(param);
				applyAdditionalData(o,type);
				return o;
			} else
				return NULL;
		}
				
		template<typename ConcreteClass>ConcreteClass* createAndCastTo(ObjectType type) {
			AbstractObject* o = create(type);
			return dynamic_cast<ConcreteClass*>(o);
		}
		
		void dumpTypes() {
			std::cout << "registered types: ";
			for(typename AssocMap::const_iterator i = _associations.begin(); i != _associations.end(); i++) {
				std::cout << i->first << ", ";
			}
			std::cout << std::endl;
		}

		template<typename T>void foreach(T* t, void(T::*fpt)(const ObjectType& , ProductCreator*))
		{
			for(typename AssocMap::const_iterator i = _associations.begin(); i != _associations.end(); i++) {
				(*t.*fpt)(i->first, i->second.get());
			}
		}
				
	protected:
		virtual void applyAdditionalData(AbstractObject* o, ObjectType type) {};
		
		virtual ~ObjectFactory() {}
		AssocMap& getAssociations() { return _associations; }
	
	private:
	
		AssocMap	_associations;

};

}
#endif