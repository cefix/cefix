/*
 *  TestSerializer.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 28.10.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include <cefix/Serializer.h>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <map>
#include <cefix/StringUtils.h>

#include "catch.hpp"


struct ThirdTestClass {

ThirdTestClass() : f(10), i(20), d(30) {}

	float f;
	int i;
	double d;
	void readFrom(cefix::Serializer& s) 
	{
			s >> "f" >> f;
			s >> "i" >> i;
			s >> "d" >> d;
			unsigned int k;
			s >> "k" >> k;
	}
	
	void writeTo(cefix::Serializer& s) 
	{
			s << "f" << f;
			s << "i" << i;
			s << "d" << d;
	}
};


class SecondTestClass {
public:
	float f;
	int i;
	double d;
	std::vector<ThirdTestClass> vec;
	
	SecondTestClass() {
		vec.resize(10);
	}
	
	void readFrom(cefix::Serializer& s) 
	{
			s >> "f" >> f;
			s >> "i" >> i;
			s >> "d" >> d;
			unsigned int k;
			s >> "k" >> k;
			s >> "vec" >> vec;
	}
	
	void writeTo(cefix::Serializer& s) 
	{
			s << "f" << f;
			s << "i" << i;
			s << "d" << d;
			s << "vec" << vec;
			
	}
};


class TestClass {

public:
	TestClass()
	{
	}
	
	void readFrom(cefix::Serializer& s) 
	{
		using cefix::Serializer;
		int i;
		
		s >> "int" >>		cefix::Serializer::DefaultValue<int>(_int, 2);
		s >> "double" >>	cefix::Serializer::defaultValue(_double, 3.145);
		s >> "float" >>		cefix::Serializer::defaultValue(_float, 1.234f);
		s >> "string" >>	cefix::Serializer::defaultValue(_string, "haha");
		s >> "grumpel" >> i;
		s >> "vector" >> _vector;
		s >> "set" >> _set;
		s >> "map" >> _map;
		s >> "multimap" >> _multimap;
		s >> "multiset" >> _multiset;
		s >> "v3" >> s.defaultValue(_v3, osg::Vec3(1,2,3));
		s >> "v2" >> s.defaultValue(_v2, osg::Vec2(1,2));
		
	}
	
	void writeTo(cefix::Serializer& s) 
	{
		
		s << "int" << _int;
		s << "double" << _double;
		s << "float" << _float;
		s << "string" << _string;
		s << "vector" << _vector;
		s << "set" << _set;
		s << "map" << _map;
		s << "multimap" << _multimap;
		s << "multiset" << _multiset;
		s << "v3" << _v3;
		s << "v2" << _v2;
		
	}
	
	void createSTL() {
	
		for(unsigned int i = 0; i < 50; ++i) {
			_vector.push_back( SecondTestClass() );
			_set.insert(osg::Vec3(i,i,i));
			_multiset.insert(osg::Vec3(i,i,i));
			_map[cefix::intToString(i)] = SecondTestClass() ;
			_multimap.insert(std::make_pair(i, osg::Vec4(i,i,i,i)));
		}
		
		_v3.set(2,4,6);
		_v2.set(2,4);
	
	}

	int _int;
	double _double;
	float _float;
	std::string _string;
	std::vector<SecondTestClass> _vector;
	std::set<osg::Vec3> _set;
	std::multiset<osg::Vec3> _multiset;
	std::map<std::string, SecondTestClass> _map;
	std::multimap<unsigned int, osg::Vec4> _multimap;
	osg::Vec3 _v3;
	osg::Vec2 _v2;
};


TEST_CASE( "Serializer/serializer", "serializer" )
{
	
	
	{	
		TestClass r, r2, w;
		
		// im 1. durchgang defaultvalues testen
		
		osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
		{
			cefix::Serializer s(pl);
			s >> "test" >> r;
			w = r;
		}
		
		REQUIRE(w._int == r._int);
		REQUIRE(w._double == r._double);
		REQUIRE(w._float == r._float);
		REQUIRE(w._string == r._string);
		REQUIRE(w._v3 == r._v3);
		REQUIRE(w._v2 == r._v2);
		
		
		w.createSTL();
		
		// daten schreiben
		{
			cefix::Serializer s(pl);
			s << "test" << w;
			s.getPropertyList()->saveAsXML("unittest_serializer.xml");
		}
				
		// daten wieder lesen
		{
			cefix::Serializer s(pl);
			s.setNotifyLevel(osg::ALWAYS);
			s >> "test" >> r;
		}
		
		
		// vergleichen
		
		REQUIRE(w._int == r._int);
		REQUIRE(w._double == r._double);
		REQUIRE(w._float == r._float);
		REQUIRE(w._string == r._string);

		
		REQUIRE(w._vector.size() == r._vector.size());
		REQUIRE(w._vector[0].vec.size() == r._vector[0].vec.size());
		REQUIRE(w._set.size() == r._set.size());
		REQUIRE(w._multiset.size() == r._multiset.size());
		REQUIRE(w._map.size() == r._map.size());
		REQUIRE(w._multimap.size() == r._multimap.size());
		
		REQUIRE(w._v3 == r._v3);
		REQUIRE(w._v2 == r._v2);
		
		REQUIRE(r._int == 2);
		REQUIRE(r._double == 3.145);
		REQUIRE(r._float == 1.234f);
		REQUIRE(r._string == "haha");
		REQUIRE(r._v3 == osg::Vec3(2,4,6));
		REQUIRE(r._v2 == osg::Vec2(2,4));
		
		{
			cefix::Serializer s(new cefix::PropertyList("unittest_serializer.xml"));
			s >> "test" >> r2;
		}
		
		REQUIRE(w._int == r2._int);
		REQUIRE(w._double == r2._double);
		REQUIRE(w._float == r2._float);
		REQUIRE(w._string == r2._string);

		
		REQUIRE(w._vector.size() == r2._vector.size());
		REQUIRE(w._vector[0].vec.size() == r2._vector[0].vec.size());
		REQUIRE(w._set.size() == r2._set.size());
		REQUIRE(w._multiset.size() == r2._multiset.size());
		REQUIRE(w._map.size() == r2._map.size());
		REQUIRE(w._multimap.size() == r2._multimap.size());
		
		REQUIRE(w._v3 == r2._v3);
		REQUIRE(w._v2 == r2._v2);
		
		REQUIRE(r2._int == 2);
		REQUIRE(r2._double == 3.145);
		REQUIRE(r2._float == 1.234f);
		REQUIRE(r2._string == "haha");
		REQUIRE(r2._v3 == osg::Vec3(2,4,6));
		REQUIRE(r2._v2 == osg::Vec2(2,4));


	}
	
	
		
}

