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

#ifndef CEFIX_FLICKR_DATE_HEADER
#define CEFIX_FLICKR_DATE_HEADER

#include <string>
#include <sstream>

namespace cefix {

class CantParseDateException : public std::exception {

	public:
		CantParseDateException(const std::string& str) : std::exception(), _str(str) {}
		
		virtual const char* what() const throw() {
			return std::string("Can't parse date from string " + _str).c_str();
		}
		virtual ~CantParseDateException() throw() {}
	private:
		std::string _str;
};

class Date {

	public:
		Date();
		
		Date(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second, bool isDaylightSavingTime = false);	
		
		Date(const Date& d) : _time(d._time) { update(); }
		
		bool operator<(const Date& d) const { return _time < d._time; }
		bool operator<=(const Date& d) const { return _time <= d._time; }
		bool operator>(const Date& d) const { return _time > d._time; }
		bool operator>=(const Date& d) const { return _time >= d._time; }
		
		bool operator==(const Date& d) const { return _time == d._time; }
		bool operator!=(const Date& d) const { return _time != d._time; }
		
		const Date& operator+=(int secs) { _time += secs; update(); return *this; }
		const Date& operator-=(int secs) { _time -= secs; update(); return *this; }
		
		
		void now();
		
		inline unsigned int getYear() const { return _year; }
		inline unsigned int getMonth() const { return _month; }
		inline unsigned int getDay() const { return _day; }
		inline unsigned int getHour() const { return _hour; }
		inline unsigned int getMinute() const { return _min; }
		inline unsigned int getSecond() const { return _sec; }
		
		inline void addSeconds( int secs) { _time += secs; update(); }
		inline void addMinutes(int v) { addSeconds(v*60); }
		inline void addHours(int v) {addSeconds(v*60*60); }
		static Date getFromUnixTimeStamp(time_t t) { return Date(t); }
		static Date getFromUnixTimeStamp(const std::string& date) throw (CantParseDateException);
		static Date getFromMySQL(const std::string& date) throw (CantParseDateException);
		
		std::string getAsMySQL() const;
		std::string getAsUnixTimeStampString() const { std::ostringstream ss; ss << _time; return ss.str(); }
		const time_t& getAsUnixTimeStamp() const { return _time; }
		
		bool isDaylightSavingTime() const { return _isDaylightSavingTime; }
		
	private:
		explicit Date(time_t secs) : _time(secs) { update(); }
	
	private:
		
		void update();
		
		time_t _time;
		unsigned int _year, _month, _day, _hour, _min, _sec;
		bool _isDaylightSavingTime;

};

std::ostream& operator<<(std::ostream& os, const Date& date);

}

#endif