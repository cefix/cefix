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

#ifndef CEFIX_ENUM_UTILS_HEADER
#define CEFIX_ENUM_UTILS_HEADER

#include <map>
#include <string>
namespace cefix {

struct __lesscasecmp
{
   bool operator() (const char* a, const char* b) const
   {
#ifdef WIN32
	   return (strcmpi(a,b) < 0);
#else
	return (strcasecmp (a, b) < 0);
#endif
   }
};
/** EnumAsString helps to serialize/deserialize enums to/from strings
 *  here's an example:
 *  static MyEnum[3] my_enum = { MY_ENUM_1, MY_ENUM_2, MY_ENUM_3};
 *  static std::string[3] my_enum_str = {"my_enum_1", "my_enum_2", "my_enum_3"};
 * 
 *  EnumAsString<MyEnum, 3> my_enums(my_enum, my_enum_str);
 *  std::cout << my_enums.string(MY_ENUM_1) << std::endl;
 *  std::cout << my_enums.value("my_enum_1") << std::endl;
 */
template <typename T, int NUM>
class EnumAsString {
public:
	EnumAsString(T t[], const char* s[])
	{
		for(unsigned int i = 0; i < NUM; ++i) {
			_toString.insert(std::make_pair(t[i],s[i]));
			_toValue.insert(std::make_pair(s[i],t[i]));
		}
	}
	/** get the string representation for a specific enum */
	const char* string(T t) { return _toString.find(t) != _toString.end() ? _toString[t] : "UNKNOWN"; }
	
	/** get the enum from a string representation, if the string is not found, return the default value*/
	T value(const char* s, T default_value) { return _toValue.find(s) != _toValue.end() ? _toValue[s] : default_value; }
	/** get the enum from a string representation, if the string is not found, return the default value*/
	T value(const std::string& s, T default_value) { return _toValue.find(s.c_str()) != _toValue.end() ? _toValue[s.c_str()] : default_value; }
	
private:
	typename std::map<T, const char*> _toString;
	typename std::map<const char*, T, __lesscasecmp> _toValue;
};



}

#endif