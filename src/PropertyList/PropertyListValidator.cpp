/*
 *  PropertyListValidator.cpp
 *  AudiParis
 *
 *  Created by Stephan Huber on 02.08.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "PropertyListValidator.h"

namespace cefix {

// --------------------------------------------------------------------------------------------------------------------
// helper-func, returns the valuetype as a string
// --------------------------------------------------------------------------------------------------------------------

std::string PropertyListValidator::getValueTypeAsString(ValueType type) {

	static std::map<ValueType, std::string> s_valuetypes_map;
	if (s_valuetypes_map.size() == 0) {
		s_valuetypes_map[ANY]				= "any type";
		s_valuetypes_map[BOOLEAN]			= "boolean";
		s_valuetypes_map[INTEGER]			= "integer";
		s_valuetypes_map[FLOAT]				= "float";
		s_valuetypes_map[DOUBLE]			= "double";
		s_valuetypes_map[STRING]			= "string";
		s_valuetypes_map[SYMBOL]			= "symbol";
		s_valuetypes_map[POINT]				= "point";
		s_valuetypes_map[COLOR]				= "color";
		s_valuetypes_map[RECT]				= "rect";
		s_valuetypes_map[VEC3]				= "vec3";
		s_valuetypes_map[VEC4]				= "vec4";
		s_valuetypes_map[PROPERTY_LIST]		= "propertylist";
	}
	
	return s_valuetypes_map[type];

}

// --------------------------------------------------------------------------------------------------------------------
// helper-func: returns the AbstractPropertyType for a given ValueType
// --------------------------------------------------------------------------------------------------------------------

cefix::AbstractProperty::PropertyType PropertyListValidator::getPropertyTypeFor(ValueType type) {

static std::map<ValueType, cefix::AbstractProperty::PropertyType> s_propertytypes_map;
	if (s_propertytypes_map.size() == 0) {
		s_propertytypes_map[ANY]				= cefix::AbstractProperty::PROPERTY_LIST;
		s_propertytypes_map[BOOLEAN]			= cefix::AbstractProperty::INTEGER;
		s_propertytypes_map[INTEGER]			= cefix::AbstractProperty::INTEGER;
		s_propertytypes_map[FLOAT]				= cefix::AbstractProperty::FLOAT;
		s_propertytypes_map[DOUBLE]				= cefix::AbstractProperty::DOUBLE;
		s_propertytypes_map[STRING]				= cefix::AbstractProperty::STRING;
		s_propertytypes_map[SYMBOL]				= cefix::AbstractProperty::STRING;
		s_propertytypes_map[POINT]				= cefix::AbstractProperty::VEC2D;
		s_propertytypes_map[COLOR]				= cefix::AbstractProperty::VEC4D;
		s_propertytypes_map[RECT]				= cefix::AbstractProperty::VEC4D;
		s_propertytypes_map[VEC3]				= cefix::AbstractProperty::VEC3D;
		s_propertytypes_map[VEC4]				= cefix::AbstractProperty::VEC4D;
		s_propertytypes_map[PROPERTY_LIST]		= cefix::AbstractProperty::PROPERTY_LIST;

	}
	
	return s_propertytypes_map[type];
}


// --------------------------------------------------------------------------------------------------------------------
// checks for any missing wrong keys and report the errors
// --------------------------------------------------------------------------------------------------------------------

bool PropertyListValidator::getMissingKeys(ErrorVector& errors) {
	bool result = false;
	if (_pl == NULL) {
		errors.push_back("no valid keys found");
		return true;
	}
	
	for(KeyTypeVector::iterator i = _required.begin(); i!= _required.end(); i++) {
		if (!_pl->hasKey(i->first)) {
			errors.push_back("key '"+i->first + "' not found! ("+_pl->getXmlFileName()+")");
			result = true;
		} else if ((i->second != ANY) && (_pl->get(i->first)->getType() != getPropertyTypeFor(i->second))) {
			errors.push_back("key '"+i->first + "' has wrong type, should be " + getValueTypeAsString(i->second) + " ("+_pl->getXmlFileName()+")");
			result = true;
		}
	}
	
	return result;
}

}