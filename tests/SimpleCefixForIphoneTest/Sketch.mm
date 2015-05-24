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
#include <cefix/SysUtils.h>
#include <osg/Autotransform>
#include <osg/io_utils>



#include <cefix/PickEventHandler.h>
#include <cefix/MathUtils.h>
#include <cefix/MouseLocationProvider.h>
#include <cefix/Quad2DGeometry.h>
#include <cefix/Pickable.h>
#include <cefix/FileUtils.h>
#include <cefix/TextGeode.h>
#include <cefix/StringUtils.h>
#include <cefix/PickEventHandler.h>
#include <cefix/ColorUtils.h>
#include <cefix/DebugGeometryFactory.h>
#include <cefix/WidgetComposer.h>
#include <cefix/SoundManager.h>
#include <cefix/WidgetThemeManager.h>

#include "Sketch.h"

#import <UIKit/UIKit.h>


class PlaySoundResponder : public cefix::ButtonWidget::Responder {
public:
    PlaySoundResponder(const std::string& sound_file) : cefix::ButtonWidget::Responder(), _soundFile(sound_file) {}
    
    virtual void buttonPressed() {
        if (_sound == NULL) {
            _sound = cefix::SoundManager::readSoundFile(_soundFile);
        }
        if (_sound->isPlaying()) {
            _sound->pause();
        } else {
            _sound->play();
            _sound->seek(0);
        }
    }
private:
    std::string _soundFile;
    osg::ref_ptr<cefix::Sound> _sound;
};


class TestTouchable : public cefix::Pickable, public osg::Referenced {
public:
	TestTouchable(cefix::Quad2DGeometry* geo,  cefix::Utf8TextGeode* tg, bool pass_event, bool persistent)
	:	cefix::Pickable(),
		osg::Referenced(),
		_geo(geo),
		_tg(tg),
        _passEvent(pass_event)
	{
        if (_passEvent)
            allowPropagation();
        else
            stopPropagation();
	}
	
	virtual void down()
	{
		_status += "began " + cefix::intToString(getLastPointerId())+"\n";
		_geo->setColor(osg::Vec4(0, 1,0,0.2));
	}
	virtual void within()
	{
        _status += "within " + cefix::intToString(getLastPointerId())+"\n";
		_geo->setColor(osg::Vec4(0.4, 0.4, 0.4, 0.2));
    }

	
	virtual void up(bool inside)
	{
		_status += "ended " + cefix::intToString(getLastPointerId())+"\n";
		_geo->setColor(osg::Vec4(1,0,0,0.2));
	}
	
	virtual void handle() 
	{
		std::string s("");
        s += std::string(_passEvent ? "PASS" : "DO NOT PASS");
        /*
        if (persistenceRequested())
            s += " / PERSISTENT ";
        */
        _tg->setText(s + " / "+ cefix::intToString(getNumPointers()) + " touches\n"+_status);
		_status = "";
        
        unsigned int num_touch_ended = 0;
        for(unsigned int i=0;i < getNumPointers(); ++i) {
            if (getNthPointerData(i).phase == cefix::Pickable::PointerData::UP)
                num_touch_ended++;
        }
        
        if(num_touch_ended == getNumPointers())
            _tg->setText("");
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

void Sketch::init() {

	// Fenster-Konfiguration
	
	cefix::DisplayCollection* dc = new cefix::DisplayCollection();
    osg::ref_ptr<cefix::WindowConfiguration> win_conf;
    
    #if CEFIX_FOR_IPHONE	
		win_conf = cefix::WindowConfiguration::createFullScreen(0);
		
		osgViewer::GraphicsWindowIOS::WindowData* data = NULL;
		data = new osgViewer::GraphicsWindowIOS::WindowData(
			NULL, 
			osgViewer::GraphicsWindowIOS::WindowData::LANDSCAPE_LEFT_ORIENTATION |
            osgViewer::GraphicsWindowIOS::WindowData::LANDSCAPE_RIGHT_ORIENTATION|
            osgViewer::GraphicsWindowIOS::WindowData::PORTRAIT_ORIENTATION
		); 
		win_conf->getTraits()->inheritedWindowData = data;
        		
    #else
		win_conf = cefix::WindowConfiguration::createCenteredWindowOnScreen(0, 1024, 768);
        // win_conf->setWindowRotation(cefix::WindowRotation::TILTED_RIGHT);
    #endif
    win_conf->setMultiTouchEnabled(true);
    
    /*
     osg::ref_ptr<cefix::Display> display = win_conf->createStandardDisplay();
	display->set2DConfiguration(cefix::Display::Configuration2D(1024, 768));
    win_conf->addDisplay(display);
    */
    
    dc->addWindowConfiguration(win_conf);
	
	// dem AppController diese DisplayCollection übergeben
    setDisplayCollection(dc);
}


void Sketch::setup() 
{    
    cefix::WidgetThemeManager::instance()->loadThemeAndMakeCurrent("sw_widget_theme.xml");
    
    std::cout << "ui scale: " << cefix::SysUtils::getUIScale() << std::endl;
    //osg::setNotifyLevel(osg::INFO);
    
    getMainWindow()->requestPicking();
    
    createTouchTest();
    
    cefix::DebugGeometryFactory::enable("multi_touch");
    get2DLayer()->addChild(cefix::DebugGeometryFactory::get("multi_touch")->getOrCreateNode());
    
    
    createSoundTest();
    createDraggables();
    
    getMainWindow()->getCamera()->setClearColor(osg::Vec4(0.2,0.2,0.2,1.0));

}


class TestDraggable : public cefix::Draggable, public osg::Referenced {
public:
    TestDraggable(osg::MatrixTransform* mat, cefix::Quad2DGeometry* geo, int ndx)
        : cefix::Draggable()
        , osg::Referenced()
        , _mat(mat)
        , _geo(geo)
        , _ndx(ndx)
    {
        _color = osg::Vec4(cefix::randomf(1), cefix::randomf(1), cefix::randomf(1), 0.2);
        _geo->setColor(_color);
    }
    
    void down()
    {
        _color[3] = 1.0;
        _geo->setColor(_color);
        cefix::Draggable::down();
    }
    
    void up(bool inside)
    {
        _color[3] = 0.2;
        _geo->setColor(_color);
        cefix::Draggable::up(inside);
    }
    
    virtual bool dragStarted()
    {
        _start = _mat->getMatrix().getTrans();
        return true;
    }
    
    virtual bool drag(const osg::Vec3& delta)
    {
        //std::cout << "start " << _start << " delta " << delta << std::endl;
        osg::Vec3 v(_start + delta);
        v[3] = _ndx;
        _mat->setMatrix(osg::Matrix::translate(v));
        return true;
    }
    
private:
    osg::MatrixTransform* _mat;
    cefix::Quad2DGeometry* _geo;
    osg::Vec3 _start;
    osg::Vec4 _color;
    int _ndx;
};

void Sketch::createDraggables()
{
    float w = cefix::DisplayCollection::instance()->getDisplay("main")->get2DConfiguration().width / 5.0;
    float h = cefix::DisplayCollection::instance()->getDisplay("main")->get2DConfiguration().height / 5.0;
    unsigned int ndx(0);
    for(unsigned int y = 0; y < 5; ++y)
    {
        for(unsigned int x = 0; x < 5; ++x)
        {
            osg::MatrixTransform* mat = new osg::MatrixTransform();
            osg::Geode* geode = new osg::Geode();
            cefix::Quad2DGeometry* geo = new cefix::Quad2DGeometry(0,0,w,h);
            geode->addDrawable(geo);
            mat->addChild(geode);
            mat->setMatrix(osg::Matrix::translate(x * w, y * h, ndx));
            
            geode->setUserData(new TestDraggable(mat, geo, ndx));
            
            cefix::Utf8TextGeode* tg = new cefix::Utf8TextGeode("system.xml", 8, cefix::intToString(ndx));
            tg->setPosition(osg::Vec3(10,10,1));
            tg->setTextColor(osg::Vec4(1,1,1,1));
            mat->addChild(tg);
            get2DLayer()->addChild(mat);
            ndx++;
        }
    }
}

void Sketch::createTouchTest()
{
    osg::Geode* geode = new osg::Geode();
    
    for(unsigned int i = 0; i < 10; ++i)
    {
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
    
    if (0)
    {
        float w = getWidth() /3;
        float h = getHeight() / 3;
        for(int y = 0; y < 3; ++y)
        {
            for(int x = 0; x < 3; ++x)
            {
                for(int i= 0; i < 4; ++i)
                {
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
    }
}


void Sketch::createSoundTest()
{
    cefix::WidgetComposer composer(osg::Vec3(10,200,900), 150);
    composer.addButton("SOUND 1", new PlaySoundResponder("sound_1.wav"));
    composer.addButton("SOUND 2", new PlaySoundResponder("sound_2.wav"));
    composer.addButton("SOUND 3", new PlaySoundResponder("sound_3.wav"));
    
    _widgets = composer.getWidgets();
    get2DLayer()->addChild(_widgets->getNode());
}

void Sketch::debugTouches()
{
    std::ostringstream o;
    unsigned num_tp(0);
    int xxx, xxy, width, height;
    getMainWindow()->getGraphicsWindow()->getWindowRectangle(xxx, xxy, width, height);
    
    cefix::WindowPickable* wp = getMainWindow()->getPickEventHandler()->getWindowPickable();
    
    for(cefix::WindowPickable::iterator i = wp->begin(); i != wp->end(); ++i)
    {
        osg::Vec4 color = cefix::rgb(0x00ffff);
        
        o << "ID: " << i->first << "/" << i->second.id << " P: " << i->second.phase << " X/Y: " << i->second.x << "/" << i->second.y << std::endl;
        switch(i->second.phase) 
        {
            case cefix::Pickable::PointerData::DOWN:
                color = cefix::rgb(0x00ff00);
                break;
            case cefix::Pickable::PointerData::UP:
                color = cefix::rgb(0xff0000);
                break;
            default:
                break;
        };
        
        cefix::DebugGeometryFactory::get("multi_touch")->addRect(osg::Vec4(i->second.x-15, i->second.y-15, i->second.x+15, i->second.y+15), color);
        num_tp++;
    }
    o << std::endl << "num pickables: " << wp->getNumPickables() << std::endl;
    
    
    
    
    cefix::DebugGeometryFactory::get("multi_touch")->addText(osg::Vec2(10, getMainWindow()->getStoredHeight()-20), o.str(), osg::Vec4(1,1,1,1));
    
    if(num_tp > 0) 
        getMainWindow()->requestWarpPointer(100,100);
}
void Sketch::update() 
{
    
    debugTouches();
    
    UIView* parent_win = cefix::DisplayCollection::instance()->getDisplay("main")->getWindow()->getNativeWindow();
    UIView* parent_view = cefix::DisplayCollection::instance()->getDisplay("main")->getWindow()->getNativeView();
        
    NSLog(@"win  frame: %@ bounds: %@", NSStringFromCGRect(parent_win.frame), NSStringFromCGRect(parent_win.bounds));
    NSLog(@"view frame: %@ bounds: %@", NSStringFromCGRect(parent_view.frame), NSStringFromCGRect(parent_view.bounds));
    NSLog(@"class: %@", [parent_view className]);
}

	


