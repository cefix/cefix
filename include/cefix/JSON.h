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
 
 /* JSON based on SimpleJSON Library from http://mjpa.in/json modified to 
 * work with cefix's PropertyList
 */

/*
 * File JSON.h part of the SimpleJSON Library - http://mjpa.in/json
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

#ifndef CEFIX_JSON_H_
#define CEFIX__JSON_H_

// Win32 incompatibilities
#if defined(WIN32) && !defined(__GNUC__)
	#define wcsncasecmp _wcsnicmp 
	static inline bool isnan(double x) { return x != x; }
	static inline bool isinf(double x) { return !isnan(x) && isnan(x - x); }
#endif

#include <vector>
#include <string>
#include <map>
#include <cefix/PropertyList.h>


namespace cefix {
class JSON
{
	
	public:
        struct ParseOptions {
        public:
            bool mergeArrays;
            ParseOptions() : mergeArrays(false) {}
        };
        
		static AbstractProperty* parse(const std::string& data, const ParseOptions& options = ParseOptions());
		static AbstractProperty* parse(const wchar_t *data, const ParseOptions& options = ParseOptions());
        
		static std::wstring stringify(const std::string& key, AbstractProperty *value, unsigned int depth = 0);
		static std::string stringifyToUTF8(const std::string& key, AbstractProperty *value, unsigned int depth = 0);
		
		static std::wstring stringify(const std::string& key, cefix::PropertyList *value);
		static std::string stringifyToUTF8(const std::string& key, cefix::PropertyList *value);
        
        static std::wstring stringify(cefix::PropertyList *value);
        static std::string stringifyToUTF8(cefix::PropertyList *value);

		static const std::string& getTypeKey();
		static void setTypeKey(const std::string& key);
	protected:
		static bool skipWhitespace(const wchar_t **data);
		static bool extractString(const wchar_t **data, std::wstring &str);
		static double parseInt(const wchar_t **data);
		static double parseDecimal(const wchar_t **data);
		
		static std::string stringifyString(const std::string &str);
		static AbstractProperty* parseValue(const wchar_t** data, const ParseOptions& options);
		static AbstractProperty* convertToNativeType(PropertyList* pl);
	private:
		JSON();
};

}

#endif
