/* JSON based on SimpleJSON Library from ttp://mjpa.in/json modified to 
 * work with cefix's PropertyList
 */
 
/*
 * File JSON.cpp part of the SimpleJSON Library - http://mjpa.in/json
 * 
 * Copyright (C) 2010 Mike Anchor
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <cefix/JSON.h>
#include <cefix/StringUtils.h>

// Linux compile fix - from quaker66
#ifndef WIN32
	#include <cstring> 
	#include <cstdlib>
#endif

// Mac compile fixes - from quaker66
#if (defined(__APPLE__) && (__DARWIN_C_LEVEL < 200809L)) || (defined(WIN32) && defined(__GNUC__))
	#include <wctype.h>
	#include <wchar.h>

	static inline int wcsncasecmp(const wchar_t *s1, const wchar_t *s2, size_t n)
	{
		int lc1  = 0;
		int lc2  = 0;

		while (n--)
		{
			lc1 = towlower (*s1);
			lc2 = towlower (*s2);

			if (lc1 != lc2)
				return (lc1 - lc2);

			if (!lc1)
				return 0;

			++s1;
			++s2;
		}

		return 0;
	}
#endif


// Simple function to check a string 's' has at least 'n' characters
static inline bool simplejson_wcsnlen(const wchar_t *s, size_t n) {
	if (s == 0)
		return false;

	const wchar_t *save = s;
	while (n-- > 0)
	{
		if (*(save++) == 0) return false;
	}

	return true;
}

namespace cefix {


static std::string s_type_key = "type";

const std::string& JSON::getTypeKey() 
{ 
	return s_type_key; 
}

void setTypeKey(const std::string& key)
{
	s_type_key = key;
}


/** 
 * Blocks off the public constructor
 *
 * @access private
 *
 */
JSON::JSON()
{
}

/**
 * Parses a complete JSON encoded string
 * This is just a wrapper around the UNICODE Parse().
 *
 * @access public
 *
 * @param char* data The JSON text
 *
 * @return JSONValue* Returns a JSON Value representing the root, or NULL on error
 */
AbstractProperty *JSON::parse(const std::string& data, const ParseOptions& options)
{
	std::wstring wstr = cefix::Unicode::UTF8toWstring(data);
	
	AbstractProperty *value = JSON::parse(wstr.c_str(), options);
	return value;
}

/**
 * Parses a complete JSON encoded string (UNICODE input version)
 *
 * @access public
 *
 * @param wchar_t* data The JSON text
 *
 * @return JSONValue* Returns a JSON Value representing the root, or NULL on error
 */
AbstractProperty *JSON::parse(const wchar_t *data, const ParseOptions& options)
{
	// Skip any preceding whitespace, end of data = no JSON = fail
	if (!skipWhitespace(&data))
		return NULL;

	// We need the start of a value here now...
	AbstractProperty *value = parseValue(&data, options);
	if (value == NULL)
		return NULL;
	
	// Can be white space now and should be at the end of the string then...
	if (skipWhitespace(&data))
	{
		delete value;
		return NULL;
	}
	
	// We're now at the end of the string
	return value;
}

/**
 * Turns the passed in JSONValue into a JSON encode string
 *
 * @access public
 *
 * @param JSONValue* value The root value
 *
 * @return std::wstring Returns a JSON encoded string representation of the given value
 */
std::string JSON::stringifyToUTF8(const std::string& key, AbstractProperty *value, unsigned int depth)
{
	if (value == NULL)
		return "";
	std::string type_str = value->getXmlType();
	
	std::ostringstream ss;
	if (depth)
		ss << std::setw(depth * 2) << " ";
		
	if (!key.empty())
		ss << stringifyString(key) << ": ";
	
	switch (value->getType()) {

		case AbstractProperty::UNDEFINED:	
			ss << "null";
			break;
				
		case AbstractProperty::INTEGER:
			if (type_str == "boolean") {
				ss << ((value->asInt() != 0) ? "true" : "false");
			} else {
				ss << value->asString();
			}
			break;
			
		case AbstractProperty::FLOAT:
		case AbstractProperty::DOUBLE:
			ss << value->asString();
			break;
			
		case AbstractProperty::STRING:
			ss << stringifyString(value->asString());
			break;
			
		case AbstractProperty::VEC2F:
		case AbstractProperty::VEC2D:
			{
				osg::Vec2d v = value->asVec2d();
				if (type_str == "size") 
					ss << "{ \"" << s_type_key << "\": \"" << type_str << "\", \"width\": " << v[0] << ", \"height\": " << v[1] << " } ";
				else
					ss << "{ \"" << s_type_key << "\": \"" << type_str << "\", \"x\": " << v[0] << ", \"y\": " << v[1] << " } ";
			}
			break;
			
		case AbstractProperty::VEC3F:
		case AbstractProperty::VEC3D:
			{
				osg::Vec3d v = value->asVec3d();
				ss << "{ \"" << s_type_key << "\": \"" << type_str << "\", \"x\": " << v[0] << ", \"y\": " << v[1] << ", \"z\": " << v[2] << " } ";
			}
			break;
			
		case AbstractProperty::VEC4F:
		case AbstractProperty::VEC4D:
			{
				osg::Vec4d v = value->asVec4d();
				if (type_str == "color")
					ss << "{ \"" << s_type_key << "\": \"" << type_str << "\", \"r\": " << v[0] << ", \"g\": " << v[1] << ", \"b\": " << v[2] << ", \"a\": " << v[3] << " } ";
				else if (type_str == "rect")
					ss << "{ \"" << s_type_key << "\": \"" << type_str << "\", \"l\": " << v[0] << ", \"t\": " << v[1] << ", \"w\": " << (v[2]-v[0]) << ", \"h\": " << (v[3]-v[1]) << " } ";
				else 
					ss << "{ \"" << s_type_key << "\": \"" << type_str << "\", \"x\": " << v[0] << ", \"y\": " << v[1] << ", \"z\": " << v[2] << ", \"w\": " << v[3] << " } ";

			}
			break;
		
		case AbstractProperty::MATRIXF:
		case AbstractProperty::MATRIXD:
			{
				osg::Matrixd v = value->asMatrixd();
				ss << "{ \"" << s_type_key << "\": " << stringifyString(type_str) << ", ";
				for(unsigned int y=0; y<4; ++y) {
					for(unsigned int x=0; x<4; ++x) {
						ss << " \"i"+intToString(x)+intToString(y) << "\": " << floatToString(v(x,y));
						if ((y == 3) && (x == 3)) {
							;
						} 
						else {
							ss << ", ";
						}
					}
				}
				ss << " } ";
			}
			break;
			
		case AbstractProperty::QUAT:
			{
				osg::Quat v = value->asQuat();
				ss << "{ \"" << s_type_key << "\": \"" << type_str << "\", \"x\": " << v[0] << ", \"y\": " << v[1] << ", \"z\": " << v[2] << ", \"w\": " << v[3] << " } ";
			}
			break;

		case AbstractProperty::PROPERTY_LIST:
			{
				cefix::PropertyList* pl = value->asPropertyList();
				if (!pl)
					ss << "null\n";
				else 
				{
					ss << " { \n";
					unsigned j(0);
					for (PropertyList::PropertyListT::iterator i = pl->_list.begin(); i != pl->_list.end(); ++i, ++j) 
					{
						ss << stringifyToUTF8(i->first, i->second.get(), depth + 1);
						if (j+1 != pl->_list.size()) 
							ss << ", \n";
					}
					ss << "\n";
					if (depth)
						ss << std::setw(depth * 2) << " " << "}";
				}
			}
			break;
		
		case AbstractProperty::MULTIPLE_VALUE_LIST:
			{
				MultipleValueList::List& l = value->asMultipleValueList()->list();
				
				ss << " [ \n";
				unsigned int j(0);
				for(MultipleValueList::List::iterator i = l.begin(); i != l.end(); ++i, ++j) 
				{
					ss << stringifyToUTF8("", (*i), depth + 1);
					if (j+1 != l.size())
						ss << ", \n";
				}
				
				ss << "\n";
				
                if (depth)
					ss << std::setw(depth * 2) << " "; 
                ss << "]";
			}
			break;
			
		case AbstractProperty::OBJECT:
			ss << "null";
			break;
		default:
			break;
	}
	
	return ss.str();
}


std::wstring JSON::stringify(const std::string& key, AbstractProperty *value, unsigned int depth)
{
	return cefix::Unicode::UTF8toWstring(stringifyToUTF8(key, value, depth));
}

std::wstring JSON::stringify(const std::string& key, cefix::PropertyList *value)
{
	osg::ref_ptr<cefix::AbstractProperty> prop = new cefix::PropertyT<osg::Referenced*>(AbstractProperty::PROPERTY_LIST, value);
	return L"{ \n" + stringify(key, prop, 1) + L"\n}";
}



std::string JSON::stringifyToUTF8(const std::string& key, cefix::PropertyList *value)
{
	osg::ref_ptr<cefix::AbstractProperty> prop = new cefix::PropertyT<osg::Referenced*>(AbstractProperty::PROPERTY_LIST, value);
	return "{ \n" + stringifyToUTF8(key, prop, 1) + "\n} ";
}	



std::string JSON::stringifyToUTF8(cefix::PropertyList *value)
{
	std::string result = "{ \n";
    unsigned int num_items = value->size();
    for(unsigned int i= 0; i < num_items; ++i) {
        if (i > 0)
            result += ",\n";
        result += stringifyToUTF8(value->getKeyAt(i), value->get(i), 1);
    }
    return result + "\n}";
}		


std::wstring JSON::stringify(cefix::PropertyList *value)
{
    std::wstring result = L"{ \n";
    for(unsigned int i= 0; i < value->size(); ++i) {
        result += stringify(value->getKeyAt(i), value->get(i), 1)+L"\n";
    }
    return result + L"}";
}

/** 
 * Skips over any whitespace characters (space, tab, \r or \n) defined by the JSON spec
 *
 * @access protected
 *
 * @param wchar_t** data Pointer to a wchar_t* that contains the JSON text
 *
 * @return bool Returns true if there is more data, or false if the end of the text was reached
 */
bool JSON::skipWhitespace(const wchar_t **data)
{
	while (**data != 0 && (**data == L' ' || **data == L'\t' || **data == L'\r' || **data == L'\n'))
		(*data)++;
	
	return **data != 0;
}

/** 
 * Extracts a JSON String as defined by the spec - "<some chars>"
 * Any escaped characters are swapped out for their unescaped values
 *
 * @access protected
 *
 * @param wchar_t** data Pointer to a wchar_t* that contains the JSON text
 * @param std::wstring& str Reference to a std::wstring to receive the extracted string
 *
 * @return bool Returns true on success, false on failure
 */
bool JSON::extractString(const wchar_t **data, std::wstring &str)
{
	str = L"";
	
	while (**data != 0)
	{
		// Save the char so we can change it if need be
		wchar_t next_char = **data;
		
		// Escaping something?
		if (next_char == L'\\')
		{
			// Move over the escape char
			(*data)++;
			
			// Deal with the escaped char
			switch (**data)
			{
				case L'"': next_char = L'"'; break;
				case L'\\': next_char = L'\\'; break;
				case L'/': next_char = L'/'; break;
				case L'b': next_char = L'\b'; break;
				case L'f': next_char = L'\f'; break;
				case L'n': next_char = L'\n'; break;
				case L'r': next_char = L'\r'; break;
				case L't': next_char = L'\t'; break;
				case L'u':
				{
					// We need 5 chars (4 hex + the 'u') or its not valid
					if (wcslen(*data) < 5)
						return false;
					
					// Deal with the chars
					next_char = 0;
					for (int i = 0; i < 4; i++)
					{
						// Do it first to move off the 'u' and leave us on the 
						// final hex digit as we move on by one later on
						(*data)++;
						
						next_char <<= 4;
						
						// Parse the hex digit
						if (**data >= '0' && **data <= '9')
							next_char |= (**data - '0');
						else if (**data >= 'A' && **data <= 'F')
							next_char |= (10 + (**data - 'A'));
						else if (**data >= 'a' && **data <= 'f')
							next_char |= (10 + (**data - 'a'));
						else
						{
							// Invalid hex digit = invalid JSON
							return false;
						}
					}
					break;
				}
				
				// By the spec, only the above cases are allowed
				default:
					return false;
			}
		}
		
		// End of the string?
		else if (next_char == L'"')
		{
			(*data)++;
			str.reserve(); // Remove unused capacity
			return true;
		}
		
		// Disallowed char?
		else if (next_char < L' ' && next_char != L'\t')
		{
			// SPEC Violation: Allow tabs due to real world cases
			return false;
		}
		
		// Add the next char
		str += next_char;
		
		// Move on
		(*data)++;
	}
	
	// If we're here, the string ended incorrectly
	return false;
}

/** 
 * Parses some text as though it is an integer
 *
 * @access protected
 *
 * @param wchar_t** data Pointer to a wchar_t* that contains the JSON text
 *
 * @return double Returns the double value of the number found
 */
double JSON::parseInt(const wchar_t **data)
{
	double integer = 0;
	while (**data != 0 && **data >= '0' && **data <= '9')
		integer = integer * 10 + (*(*data)++ - '0');
	
	return integer;
}

/** 
 * Parses some text as though it is a decimal
 *
 * @access protected
 *
 * @param wchar_t** data Pointer to a wchar_t* that contains the JSON text
 *
 * @return double Returns the double value of the decimal found
 */
double JSON::parseDecimal(const wchar_t **data)
{
	double decimal = 0.0;
	double factor = 0.1;
	while (**data != 0 && **data >= '0' && **data <= '9')
	{
		int digit = (*(*data)++ - '0');
		decimal = decimal + digit * factor;
		factor *= 0.1;
	}
	return decimal;
}

std::string JSON::stringifyString(const std::string &str)
{
	std::wstring str_out = L"\"";
	
	unsigned int m = str.length();
	for(unsigned int j = 0; j < m; )
	{
		wchar_t chr = Unicode::getCodePointFromUTF8(str, j);

		if (chr == L'"' || chr == L'\\' || chr == L'/')
		{
			str_out += L'\\';
			str_out += chr;
		}
		else if (chr == L'\b')
		{
			str_out += L"\\b";
		}
		else if (chr == L'\f')
		{
			str_out += L"\\f";
		}
		else if (chr == L'\n')
		{
			str_out += L"\\n";
		}
		else if (chr == L'\r')
		{
			str_out += L"\\r";
		}
		else if (chr == L'\t')
		{
			str_out += L"\\t";
		}
		else if (chr < L' ')
		{
			str_out += L"\\u";
			for (int i = 0; i < 4; i++)
			{
				int value = (chr >> 12) & 0xf;
				if (value >= 0 && value <= 9)
					str_out += (unsigned char)(L'0' + value);
				else if (value >= 10 && value <= 15)
					str_out += (unsigned char)(L'A' + (value - 10));
				chr <<= 4;
			}
		}
		else
		{
			str_out += chr;
		}
		
	}
	
	str_out += L"\"";
	return Unicode::wstringToUTF8(str_out);
}


AbstractProperty *JSON::parseValue(const wchar_t **data, const ParseOptions& options)
{
	// Is it a string?
	if (**data == '"')
	{
		std::wstring str;
		if (!JSON::extractString(&(++(*data)), str))
			return NULL;
		else
			return new PropertyT<std::string>(Unicode::wstringToUTF8(str) );
	}
	
	// Is it a boolean?
	else if ((simplejson_wcsnlen(*data, 4) && wcsncasecmp(*data, L"true", 4) == 0) || (simplejson_wcsnlen(*data, 5) && wcsncasecmp(*data, L"false", 5) == 0))
	{
		bool value = wcsncasecmp(*data, L"true", 4) == 0;
		(*data) += value ? 4 : 5;
		return new PropertyT<bool>(value);
	}
	
	// Is it a null?
	else if (simplejson_wcsnlen(*data, 4) && wcsncasecmp(*data, L"null", 4) == 0)
	{
		(*data) += 4;
		return new AbstractProperty();
	}
	
	// Is it a number?
	else if (**data == L'-' || (**data >= L'0' && **data <= L'9'))
	{
		// Negative?
		bool neg = **data == L'-';
		if (neg) (*data)++;

		double number = 0.0;

		// Parse the whole part of the number - only if it wasn't 0
		if (**data == L'0')
			(*data)++;
		else if (**data >= L'1' && **data <= L'9')
			number = JSON::parseInt(data);
		else
			return NULL;
		
		// Could be a decimal now...
		if (**data == '.')
		{
			(*data)++;

			// Not get any digits?
			if (!(**data >= L'0' && **data <= L'9'))
				return NULL;
			
			// Find the decimal and sort the decimal place out
			// Use ParseDecimal as ParseInt won't work with decimals less than 0.1
			// thanks to Javier Abadia for the report & fix
			double decimal = JSON::parseDecimal(data);
			
			// Save the number
			number += decimal;
		}

		// Could be an exponent now...
		if (**data == L'E' || **data == L'e')
		{
			(*data)++;

			// Check signage of expo
			bool neg_expo = false;
			if (**data == L'-' || **data == L'+')
			{
				neg_expo = **data == L'-';
				(*data)++;
			}
			
			// Not get any digits?
			if (!(**data >= L'0' && **data <= L'9'))
				return NULL;

			// Sort the expo out
			double expo = JSON::parseInt(data);
			for (double i = 0.0; i < expo; i++)
				number = neg_expo ? (number / 10.0) : (number * 10.0);
		}

		// Was it neg?
		if (neg) number *= -1;

		return new PropertyT<double>(number);
	}

	// An object?
	else if (**data == L'{')
	{
		osg::ref_ptr<PropertyList> object = new PropertyList();
		
		(*data)++;
	
		while (**data != 0)
		{
			// Whitespace at the start?
			if (!JSON::skipWhitespace(data))
			{
				return NULL;
			}
			
			// Special case - empty object
			if (object->size() == 0 && **data == L'}')
			{
				(*data)++;
				return new PropertyT<osg::Referenced*>(AbstractProperty::PROPERTY_LIST, object);
			}
			
			// We want a string now...
			std::wstring name;
			if (!JSON::extractString(&(++(*data)), name))
			{
				return NULL;
			}
			
			// More whitespace?
			if (!JSON::skipWhitespace(data))
			{
				return NULL;
			}
			
			// Need a : now
			if (*((*data)++) != L':')
			{
				return NULL;
			}
			
			// More whitespace?
			if (!JSON::skipWhitespace(data))
			{
				return NULL;
			}
			
			// The value is here			
			osg::ref_ptr<AbstractProperty> value = parseValue(data, options);
			if (value == NULL)
			{
				return NULL;
			}
			
			if (value->getType() == AbstractProperty::MULTIPLE_VALUE_LIST) 
			{
				if (options.mergeArrays) 
                {
                    MultipleValueList* mvl = value->asMultipleValueList();
                    if(mvl) 
                    {
                        const MultipleValueList::List& list = mvl->list();
                        for(MultipleValueList::List::const_iterator k = list.begin(); k != list.end(); ++k) 
                        {
                            object->addValue(Unicode::wstringToUTF8(name), (*k));
                        }
                    }
                }
                else {
                    osg::ref_ptr<cefix::PropertyList> sub_pl = new cefix::PropertyList();
                    
                    MultipleValueList* mvl = value->asMultipleValueList();
                    if(mvl) 
                    {
                        const MultipleValueList::List& list = mvl->list();
                        for(MultipleValueList::List::const_iterator k = list.begin(); k != list.end(); ++k) 
                        {
                            sub_pl->addValue(Unicode::wstringToUTF8(name), (*k));
                        }
                    }
                    object->addValue(Unicode::wstringToUTF8(name), new PropertyT<osg::Referenced*>(AbstractProperty::PROPERTY_LIST,sub_pl.get()));
                }
			}
			
			else 
			{
				object->addValue(Unicode::wstringToUTF8(name), value);
			}
			
			// More whitespace?
			if (!JSON::skipWhitespace(data))
			{
				return NULL;
			}
			
			// End of object?
			if (**data == L'}')
			{
				(*data)++;
				
				if (object->hasKey(s_type_key)) 
				{
					return convertToNativeType(object);
				} 
				else 
				{
					return new PropertyT<osg::Referenced*>(AbstractProperty::PROPERTY_LIST, object.get());
				}
			}
			
			// Want a , now
			if (**data != L',')
			{
				return NULL;
			}
			
			(*data)++;
		}
		
		// Only here if we ran out of data
		return NULL;
	}
	
	// An array?
	else if (**data == L'[')
	{
		osg::ref_ptr<MultipleValueList> array = new MultipleValueList();
		
		(*data)++;
        unsigned int count = 0;
		
		while (**data != 0)
		{
			// Whitespace at the start?
			if (!JSON::skipWhitespace(data))
			{
				return NULL;
			}
			
			// Special case - empty array
			if (array->count() == 0 && **data == L']')
			{
				(*data)++;
				return array.release();
			}
			
			// Get the value
			osg::ref_ptr<AbstractProperty> value = parseValue(data, options);
			if (value == NULL)
			{
				return NULL;
			}
			
			// Add the value
			array->add(value);
			
			// More whitespace?
			if (!JSON::skipWhitespace(data))
			{
				return NULL;
			}
			
			// End of array?
			if (**data == L']')
			{
				(*data)++;
				return array.release();
			}
			
			// Want a , now
			if (**data != L',')
			{
				return NULL;
			}
			
			(*data)++;
		}
		
		// Only here if we ran out of data
		return NULL;
	}
	
	// Ran out of possibilites, it's bad!
	else
	{
		return NULL;
	}
}


AbstractProperty* JSON::convertToNativeType(PropertyList* pl) 
{
	std::string type= pl->get(s_type_key)->asString();
	AbstractProperty* result = NULL;
	if (type=="vec2f") {
		osg::Vec2f v;
		v[0] = pl->get("x")->asFloat();
		v[1] = pl->get("y")->asFloat();
		result = new PropertyT<osg::Vec2f>(v);
	}
	else if((type=="vec2d") || (type=="point")) {
		osg::Vec2d v;
		v[0] = pl->get("x")->asDouble();
		v[1] = pl->get("y")->asDouble();
		result = new PropertyT<osg::Vec2d>(v);
	}
	else if(type=="size") {
		osg::Vec2d v;
		v[0] = pl->get("width")->asDouble();
		v[1] = pl->get("height")->asDouble();
		result = new PropertyT<osg::Vec2d>(v);
	}
	else if (type=="vec3f") {
		osg::Vec3f v;
		v[0] = pl->get("x")->asFloat();
		v[1] = pl->get("y")->asFloat();
		v[2] = pl->get("z")->asFloat();
		result = new PropertyT<osg::Vec3f>(v);
	}
	else if((type=="vec3d") || (type=="vector")) {
		osg::Vec3d v;
		v[0] = pl->get("x")->asDouble();
		v[1] = pl->get("y")->asDouble();
		v[2] = pl->get("z")->asDouble();
		result = new PropertyT<osg::Vec3d>(v);
	}
	else if (type=="vec4f") {
		osg::Vec4f v;
		v[0] = pl->get("x")->asFloat();
		v[1] = pl->get("y")->asFloat();
		v[2] = pl->get("z")->asFloat();
		v[3] = pl->get("w")->asFloat();
		result = new PropertyT<osg::Vec4f>(v);
	}
	else if(type=="vec4d") {
		osg::Vec4d v;
		v[0] = pl->get("x")->asDouble();
		v[1] = pl->get("y")->asDouble();
		v[2] = pl->get("z")->asDouble();
		v[3] = pl->get("w")->asDouble();
		result = new PropertyT<osg::Vec4d>(v);
	}
	else if(type=="color") {
		osg::Vec4d v;
		v[0] = pl->get("r")->asDouble();
		v[1] = pl->get("g")->asDouble();
		v[2] = pl->get("b")->asDouble();
		v[3] = pl->get("a")->asDouble();
		result = new PropertyT<osg::Vec4d>(v);
	}
	else if(type=="rect") {
		osg::Vec4d v;
		v[0] = pl->get("l")->asDouble();
		v[1] = pl->get("t")->asDouble();
		v[2] = v[0] + pl->get("w")->asDouble();
		v[3] = v[1] + pl->get("h")->asDouble();
		result = new PropertyT<osg::Vec4d>(v);
	}
	else if(type=="quat") {
		osg::Quat v;
		v[0] = pl->get("x")->asDouble();
		v[1] = pl->get("y")->asDouble();
		v[2] = pl->get("z")->asDouble();
		v[3] = pl->get("w")->asDouble();
		result = new PropertyT<osg::Quat>(v);
	}
	else if(type=="matrixf") {
		osg::Matrixf v;
		for(unsigned int y = 0; y < 4; ++y) {
			for(unsigned int x = 0; x < 4; ++x) {
				v(x,y) = pl->get("i"+intToString(x)+intToString(y))->asDouble();
			}
		}
		result = new PropertyT<osg::Matrixf>(v);
	}
	else if(type=="matrixd") {
		osg::Matrixd v;
		for(unsigned int y = 0; y < 4; ++y) {
			for(unsigned int x = 0; x < 4; ++x) {
				v(x,y) = pl->get("i"+intToString(x)+intToString(y))->asDouble();
			}
		}
		result = new PropertyT<osg::Matrixd>(v);
	}
	else {
		// unknown type!
		cefix::log::error("JSON") << "unknown type: " << type << std::endl;
		result = new PropertyT<osg::Referenced*>(AbstractProperty::PROPERTY_LIST, pl);
	}
	
	if (result) {
		result->setXmlType(type);
		return result;
	}
	
	return NULL;

}


}
