/*
 *  Date.cpp
 *  cefixFlickr
 *
 *  Created by Stephan Huber on 06.02.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Date.h"

#include <time.h>

namespace cefix {


Date::Date() {
	
	_time = time(NULL);
}


Date::Date(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second, bool isDaylightSavingTime) {
	
	struct tm * timeinfo;
	
	_time = time(NULL);
	
	timeinfo = ::localtime(&_time);
	timeinfo->tm_year = year - 1900;
	timeinfo->tm_mon = month - 1;
	timeinfo->tm_mday = day;
	timeinfo->tm_hour = hour;
	timeinfo->tm_min = minute;
	timeinfo->tm_sec = second;
	timeinfo->tm_isdst = isDaylightSavingTime;
	_time = mktime(timeinfo);
	update();
	
}


void Date::now() {
	_time = time(NULL);
	update();
}

void Date::update() {
	struct tm * timeinfo;
	timeinfo = localtime ( &_time );
	
	_year = timeinfo->tm_year + 1900;
	_month = timeinfo->tm_mon + 1;
	_day = timeinfo->tm_mday;
	_hour = timeinfo->tm_hour;
	_min = timeinfo->tm_min;
	_sec = timeinfo->tm_sec;
	_isDaylightSavingTime = (timeinfo->tm_isdst != 0);
}

Date Date::getFromUnixTimeStamp(const std::string& s) throw (CantParseDateException) {
	
	time_t t = atol(s.c_str());
	if (t > 0)
		return Date(t);
		
	throw CantParseDateException(s);
}


Date Date::getFromMySQL(const std::string& date) throw (CantParseDateException)
{
	if (date.size() == 10) { //only date
		return Date( 
			atoi(date.substr(0,4).c_str()), 
			atoi(date.substr(5,2).c_str()), 
			atoi(date.substr(8,2).c_str()), 
			0,0,0
		);
	}
	
	if (date.size() >= 19) { //only date
		return Date( 
			atoi(date.substr(0,4).c_str()), 
			atoi(date.substr(5,2).c_str()), 
			atoi(date.substr(8,2).c_str()), 
			atoi(date.substr(11,2).c_str()),
			atoi(date.substr(14,2).c_str()),
			atoi(date.substr(17,2).c_str())
		);
	}
	
	throw CantParseDateException(date);
}

std::string Date::getAsMySQL() const {

	char buf[255];
	struct tm * timeinfo;
	timeinfo = localtime ( &_time );
	
	int len = strftime(buf, 255, "%Y-%m-%d %H:%M:%S", timeinfo);
	return std::string(buf, len);
}



std::ostream& operator<<(std::ostream& os, const Date& date)
{
	os << date.getAsMySQL() << " (" << date.getAsUnixTimeStamp() << ")";
	return os;
}

}
