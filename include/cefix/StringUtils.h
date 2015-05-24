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

#ifndef __STRING_UTILS_HEADER__
#define __STRING_UTILS_HEADER__ 1
#include <string>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <vector>
#include <cefix/Export.h>

#include <osg/Vec4>

namespace cefix {

	template<typename T> T strReplaceAll(const T& source, const T& to_find, const T& to_replace) {
		T result(source);
		
		typename T::size_type pos = 0;
		typename T::size_type last_pos = 0;
		
		while (pos != T::npos) {
			
			pos = result.find(to_find, last_pos);
			if (pos != T::npos) {
			
				result.replace(pos, to_find.length(), to_replace);
				last_pos = pos + to_replace.length();
			
			}
		
		}
		return result;
	}

    /** compares to strings disreagrding the case*/
    int CEFIX_EXPORT stricmp(const char * cs,const char * ct);
    /**
     * returns a lower-case string
     */
    std::string CEFIX_EXPORT strToLower(const std::string &str);
	std::wstring CEFIX_EXPORT strToLower (const std::wstring &str);

	/**
     * returns a upper-case string
     */
	std::string CEFIX_EXPORT strToUpper(const std::string &str);
	std::wstring CEFIX_EXPORT strToUpper(const std::wstring &str);
	
	/** returns the string in title case */
	std::wstring CEFIX_EXPORT strToTitleCase (const std::wstring &str);
    /** returns the string in title case */
	std::string CEFIX_EXPORT strToTitleCase (const std::string &str);
	
    /** trims a string, removes trailing and leading whit-spaces */
    std::string CEFIX_EXPORT strTrim(const std::string & str);
    
	/** trims a wstring, removes trailing and leading whit-spaces */
	std::wstring CEFIX_EXPORT strTrim(const std::wstring& str);
	
    /** splits a string into a string-vector, using delimiters as delimiter */
    void CEFIX_EXPORT strTokenize(const std::string& str,
                      std::vector<std::string>& tokens,
                      const std::string& delimiters = " ");
	 
	/** splits a wstring into a wstring-vector, using delimiters as delimiter */				  
	void CEFIX_EXPORT strTokenize(const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters);


	/** splits a string into a string-vector, using delimiters as delimiter */
    void CEFIX_EXPORT strSplit(const std::string& str,
                      std::vector<std::string>& tokens,
                      char delimiter = ' ');

	
	template <class T>
	std::string strJoin(const std::vector<T>& elements, const std::string& delimiter) 
	{
		std::ostringstream result;
		switch (elements.size()) {
			case 0: 
				return "";
				break;
			case 1:
				result << elements[0];
				break;
			default:
				std::copy(elements.begin(), elements.end()-1, std::ostream_iterator<T>(result, delimiter.c_str()));
				result << *(elements.rbegin());
				break;
		}
		return result.str();
	}

    /** converts a float into a string */
	std::string CEFIX_EXPORT floatToString(float f, int precision = 10);	
    
    /** converts an integer into a string */
	std::string CEFIX_EXPORT intToString(int f);
    
    /** converts a double into a string */
	std::string CEFIX_EXPORT doubleToString(double d, int precision = 20);
    
    /** converts a hexadecimal string into a long */
    unsigned int CEFIX_EXPORT hexToDec(const std::string& s);
	
	unsigned long long hexToLong(const std::string& s);
	
	std::string intToHexString(unsigned int i, unsigned int min_len = 0, const std::string& prefix = "0x");
	std::string longToHexString(unsigned long long i, unsigned int min_len = 0, const std::string& prefix = "0x");

	/** converts a Vec4 into a string with hex-notation (like in HTML)
     * @param c color to convert
     * @param includeAlphaComponent if true, then the resulting hex-string will include the alpha component like #11223344
     */
	std::string CEFIX_EXPORT vec4ToHexColor(osg::Vec4 c, bool includeAlphaComponent = false);
    
    /** returns a Vec4 from a hex-coded color */
	osg::Vec4 CEFIX_EXPORT hexColorToVec4(std::string hex);
	
	/** compute the MD5-digest from a string, if you need more control, look at md5.h */
    std::string computeMD5(const std::string& source);
	
	
	std::string convertFromPascalString(const char* s);
	/** 
	 * static class which encapsulates some unicode transformation functions.
	 * they will convert from UTF8 to UCS-2 and UCS-4 strings and back
	 */
	
    static inline float stringToFloat(const std::string& str) { return atof(str.c_str()); }
    static inline double stringToDouble(const std::string& str) { return atof(str.c_str()); }
    static inline int stringToInt(const std::string& str) { return atoi(str.c_str()); }
    static inline long stringToLong(const std::string& str) { return atol(str.c_str()); }
    
	struct Unicode {
		typedef unsigned short   Unicode2Bytes;
		typedef unsigned int     Unicode4Bytes;
		typedef unsigned char	 byte;
		
		static void CEFIX_EXPORT fromUCS2toUTF8(const std::basic_string< Unicode2Bytes >& input, std::basic_string< byte >& output);
		static void CEFIX_EXPORT fromUTF8toUCS2(const std::basic_string< byte >& input, std::basic_string< Unicode2Bytes >& output);
		static void CEFIX_EXPORT fromUCS4toUTF8(const std::wstring& input, std::string& output);
		static void CEFIX_EXPORT fromUTF8toUCS4(const std::string& input, std::wstring& output);
		
		static unsigned int getCodePointFromUTF8(const std::string& input,unsigned int& i);
        static unsigned int getCodePointFromUTF8(const char* input);
        static const char* jumpToNextUTF8CodePoint(const char* input);
		static const char* jumpToPrevtUTF8CodePoint(const char* input);
		static std::string CEFIX_EXPORT wstringToUTF8(const std::wstring& s) {
		
			std::string result;
			fromUCS4toUTF8(s, result);
			return result;
		}
		
		static std::wstring CEFIX_EXPORT UTF8toWstring(const std::string& s) {
		
			std::wstring result;
			fromUTF8toUCS4(s, result);
			return result;
		}
        
        
        static std::string UTF8Compose(const std::string& in_str);
        static std::string UTF8Decompose(const std::string& in_str);
        
			

	};
}

std::ostream& operator << (std::ostream& os, const std::wstring& s);

#endif

