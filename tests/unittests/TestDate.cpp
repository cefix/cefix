/*
 *  TestDate.h
 *  cefixFlickr
 *
 *  Created by Stephan Huber on 06.02.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "catch.hpp"

#include <cefix/Date.h>


TEST_CASE( "Date/Date", "Date" )
 {
		
	cefix::Date d1(2007, 12, 01, 12, 1, 59);
	
	REQUIRE(d1.getYear() == 2007);
	REQUIRE(d1.getMonth() == 12);
	REQUIRE(d1.getDay() == 1);
	REQUIRE(d1.getHour() == 12);
	REQUIRE(d1.getMinute() == 1);
	REQUIRE(d1.getSecond() == 59);
	
	cefix::Date d2 = d1;
	
	REQUIRE(d2.getYear() == 2007);
	REQUIRE(d2.getMonth() == 12);
	REQUIRE(d2.getDay() == 1);
	REQUIRE(d2.getHour() == 12);
	REQUIRE(d2.getMinute() == 1);
	REQUIRE(d2.getSecond() == 59);
	REQUIRE(d1 == d2);
	
	cefix::Date d3(d1);
	
	REQUIRE(d3.getYear() == 2007);
	REQUIRE(d3.getMonth() == 12);
	REQUIRE(d3.getDay() == 1);
	REQUIRE(d3.getHour() == 12);
	REQUIRE(d3.getMinute() == 1);
	REQUIRE(d3.getSecond() == 59);
	REQUIRE(d1 == d3);
	
	d3.addHours(10);
	REQUIRE(d3.getHour() == 22);
	REQUIRE(d1 < d3);
	REQUIRE_FALSE(d1 > d3);
	
	d3.addHours(10);
	REQUIRE(d3.getHour() == 8);
	REQUIRE(d3.getDay() == 02);
	
	// 10 sekunden dazu;
	d3+=10;
	REQUIRE(d3.getSecond() == 9);
	REQUIRE(d3.getMinute() == 2);
}

TEST_CASE( "Date/Date Convert", "Date Convert" )	
{
	using namespace cefix;
	
	Date d(2006, 6, 24, 11, 38, 13);
	
	REQUIRE(Date::getFromMySQL("2006-06-24 11:38:13") == d);
	REQUIRE(Date::getFromMySQL("2006-06-24") == Date(2006,6,24, 0,0,0));
	REQUIRE(Date::getFromUnixTimeStamp("1151145493") == d);
	CHECK_THROWS(Date::getFromMySQL("weis der henker"));
	CHECK_THROWS(Date::getFromUnixTimeStamp("weis der henker"));
	
	Date d2(2000, 12, 12, 14, 03, 01);
	REQUIRE(d2.getAsMySQL() == "2000-12-12 14:03:01");
	REQUIRE(Date::getFromMySQL(d2.getAsMySQL()) == d2);
	
	REQUIRE(d.getAsUnixTimeStampString() == "1151145493");
	REQUIRE(d.getFromUnixTimeStamp(d.getAsUnixTimeStamp()) == d);
}
