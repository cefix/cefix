/*
 *  Serializer.cpp
 *  testSerializer
 *
 *  Created by Stephan Huber on 12.06.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Serializer.h"
#include "MathUtils.h"


namespace cefix {

Serializer::Serializer(cefix::PropertyList* pl, bool strict) 
:	_root(pl), 
	_current(pl),
	_currentItem(NULL),
	_lastKey("root"),
	_hasKey(false),
	_keyStack(),
	_listStack(),
	_itemStack(),
	_strict(strict),
	_factory(new Factory()),
	_notify(osg::INFO)
{
}

Serializer::Serializer(const std::string& file, bool strict) 
:	_root(new cefix::PropertyList(file)), 
	_current(_root),
	_currentItem(NULL),
	_lastKey("root"),
	_hasKey(false),
	_keyStack(),
	_listStack(),
	_itemStack(),
	_strict(strict),
	_factory(new Factory()),
	_notify(osg::INFO)
{
}

template<>
Serializer& Serializer::operator<<(const char* str)
{
	if ( _hasKey) {
		_current->add(_lastKey, std::string(str));
		_hasKey = false;
	} else {
		_lastKey = str;
		_hasKey = true;
	}
	
	// std::cout << getClassName(*this) << std::endl;
	// std::cout << getClassName<Serializer>() << std::endl;
	
	return *this;
}


template<> Serializer& Serializer::operator<<(std::string& str)
{
	return operator<<(str.c_str());
}

template<> Serializer& Serializer::operator<<(const std::string& str)
{
	return operator<<(str.c_str());
}

template <>
Serializer& Serializer::operator>>(const char* str)
{
	if (!_current.valid()) {
		if (_strict) {
			throw SerializerKeyNotFoundException(_lastKey);
		} else {
			osg::notify(_notify) << "Serializer :: no valid propertylist " << std::endl;
		}
		_keyFound = false;
		_hasKey = false;
		return *this;
	}
		
	_lastKey = str;
	_hasKey = true;
	_keyFound = true;
	cefix::AbstractProperty* p = _current->get(_lastKey);
	if (p->getType() == cefix::AbstractProperty::UNDEFINED) {
		_keyFound = false;
		if (_strict) {
			throw SerializerKeyNotFoundException(_lastKey);
		} else {
			osg::notify(_notify) << "Serializer :: could not find key " << _lastKey << " in " << _current->getXmlFileName() << std::endl;
			// _current->dump();
		}
	}
	pushItem(_current->get(_lastKey));
	
	return *this;
}




}