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

#ifndef PROPERTY_LIST_CHECKER_HEADER_
#define PROPERTY_LIST_CHECKER_HEADER_

#include <cefix/PropertyList.h>
#include <cefix/Export.h>
#include <cefix/ErrorDispatcher.h>

namespace cefix {

/** this is a small helper class to check a propertyList for required keys. Example:
 * <code>
 * PropertyListValidator validator(propertyListToValidate);
 * validator.add(keyNameAsString, aValueType);
 * validator.add(anotherkeyNameAsString, aValueType);
 * .
 * .
 * .
 * std::vector<std::string> result;
 * if (validator.getMissingKeys(result) { .. do the error-reportings sstuff ... }
 * </code>
 * result holds a vector of strings describing the problems in human language, which you can output to the screen/console
 * if everything went smooth the vector has a size() of 0
 */
 
class CEFIX_EXPORT PropertyListValidator {


	public:
	/** the possible types of a property */
		enum ValueType {
			ANY,
			BOOLEAN,
			INTEGER,
			FLOAT,
			DOUBLE,
			STRING,
			SYMBOL,
			COLOR,
			POINT,
			RECT,
			VEC3,
			VEC4,
			PROPERTY_LIST
		};
		
		/** Ctor, takes a propertyList as argument */
		PropertyListValidator(cefix::PropertyList* pl) : _pl(pl) {

		}
				
		
		inline void checkForString(std::string key) { add(key, STRING); }
		inline void checkForInteger(std::string key) { add(key, INTEGER); }
		inline void checkForBoolean(std::string key) { add(key, BOOLEAN); }
		inline void checkForAny(std::string key) { add(key, ANY); }
		inline void checkForFloat(std::string key) { add(key, FLOAT); }
		inline void checkForDouble(std::string key) { add(key, DOUBLE); }
		inline void checkForSymbol(std::string key) { add(key, SYMBOL); }
		inline void checkForColor(std::string key) { add(key, COLOR); }
		inline void checkForPoint(std::string key) { add(key, POINT); }
		inline void checkForRect(std::string key) { add(key, RECT); }
		inline void checkForVector(std::string key) { add(key, VEC3); }
		inline void checkForVector4(std::string key) { add(key, VEC4); }
		inline void checkForPropertyList(std::string key) { add(key, PROPERTY_LIST); }
		typedef std::vector<std::string> ErrorVector;
		/** @return true, if an error occured (wrong type, missing key)
		 *  populates the errors-vector with hunan readable messages */
		bool getMissingKeys(ErrorVector& errors);
		
		/** report any errors via ErrorDispatcher dispatcher */
		inline bool validated(const ErrorDispatcher& dispatcher) {
			std::vector<std::string> errors;
			if (getMissingKeys(errors)) {
				dispatcher.handle("XML-error in " + _pl->getXmlFileName(), errors);
				return false;
			}
			return true;
		}
	
	
	private:
		/** adds a required field with type-information. Use ANY if you want to check only for existence of the key */
		inline void add(std::string key, ValueType type) {
			_required.push_back(std::pair<std::string, ValueType>(key, type));
		}
		std::string getValueTypeAsString(ValueType type);
		cefix::AbstractProperty::PropertyType getPropertyTypeFor(ValueType type);
		
		typedef std::vector <std::pair<std::string, ValueType> > KeyTypeVector;
		KeyTypeVector						_required;
		osg::ref_ptr<cefix::PropertyList>	_pl;
		

};

}
#endif