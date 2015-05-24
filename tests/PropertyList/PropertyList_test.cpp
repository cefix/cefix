/*
 *  PropertyList.cpp
 *  CommonTools
 *
 *  Created by Stephan Huber on 16.04.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/Log.h>
#include <osg/io_utils>
#include <cefix/PropertyList.h>
#include <cefix/md5.h>
#include <cefix/FileUtils.h>
#include <cefix/JSON.h>


int main(int argc, char *argv[])
{
    
	osg::setNotifyLevel(osg::DEBUG_INFO);
	
	std::string s("test");
	cefix::MD5 context;
	context.update((unsigned char*)s.c_str(), s.length());
	context.finalize ();
	
	std::cout << s << ": " << context << std::endl;
	
    osg::ref_ptr<cefix::PropertyList> list = new cefix::PropertyList();
	
	list->add("ainteger", 10);
	list->add("float", 10.0f);
	list->add("string", "ein test");
	list->add("double", 10.0);
	list->add("object", new osg::Referenced());
	osg::notify(osg::ALWAYS) << list->get("not found")->asInt() << std::endl;
	list->add("vec3", osg::Vec3(1.123456789,2,4));
	list->add("vec2", osg::Vec2(1,5.123456789));
	list->add("vec23", osg::Vec2(4,5));
	list->add("vec2", osg::Vec2(7.123456789,5));
	list->add("vec4", osg::Vec4(1,2,4,29.123456789));
    list->add("quat", osg::Quat(osg::PI, osg::Vec3(1,1,1)));
	list->add("matrix", osg::Matrix::rotate( osg::Quat(osg::PI, osg::Vec3(1,1,1)) ));
	list->add("rect", osg::Vec4(100,100,300,400))->setXmlType("rect");
	list->add("point", osg::Vec2(100,100))->setXmlType("point");
	list->add("color", osg::Vec4(1.0,0.5,0.1,1.0))->setXmlType("color");
	list->add("symbol", "my_symbol")->setXmlType("symbol");
	list->add("vector", osg::Vec3(100,200,3000.11))->setXmlType("vector");
	list->add("size", osg::Vec2(300, 100))->setXmlType("size");
	osg::notify(osg::ALWAYS) << list->get("vec3")->asVec3() << std::endl;
	//+
	list->dump();
	std::cout << "////////////////////////////////////////////////////////" << std::endl;
	
	// ersetzen von werten
	list->set("vec4", osg::Vec4(10,20,30,40));
	list->set("vec2", "ich bin nun ein string");
	list->remove("ainteger");
	list->add("integer", 20);
	list->dump();
	
	osg::ref_ptr<cefix::PropertyList> list2 = new cefix::PropertyList();
	 
	list2->add("integer", 20);
	list2->add("float", 20.0f);
	list2->add("string", "noch ein test");
	list2->add("double", 20.0);
	
	list->add("list2", list2.get());
    list->add("string_test", "eine zeile\nnoch eine zeile\n  noch eine zeile");
	
	list->dump();
    
    list->saveAsXML("/Users/stephan/Desktop/test.xml", "test");
    
	{
		osg::ref_ptr<cefix::PropertyList> xmllist = new cefix::PropertyList();
		xmllist->loadFromXML("/Users/stephan/Desktop/test.xml");
		xmllist->dump();
	}
	
	{
		list->saveAsXML("/Users/stephan/Desktop/original.xml");
		std::string json = cefix::JSON::stringifyToUTF8("root", list);
		std::cout << json << std::endl;
		
		osg::ref_ptr<cefix::AbstractProperty> prop = cefix::JSON::parse(json.c_str());
		cefix::PropertyList* pl = prop->asPropertyList();
		pl->dump(); 
		pl->saveAsXML("/Users/stephan/Desktop/from_json.xml");
		pl->saveAsJSON("/Users/stephan/Desktop/from_json.json");
		
		std::cout << *prop << std::endl;
	}
    
    {
        osg::ref_ptr<cefix::PropertyList> json_pl = new cefix::PropertyList();
        json_pl->loadFromJSON("/Users/stephan/Desktop/json_input.json");
        json_pl->saveAsJSON("/Users/stephan/Desktop/json_output.json");
    }
	
	{
		cefix::PropertyList* pl = new cefix::PropertyList("cubemap.xml");
		pl->saveAsJSON("/Users/stephan/Desktop/cubemap.json");
	}
    
    {
		cefix::PropertyList* pl = new cefix::PropertyList();
        pl->loadFromJSON("structure.json");
        pl->dump();
		pl->saveAsJSON("/Users/stephan/Desktop/structure_2.json");
	}
	
	std::cout << (*list) << std::endl;
	
	return 0;
    
#ifdef __APPLE__
	std::string file = "/Users/stephan/Desktop/test_preferences.xml";
    file ="/Users/stephan/Documents/Projekte/cefix/apps/AudiHeritage/media/index.xml";
#else
	std::string file = "D:/developing/audi_iaa/q7+quattro/preferences.xml";
#endif
	
	osg::ref_ptr<cefix::PropertyList> xmllist = new cefix::PropertyList();
	xmllist->loadFromXML(file);
	xmllist->dump();
    cefix::AbstractProperty* p = xmllist->find("/year/1899/overview/stream/2/image/1");
    if (p)
        osg::notify(osg::ALWAYS) << "key found: " << p->asString() << std::endl;
    else
        osg::notify(osg::ALWAYS) << "key NOT found "  << std::endl;
	xmllist->saveAsXML("/Users/stephan/Desktop/test2.xml", "test");
	
	list = NULL;
	xmllist = NULL;
	
	while(0) {
		//osg::ref_ptr<cefix::PropertyList> l = new cefix::PropertyList();
		//l->loadFromXML(file);

	}
	
	return 0;
}

