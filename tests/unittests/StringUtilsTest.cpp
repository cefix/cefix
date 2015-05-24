/*
 *  StringUtilsTest.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 14.01.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/StringUtils.h>

#include "catch.hpp"
#include <iostream>



TEST_CASE( "StringUtils/MD5", "MD5" )
{
	REQUIRE(cefix::computeMD5("test") == "098f6bcd4621d373cade4e832627b4f6");
}

TEST_CASE( "StringUtils/strToLower", "strToLower" )
{
	REQUIRE(cefix::strToLower("Dies Ist ein Test") == "dies ist ein test");
	REQUIRE(cefix::strToLower(L"Dies ist EiN TesT") == L"dies ist ein test");

	// umlaute
	
	//REQUIRE(cefix::strToLower("ÄÖÜ"), "äöü");
}

TEST_CASE( "StringUtils/hexToColor", "hexToColor" )
 {

	REQUIRE(cefix::hexColorToVec4("#fff") == osg::Vec4(1,1,1,1));
	REQUIRE(cefix::hexColorToVec4("000") == osg::Vec4(0,0,0,1));
	REQUIRE(cefix::hexColorToVec4("#00000000") == osg::Vec4(0,0,0,0));
	REQUIRE(cefix::hexColorToVec4("#FF00FF00") == osg::Vec4(1,0,1,0));
	
	REQUIRE(cefix::hexToDec("0x0") == 0);
	REQUIRE(cefix::hexToLong("0xA2700041261A3") == 2857630788903331ULL);
	REQUIRE(cefix::hexToLong("A2700041261A3") == 2857630788903331ULL);
	REQUIRE(cefix::hexToDec("gasas") == 0);
	
}

TEST_CASE( "StringUtils/intToString", "intToString" )
{

	REQUIRE(cefix::intToString(4) == "4");
	REQUIRE(cefix::intToString(-1000) == "-1000");
	REQUIRE(cefix::intToString(12345678) == "12345678");
	REQUIRE(cefix::doubleToString(12345.0) == "12345.0");
	REQUIRE(cefix::doubleToString(12345.5) == "12345.5");
	
}

TEST_CASE( "StringUtils/strTrim", "strTrim" )
{

	REQUIRE(cefix::strTrim("  Hallo  ") == "Hallo");
	REQUIRE(cefix::strTrim("  \thuhu\t  ") == "huhu");
	REQUIRE(cefix::strTrim(L"  \n\rblubb\n\r") == L"blubb");
	REQUIRE(cefix::strTrim("  \n\r\t\n\r") == "");
	REQUIRE(cefix::strTrim("  \n\r\t\n\rdummy") == "dummy");
	REQUIRE(cefix::strTrim("dummy  \n\r\t\n\r") == "dummy");
}


TEST_CASE( "StringUtils/Unicode", "unicode" )
{
    
	REQUIRE(cefix::Unicode::UTF8toWstring("höäü") == L"höäü");
    REQUIRE(cefix::Unicode::wstringToUTF8(L"höäü") == "höäü");
    
    REQUIRE(cefix::Unicode::UTF8Decompose("ääöüÄÖÜ") == "ääöüÄÖÜ");
    REQUIRE(cefix::Unicode::UTF8Compose("ääöüÄÖÜ") == "ääöüÄÖÜ");
    
	
}

TEST_CASE( "StringUtils/Unicode", "unicode" )
{
	
	std::vector<std::string> result;
	
	cefix::strSplit("1|2|3|4|5|6|7", result, '|');
	REQUIRE(result.size() == 7);
	
	result.clear();
	cefix::strSplit("1||3||5|6|7", result, '|');
	REQUIRE(result.size() == 7);
	REQUIRE(result[0] == std::string("1"));
	REQUIRE(result[6] == std::string("7"));
	
	result.clear();
	cefix::strSplit("1|2|3|4|5|6|7|", result, '|');
	REQUIRE(result.size() == 8);
	REQUIRE(result[0] == std::string("1"));
	REQUIRE(result[6] == std::string("7"));
	REQUIRE(result[7] == std::string(""));
	
	result.clear();
	cefix::strSplit("|1|2|3|4|5|6|7", result, '|');
	REQUIRE(result.size() == 8);
	REQUIRE(result[0] == std::string(""));
	REQUIRE(result[1] == std::string("1"));
	REQUIRE(result[7] == std::string("7"));
	
	{
		std::vector<int> vec;
		vec.push_back(1);
		vec.push_back(2);
		vec.push_back(3);
		REQUIRE(cefix::strJoin(vec, ",") ==  "1,2,3");
	}
}


TEST_CASE( "StringUtils/toHexString", "toHexString" )
{
	REQUIRE("0x123456AA" == cefix::longToHexString(0x123456AA));
	REQUIRE("0xABCD" == cefix::intToHexString(0xABCD));
}


