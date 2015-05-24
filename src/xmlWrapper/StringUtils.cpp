/*
 *  stringUtils.cpp
 *  NativeContext
 *
 *  Created by Stephan Huber on Fri Apr 04 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdlib.h>

#include <cefix/StringUtils.h>
#include <cctype>
#include <sstream>
#include <osg/Vec3>
#include <osg/Vec4>
#include <cefix/MD5.h>
#include <cefix/Log.h>

extern "C" {
#include "utf8proc.h"
}


std::ostream& operator << (std::ostream& os, const std::wstring& s) {
	os << cefix::Unicode::wstringToUTF8(s);
	return os;
}

#ifdef WIN32

inline unsigned long long strtoull(const char* t, char** endpointer, unsigned int radix) {
	return _strtoui64(t, endpointer, radix);
}

#endif

namespace cefix {

    int stricmp(const char * cs,const char * ct)
    {
      signed char __res;

      while (1) {
        if ((__res = toupper( *cs ) - toupper( *ct++ )) != 0 || !*cs++)
          break;
      }

      return __res;
    }

    // ---------------------------------------------------------------------------
    // str_lower
    // wandelt einen string in kleinbuchstaben
    // ---------------------------------------------------------------------------
    std::string strToLower (const std::string &str) {
		
		std::string res(str.length(), ' ');
        for(std::string::size_type i = 0; i < res.length(); ++i)
            res[i] = tolower(str[i]); 
        return res;
		
    }
	
	// ---------------------------------------------------------------------------
    // str_lower
    // wandelt einen string in kleinbuchstaben
    // ---------------------------------------------------------------------------
    std::wstring strToLower (const std::wstring &str) {
		
		std::wstring res(str.length(), ' ');
        for(std::wstring::size_type i = 0; i < res.length(); ++i)
            res[i] = towlower(str[i]); 
        return res;
		
    }
	
	// ---------------------------------------------------------------------------
    // strToUpper
    // wandelt einen string in kleinbuchstaben
    // ---------------------------------------------------------------------------
    std::string strToUpper (const std::string &str) {
		
		std::string res(str.length(), ' ');
        for(std::string::size_type i = 0; i < res.length(); ++i)
            res[i] = toupper(str[i]); 
        return res;
		
    }
	
	// ---------------------------------------------------------------------------
    // strToUpper
    // wandelt einen string in kleinbuchstaben
    // ---------------------------------------------------------------------------
    std::wstring strToUpper (const std::wstring &str) {
		
		std::wstring res(str.length(), ' ');
        for(std::wstring::size_type i = 0; i < res.length(); ++i)
            res[i] = towupper(str[i]); 
        return res;
		
    }
	
	// ---------------------------------------------------------------------------
    // strToTitleCase
    // wandelt einen string in kleinbuchstaben
    // ---------------------------------------------------------------------------
    std::wstring strToTitleCase (const std::wstring &str) {
		
		std::wstring res(str.length(), ' ');
		bool doupper = true;
        for(std::wstring::size_type i = 0; i < res.length(); ++i) {
            res[i] = (doupper) ? towupper(str[i]) : towlower(str[i]); 
			doupper = (str[i] == ' ');
        }
		return res;
		
    }
	
	// ---------------------------------------------------------------------------
    // strToTitleCase
    // wandelt einen string in kleinbuchstaben
    // ---------------------------------------------------------------------------
    std::string strToTitleCase (const std::string &str) {
		
		std::string res(str.length(), ' ');
		bool doupper = true;
        for(std::string::size_type i = 0; i < res.length(); ++i) {
            res[i] = (doupper) ? toupper(str[i]) : tolower(str[i]); 
			doupper = (str[i] == ' ');
        }
		return res;
		
    }

    
    // ---------------------------------------------------------------------------
    // str_trim
    // entfernt leerzeichen am ende und am anfang
    // ---------------------------------------------------------------------------
    template <typename T> T strTrim(const T& str, const T& delims) {

		typename T::size_type endindex = str.find_last_not_of(delims);
		if( endindex == T::npos )
			return T();

		typename T::size_type startindex = str.find_first_not_of(delims);
		if( startindex == T::npos ) {
			startindex = 0;
		}
     
        return str.substr(startindex,endindex-startindex+1);
    }

	std::string strTrim(const std::string& str) {
	
		return strTrim<std::string>(str,"\n\t\r ");
	}

	std::wstring strTrim(const std::wstring& str) {
	
		return strTrim<std::wstring>(str,L"\n\t\r ");
	}
    
    template <typename T> void strTokenize(const T& str,
                      std::vector<T>& tokens,
                      const T& delimiters)
	{
		// Skip delimiters at beginning.
		typename T::size_type lastPos = str.find_first_not_of(delimiters, 0);
		// Find first "non-delimiter".
		typename T::size_type pos     = str.find_first_of(delimiters, lastPos);

		while (T::npos != pos || T::npos != lastPos)
		{
			// Found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));
			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);
			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}
	}
	
	void strTokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters) {
		strTokenize<std::string>(str, tokens, delimiters);
	}
	
	void strTokenize(const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters) {
		strTokenize<std::wstring>(str, tokens, delimiters);
	}
	
	void strSplit(const std::string& str, std::vector<std::string>& tokens, char delimiter)
	{
		std::string::size_type  last_position(0);
		std::string::size_type position(0);
		for (std::string::const_iterator it(str.begin()); it != str.end(); ++it, ++position)
		{
			if (*it == delimiter)
			{
				if (last_position == position) {
					tokens.push_back("");
				} else {
					tokens.push_back(str.substr(last_position, position - last_position));
				}
				last_position = position+1;
			}
		}
		
		if (last_position < str.length()) {
			tokens.push_back(str.substr(last_position, str.length() - last_position));
		}
		if (position == last_position && last_position == str.length()) {
			tokens.push_back("");
		}
	}
	
	
	
	std::string floatToString(float f, int precision) {
		std::ostringstream ss;
		ss << std::fixed << std::setprecision(precision) << f;
		std::string s(ss.str());
		int lastndx = s.length()-1;
		bool shortened(false);
		if (s.find(".") != std::string::npos) {
			while (lastndx > 0 && s[lastndx] =='0') {
				--lastndx;
				shortened =	true;
			}
			if (s[lastndx] == '.') ++lastndx;
		}
		return shortened ? s.substr(0,lastndx+1) : s;
	}
	
	std::string intToString(int f) {
		std::ostringstream ss;
		ss << f;
		return ss.str();
	}
	
	std::string doubleToString(double d, int precision) {
		std::ostringstream ss;
		ss << std::fixed << std::setprecision(precision) << d;
		std::string s(ss.str());
		int lastndx = s.length()-1;
		bool f(false);
		if (s.find(".") != std::string::npos) {
			while (lastndx > 0 && s[lastndx] =='0') {
				--lastndx;
				f =	true;
			}
			if (s[lastndx] == '.') ++lastndx;
		}
		return f ? s.substr(0,lastndx+1) : s;
	}


    std::string vec3ToHexColor(osg::Vec3 c) {
		std::ostringstream ss;
        ss << "#" << std::setfill('0') << std::hex << std::uppercase << std::setw(2) <<(int)(c[0]*255.0f) << std::setw(2) << (int)(c[1]*255.0f) << std::setw(2) << (int)(c[2]*255.0f);
        
        return ss.str();
    }
    
	std::string vec4ToHexColor(osg::Vec4 c, bool includeAlphaComponent) {
		std::ostringstream ss;
        ss << "#"  << std::setfill('0') << std::hex << std::uppercase << std::setw(2) << (int)(c[0]*255.0f) << std::setw(2) << (int)(c[1]*255.0f) << std::setw(2) <<(int)(c[2]*255.0f);
        if (includeAlphaComponent)
            ss << (int)(c[3]*255.0f);
        
        return ss.str();
    }
		
    unsigned int hexToDec(const std::string& s) 
	{
		if ((s[0] == '0') && (s[1] == 'x'))
			return (strtoul(s.substr(2,s.length()).c_str(), NULL, 16));
       
		 return (strtoul(s.c_str(), NULL, 16));
    } 

	unsigned long long hexToLong(const std::string& s) 
	{
		if ((s[0] == '0') && (s[1] == 'x'))
			return (strtoull(s.substr(2,s.length()).c_str(), NULL, 16));
       
		 return (strtoull(s.c_str(), NULL, 16));
    } 
	
	std::string intToHexString(unsigned int i, unsigned int min_len, const std::string& prefix) 
	{
		std::ostringstream ss;
        if (min_len > 0)
            ss << std::setw(min_len) << std::setfill('0');
		ss << std::hex << i;
		return prefix + strToUpper(ss.str());
	}
	
	std::string longToHexString(unsigned long long i, unsigned int min_len, const std::string& prefix)
	{
		std::ostringstream ss;
        if (min_len > 0)
            ss << std::setw(min_len) << std::setfill('0');
		ss << std::hex << i;
		return prefix + strToUpper(ss.str());
	}
	
	
	std::string convertFromPascalString(const char* s)
	{
		std::size_t len = s[0];
		const char* str = &s[1];
		
		return std::string(str, len);
	}


	osg::Vec4 CEFIX_EXPORT hexColorToVec4(std::string hex) {
        std::string s;
        float r,g,b,a;
		if ((hex.length() == 3) && (hex[0] != '#')) {
			r = hexToDec(hex.substr(0,1));
			g = hexToDec(hex.substr(1,1));
			b = hexToDec(hex.substr(2,1));
			return osg::Vec4(r / 15.0f, g / 15.0f, b / 15.0f, 1.0);
		} 
		else if ((hex.length() == 4) && (hex[0] == '#')) {
			r = hexToDec(hex.substr(1,1));
			g = hexToDec(hex.substr(2,1));
			b = hexToDec(hex.substr(3,1));
			return osg::Vec4(r / 15.0f, g / 15.0f, b / 15.0f, 1);
        } 
		else if (hex.length() < 6)
            return osg::Vec4(0,0,0,1);
        
        if (hex[0] == '#')
            s = hex.substr(1,osg::minimum((size_t)8, hex.length()));
        else
            s = hex.substr(0,osg::minimum((size_t)8, hex.length()));
         
        r = g = b = 0.0f;
        r = hexToDec(s.substr(0,2));
        g = hexToDec(s.substr(2,2));
        b = hexToDec(s.substr(4,2));
        
        if (s.length() > 6)
            a = hexToDec(s.substr(6,2));
        else
            a = 255.0f;
         
        return osg::Vec4(r / 255.0f, g / 255.0f, b / 255.0f, a/ 255.0f);
    }
	
	
	std::string computeMD5(const std::string& source) {
		
		MD5 context;
		context.update((unsigned char*)source.c_str(), source.length());
		context.finalize();
		
		return std::string(context.hex_digest());
	
	}
	
	
	// this code is a rip off from: 
	// http://codeguru.earthweb.com/Cpp/misc/misc/multi-lingualsupport/article.php/c10451/
	
	#define         MASKBITS                0x3F
	#define         MASKBYTE                0x80
	#define         MASK2BYTES              0xC0
	#define         MASK3BYTES              0xE0
	#define         MASK4BYTES              0xF0
	#define         MASK5BYTES              0xF8
	#define         MASK6BYTES              0xFC



	void Unicode::fromUCS2toUTF8(const std::basic_string< Unicode2Bytes >& input, std::basic_string< byte >& output)
	{
	   for(unsigned int i=0; i < input.size(); i++)
	   {
		  // 0xxxxxxx
		  if(input[i] < 0x80)
		  {
			 output.push_back((byte)input[i]);
		  }
		  // 110xxxxx 10xxxxxx
		  else if(input[i] < 0x800)
		  {
			 output.push_back((byte)(MASK2BYTES | input[i] >> 6));
			 output.push_back((byte)(MASKBYTE | (input[i] & MASKBITS)));
		  }
		  // 1110xxxx 10xxxxxx 10xxxxxx
		  else if(input[i] < 0x10000)
		  {
			 output.push_back((byte)(MASK3BYTES | input[i] >> 12));
			 output.push_back((byte)(MASKBYTE | (input[i] >> 6 & MASKBITS)));
			 output.push_back((byte)(MASKBYTE | (input[i] & MASKBITS)));
		  }
	   }
	}

	void Unicode::fromUTF8toUCS2(const std::basic_string< byte >& input, std::basic_string< Unicode2Bytes >& output)
	{
	   for(unsigned int i=0; i < input.size();)
	   {
		  Unicode2Bytes ch;

		  // 1110xxxx 10xxxxxx 10xxxxxx
		  if((input[i] & MASK3BYTES) == MASK3BYTES)
		  {
			 ch = ((input[i] & 0x0F) << 12) | (
				   (input[i+1] & MASKBITS) << 6)
				  | (input[i+2] & MASKBITS);
			 i += 3;
		  }
		  // 110xxxxx 10xxxxxx
		  else if((input[i] & MASK2BYTES) == MASK2BYTES)
		  {
			 ch = ((input[i] & 0x1F) << 6) | (input[i+1] & MASKBITS);
			 i += 2;
		  }
		  // 0xxxxxxx
		  else if(input[i] < MASKBYTE)
		  {
			 ch = input[i];
			 i += 1;
		  }

		  output.push_back(ch);
	   }
	}

	void Unicode::fromUCS4toUTF8(const std::wstring& input, std::string& output)
	{
	   for(unsigned int i=0; i < input.size(); i++)
	   {
		  // 0xxxxxxx
		  if(input[i] < 0x80)
		  {
			 output.push_back((byte)input[i]);
		  }
		  // 110xxxxx 10xxxxxx
		  else if(input[i] < 0x800)
		  {
			 output.push_back((byte)(MASK2BYTES | input[i] >> 6));
			 output.push_back((byte)(MASKBYTE | (input[i] & MASKBITS)));
		  }
		  // 1110xxxx 10xxxxxx 10xxxxxx
		  else if(input[i] < 0x10000)
		  {
			 output.push_back((byte)(MASK3BYTES | input[i] >> 12));
			 output.push_back((byte)(MASKBYTE | (input[i] >> 6 & MASKBITS)));
			 output.push_back((byte)(MASKBYTE | (input[i] & MASKBITS)));
		  }
		  // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		  else if(input[i] < 0x200000)
		  {
			 output.push_back((byte)(MASK4BYTES | input[i] >> 18));
			 output.push_back((byte)(MASKBYTE | (input[i] >> 12 & MASKBITS)));
			 output.push_back((byte)(MASKBYTE | (input[i] >> 6 & MASKBITS)));
			 output.push_back((byte)(MASKBYTE | (input[i] & MASKBITS)));
		  }
		  // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		  else if(input[i] < 0x4000000)
		  {
			 output.push_back((byte)(MASK5BYTES | input[i] >> 24));
			 output.push_back((byte)(MASKBYTE | (input[i] >> 18 & MASKBITS)));
			 output.push_back((byte)(MASKBYTE | (input[i] >> 12 & MASKBITS)));
			 output.push_back((byte)(MASKBYTE | (input[i] >> 6 & MASKBITS)));
			 output.push_back((byte)(MASKBYTE | (input[i] & MASKBITS)));
		  }
		  // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		  else if(input[i] < 0x8000000)
		  {
			 output.push_back((byte)(MASK6BYTES | input[i] >> 30));
			 output.push_back((byte)(MASKBYTE | (input[i] >> 18 & MASKBITS)));
			 output.push_back((byte)(MASKBYTE | (input[i] >> 12 & MASKBITS)));
			 output.push_back((byte)(MASKBYTE | (input[i] >> 6 & MASKBITS)));
			 output.push_back((byte)(MASKBYTE | (input[i] & MASKBITS)));
		  }
	   }
	}
	
	

	void Unicode::fromUTF8toUCS4(const std::string& input,std::wstring& output)
	{
	   for(unsigned int i=0; i < input.size();)
	   {
		  wchar_t ch;

		  // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		  if((input[i] & MASK6BYTES) == MASK6BYTES)
		  {
			 ch = ((input[i] & 0x01) << 30) | ((input[i+1] & MASKBITS) << 24)
				  | ((input[i+2] & MASKBITS) << 18) | ((input[i+3]
							& MASKBITS) << 12)
				  | ((input[i+4] & MASKBITS) << 6) | (input[i+5] & MASKBITS);
			 i += 6;
		  }
		  // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		  else if((input[i] & MASK5BYTES) == MASK5BYTES)
		  {
			 ch = ((input[i] & 0x03) << 24) | ((input[i+1]
					& MASKBITS) << 18)
				  | ((input[i+2] & MASKBITS) << 12) | ((input[i+3]
					  & MASKBITS) << 6)
				  | (input[i+4] & MASKBITS);
			 i += 5;
		  }
		  // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		  else if((input[i] & MASK4BYTES) == MASK4BYTES)
		  {
			 ch = ((input[i] & 0x07) << 18) | ((input[i+1]
					& MASKBITS) << 12)
				  | ((input[i+2] & MASKBITS) << 6) | (input[i+3] & MASKBITS);
			 i += 4;
		  }
		  // 1110xxxx 10xxxxxx 10xxxxxx
		  else if((input[i] & MASK3BYTES) == MASK3BYTES)
		  {
			 ch = ((input[i] & 0x0F) << 12) | ((input[i+1] & MASKBITS) << 6)
				  | (input[i+2] & MASKBITS);
			 i += 3;
		  }
		  // 110xxxxx 10xxxxxx
		  else if((input[i] & MASK2BYTES) == MASK2BYTES)
		  {
			 ch = ((input[i] & 0x1F) << 6) | (input[i+1] & MASKBITS);
			 i += 2;
		  }
		  // 0xxxxxxx
		  else if(input[i] < MASKBYTE)
		  {
			 ch = input[i];
			 i += 1;
		  }
		  output.push_back(ch);
	   }
	}


	unsigned int Unicode::getCodePointFromUTF8(const std::string& input,unsigned int& i)
	{
	   unsigned int ch(0);

	  // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
	  if((input[i] & MASK6BYTES) == MASK6BYTES)
	  {
		 ch = ((input[i] & 0x01) << 30) | ((input[i+1] & MASKBITS) << 24)
			  | ((input[i+2] & MASKBITS) << 18) | ((input[i+3]
						& MASKBITS) << 12)
			  | ((input[i+4] & MASKBITS) << 6) | (input[i+5] & MASKBITS);
		 i += 6;
	  }
	  // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
	  else if((input[i] & MASK5BYTES) == MASK5BYTES)
	  {
		 ch = ((input[i] & 0x03) << 24) | ((input[i+1]
				& MASKBITS) << 18)
			  | ((input[i+2] & MASKBITS) << 12) | ((input[i+3]
				  & MASKBITS) << 6)
			  | (input[i+4] & MASKBITS);
		 i += 5;
	  }
	  // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	  else if((input[i] & MASK4BYTES) == MASK4BYTES)
	  {
		 ch = ((input[i] & 0x07) << 18) | ((input[i+1]
				& MASKBITS) << 12)
			  | ((input[i+2] & MASKBITS) << 6) | (input[i+3] & MASKBITS);
		 i += 4;
	  }
	  // 1110xxxx 10xxxxxx 10xxxxxx
	  else if((input[i] & MASK3BYTES) == MASK3BYTES)
	  {
		 ch = ((input[i] & 0x0F) << 12) | ((input[i+1] & MASKBITS) << 6)
			  | (input[i+2] & MASKBITS);
		 i += 3;
	  }
	  // 110xxxxx 10xxxxxx
	  else if((input[i] & MASK2BYTES) == MASK2BYTES)
	  {
		 ch = ((input[i] & 0x1F) << 6) | (input[i+1] & MASKBITS);
		 i += 2;
	  }
	  // 0xxxxxxx
	  else if(input[i] < MASKBYTE)
	  {
		 ch = input[i];
		 i += 1;
	  }
	  
	  return ch;
}
    
    unsigned int Unicode::getCodePointFromUTF8(const char* input)
	{
        unsigned int ch(0);
        unsigned int i(0);
        // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        if((input[i] & MASK6BYTES) == MASK6BYTES)
        {
            ch = ((input[i] & 0x01) << 30) | ((input[i+1] & MASKBITS) << 24)
            | ((input[i+2] & MASKBITS) << 18) | ((input[i+3]
                                                  & MASKBITS) << 12)
            | ((input[i+4] & MASKBITS) << 6) | (input[i+5] & MASKBITS);
            
        }
        // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if((input[i] & MASK5BYTES) == MASK5BYTES)
        {
            ch = ((input[i] & 0x03) << 24) | ((input[i+1]
                                               & MASKBITS) << 18)
            | ((input[i+2] & MASKBITS) << 12) | ((input[i+3]
                                                  & MASKBITS) << 6)
            | (input[i+4] & MASKBITS);
            
        }
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if((input[i] & MASK4BYTES) == MASK4BYTES)
        {
            ch = ((input[i] & 0x07) << 18) | ((input[i+1]
                                               & MASKBITS) << 12)
            | ((input[i+2] & MASKBITS) << 6) | (input[i+3] & MASKBITS);
            
        }
        // 1110xxxx 10xxxxxx 10xxxxxx
        else if((input[i] & MASK3BYTES) == MASK3BYTES)
        {
            ch = ((input[i] & 0x0F) << 12) | ((input[i+1] & MASKBITS) << 6)
            | (input[i+2] & MASKBITS);
            
        }
        // 110xxxxx 10xxxxxx
        else if((input[i] & MASK2BYTES) == MASK2BYTES)
        {
            ch = ((input[i] & 0x1F) << 6) | (input[i+1] & MASKBITS);
            
        }
        // 0xxxxxxx
        else if(input[i] < MASKBYTE)
        {
            ch = input[i];
        }
        
        return ch;
    }
    
    const char* Unicode::jumpToNextUTF8CodePoint(const  char* input)
	{
        // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx        
        if(((*input) & MASK6BYTES) == MASK6BYTES)
        {
            input += 6;
        }
        // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if(((*input) & MASK5BYTES) == MASK5BYTES)
        {
            input += 5;
        }
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if(((*input) & MASK4BYTES) == MASK4BYTES)
        {
            input += 4;
        }
        // 1110xxxx 10xxxxxx 10xxxxxx
        else if(((*input) & MASK3BYTES) == MASK3BYTES)
        {
            input += 3;
        }
        // 110xxxxx 10xxxxxx
        else if(((*input) & MASK2BYTES) == MASK2BYTES)
        {
            input += 2;
        }
        // 0xxxxxxx
        else if((*input) < MASKBYTE)
        {
            input += 1;
        }
        
        return input;
    }
    
    const char* Unicode::jumpToPrevtUTF8CodePoint(const  char* input)
	{
        // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        if(((*input-6) & MASK6BYTES) == MASK6BYTES)
        {
            return input - 6;
        }
        // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if(((*input-5) & MASK5BYTES) == MASK5BYTES)
        {
            return input - 5;
        }
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        else if(((*input-4) & MASK4BYTES) == MASK4BYTES)
        {
            return input - 4;
        }
        // 1110xxxx 10xxxxxx 10xxxxxx
        else if(((*input-3) & MASK3BYTES) == MASK3BYTES)
        {
            return input - 3;
        }
        // 110xxxxx 10xxxxxx
        else if(((*input-2) & MASK2BYTES) == MASK2BYTES)
        {
            return input - 2;
        }
        
        return input -1;
    }

std::string utf8_map(const std::string& str, unsigned int flags) 
{
    ssize_t result;
	uint8_t * destPtr;
	result = utf8proc_map( (uint8_t*)str.c_str(), str.size(), &destPtr, flags );
	if (result < 0) {
		cefix::log::info("UTF8Compose") << utf8proc_errmsg(result) << std::endl;
		return str;
	}
	std::string result_str((const char*)destPtr);
	free(destPtr);
	return result_str;
}

std::string Unicode::UTF8Compose(const std::string& str) 
{
    return utf8_map(str, UTF8PROC_COMPOSE);
}


std::string Unicode::UTF8Decompose(const std::string& str) 
{
    return utf8_map(str, UTF8PROC_DECOMPOSE);
}



}
