/*
 *  ImageFactory_test.cpp
 *  SimpleGLUTViewer
 *
 *  Created by Stephan Huber on Tue Nov 18 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */
 
 #define __USE_OSX_AGL_IMPLEMENTATION__
#include <sstream>

#include <cefix/Log.h>
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Material>
#include <osg/Depth>
#include <osg/Stateset>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <cefix/TextGeode.h>
#include <osg/Autotransform>
#include <osg/io_utils>



#include <cefix/PickEventHandler.h>
#include <cefix/MathUtils.h>
#include <cefix/MouseLocationProvider.h>
#include <cefix/Quad2DGeometry.h>
#include <cefix/FileUtils.h>
#include <cefix/TextGeode.h>
#include <cefix/StringUtils.h>
#include <cefix/PickEventHandler.h>
#include <cefix/ColorUtils.h>
#include <cefix/DebugGeometryFactory.h>

#include "Sketch.h"

class TestTouchable : public cefix::Pickable, public osg::Referenced {
public:
	TestTouchable(cefix::Quad2DGeometry* geo,  cefix::Utf8TextGeode* tg, bool pass_event, bool persistent)
	:	cefix::Pickable(),
		osg::Referenced(),
		_geo(geo),
		_tg(tg),
        _passEvent(pass_event)
	{
	}
	
	virtual void down()
	{
		_status += "began " + cefix::intToString(getLastPointerId())+"\n";
		_geo->setColor(osg::Vec4(0, 1,0,0.2));
        if (!_passEvent)
            stopPropagation();
        
	}
	
    virtual void within()
	{
		_status += "moved " + cefix::intToString(getLastPointerId())+"\n";
		//_geo->setColor(osg::Vec4(0.5,0.5,0.5,0.2));
        if (!_passEvent)
            stopPropagation();
    }
	
	virtual void up(bool inside)
	{
		_status += "ended " + cefix::intToString(getLastPointerId())+"\n";
		_geo->setColor(osg::Vec4(1,0,0,0.2));
        if (!_passEvent)
            stopPropagation();
    }
	
	virtual void handle() 
	{
		
        std::string s("");
        s += std::string(_passEvent ? "PASS" : "DO NOT PASS");
        
        _tg->setText(s + " / "+ cefix::intToString(getNumPointers()) + " touches\n"+_status);
        _status = "";
        
        /*if(getNumTouchesEnded() == getNumTouchPoints()) {
            _tg->setText("");
        }*/
        
	}
    virtual void sequenceBegan()
    {
        _status += "sequence began\n";
    }
    
    virtual void sequenceEnded()
    {
        _status += "sequence ended\n";
    }

private:
	cefix::Quad2DGeometry* _geo;
	cefix::Utf8TextGeode* _tg;
	std::string _status;
    bool _passEvent;

};


Sketch::Sketch() 
    : cefix::Sketch()
{
    allowOsgHandler(true);
    getMainWindow()->setMultiTouchEnabled(true);
    
}

void Sketch::setup() 
{    
    getMainWindow()->requestPicking();
    
    osg::Geode* geode = new osg::Geode();
    
    for(unsigned int i = 0; i < 10; ++i) {
    
        cefix::Quad2DGeometry* geo = new cefix::Quad2DGeometry(-50, -50, 100, 100);
        geode->addDrawable(geo);
        geo->setColor(osg::Vec4(1,1,0,0.2));
        geo->setLocZ(999);
        _quads.push_back(geo);
        
        cefix::Utf8TextGeode* tg = new cefix::Utf8TextGeode("system.xml", 8);
        tg->setTextColor(osg::Vec4(1,1,1,0.7));
        get2DLayer()->addChild(tg);
        _tgs.push_back(tg);
        
    }
    
    get2DLayer()->addChild(geode);
    float w = getWidth() /3;
    float h = getHeight() / 3;
    for(int y = 0; y < 3; ++y) {
        for(int x = 0; x < 3; ++x) {
            for(int i= 0; i < 4; ++i) {
                cefix::Utf8TextGeode* tg = new cefix::Utf8TextGeode("system.xml", 8);
                tg->setPosition(osg::Vec3(x * w+10, y*h + h - 20-(i*50), 10+i*20));
                tg->setTextColor(osg::Vec4(1,1,1,0.3));
                get2DLayer()->addChild(tg);
                
                osg::Geode* geode =  new osg::Geode();
                cefix::Quad2DGeometry* geo = new cefix::Quad2DGeometry(x*w, y*h, w, h-(i*50));
                geo->setLocZ(1+i*20);
                geo->setColor(osg::Vec4(1,1,1,0.2));
                geode->addDrawable(geo);
                get2DLayer()->addChild(geode);
                geode->setUserData(new TestTouchable(geo, tg, (i%2==0), (i == 0)));
            }
        }
    }
    
    cefix::DebugGeometryFactory::enable("multi_touch");
    get2DLayer()->addChild(cefix::DebugGeometryFactory::get("multi_touch")->getOrCreateNode());
}

void Sketch::update() 
{
    /*
     cefix::TouchPickEventHandler* h = getMainWindow()->getWindowPickable()->getTouchPickEventHandler();
    if (h) 
    {
        std::ostringstream o;
        unsigned num_tp(0);
        float height(getMainWindow()->getHeight());
        for(cefix::TouchPickEventHandler::TouchDataById::iterator i = h->getTouchData().begin(); i != h->getTouchData().end(); ++i) 
        {
            osg::Vec4 color = cefix::rgb(0x00ffff);
            o << "ID: " << i->first << "/" << i->second.id << " P: " << i->second.phase << " X/Y: " << i->second.x << "/" << i->second.y << "num touchables: " << h->getTouchablesForId(i->second.id).size() << std::endl;
            switch(i->second.phase) 
            {
                case osgGA::GUIEventAdapter::TOUCH_BEGAN:
                    color = cefix::rgb(0x00ff00);
                    break;
                case osgGA::GUIEventAdapter::TOUCH_ENDED:
                    color = cefix::rgb(0xff0000);
                    break;
                default:
                    break;
            };
            
            cefix::DebugGeometryFactory::get("multi_touch")->addRect(osg::Vec4(i->second.x-15, height - i->second.y-15, i->second.x+15, height - i->second.y+15), color);
            num_tp++;
        }
        o << std::endl << "num touchables: " << h->getCurrentTouchables().size() << std::endl;
        
        
        if (h->getRawTouchData()) {
            osgGA::GUIEventAdapter::TouchData* td = h->getRawTouchData();
            unsigned int i;
            for(i = 0; i < td->getNumTouchPoints(); ++i)
            {
                const osgGA::GUIEventAdapter::TouchData::TouchPoint& tp = td->get(i);
                                
                osg::Vec4 color = cefix::rgb(0x00ffff);
                o << "raw: " << tp.id << " " << tp.phase << " " << tp.x << "/" << tp.y << std::endl;
                switch(tp.phase) 
                {
                    case osgGA::GUIEventAdapter::TOUCH_BEGAN:
                        color = cefix::rgb(0x00ff00);
                        break;
                    case osgGA::GUIEventAdapter::TOUCH_ENDED:
                        color = cefix::rgb(0xff0000);
                        break;
                    default:
                        break;
                }
                
                _quads[i]->setCenter(tp.x, getMainWindow()->getHeight() - tp.y);
                _tgs[i]->setPosition(osg::Vec3(tp.x + 60, getMainWindow()->getHeight() - tp.y, 100));
                _tgs[i]->setText("ID: "+cefix::intToString(tp.id)+"\nPHASE: " +cefix::intToString(tp.phase));
                cefix::DebugGeometryFactory::get("multi_touch")->addRect(osg::Vec4(tp.x-10, height - tp.y-10, tp.x+10, height - tp.y+10), color);
            }
            for(unsigned int j = i; j < 10; ++j) {
                _quads[j]->setCenter(-100,-100);
                _tgs[j]->setPosition(osg::Vec3(-100,-100,0));
            }
        }
        
        cefix::DebugGeometryFactory::get("multi_touch")->addText(osg::Vec2(10, getMainWindow()->getHeight()-20), o.str(), osg::Vec4(1,1,1,1));
        
        if(num_tp > 0) 
            getMainWindow()->requestWarpPointer(100,100);
    }
     */
}

	


