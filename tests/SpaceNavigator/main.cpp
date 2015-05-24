//
//  main.c
//  cefixTemplate
//
//  Created by Stephan Huber on 26.11.06.
//  Copyright __MyCompanyName__ 2006. All rights reserved.
//
#include <cefix/InputDeviceManager.h>
#include "SpaceNavigatorInputDevice.h"
#include "SpaceNavigatorManipulator.h"

#include <cefix/AppController.h>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>

#include <cefix/DataFactory.h>

class TestInputDevice : public cefix::InputDevice {

	public:
		TestInputDevice() : cefix::InputDevice(identifier()) { setNeedsPolling(true); }
		static std::string identifier() { return "testInputDevice"; }
		
		virtual bool available() { return true; }
		
		virtual void poll() {
			std::cout << "TestInputDevice::poll" << std::endl;
		}
		
	protected:
		virtual bool openImplementation() {
			std::cout << "TestInputDevice::openImplementation" << std::endl;
			return true;
		}
		
		virtual void closeImplementation() {
			std::cout << "TestInputDevice::closeImplementation" << std::endl;
		}
};




class MyAppController: public cefix::AppController {

	public:
		osg::Group * createWorld() {
		
			osg::Group* g = new osg::Group();
			/*
			osg::Geode* geode = new osg::Geode();
			geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0,0,0), 1)));
			g->addChild(geode);
			*/
			g->addChild(osgDB::readNodeFile("cow.osg"));
			
			return g;
		}

};


int main(int argc, char* argv[])
{
    osg::setNotifyLevel(osg::INFO);
		
	cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	osg::ref_ptr<MyAppController> app = new MyAppController();
	
	cefix::InputDeviceManager::instance()->registerDevice(new TestInputDevice());
	cefix::InputDeviceManager::instance()->openDevice(TestInputDevice::identifier());
	
	cefix::InputDeviceManager::instance()->openDevice(cefix::SpaceNavigatorInputDevice::identifier());
	
	cefix::SpaceNavigatorInputDevice* snid = dynamic_cast<cefix::SpaceNavigatorInputDevice*>(cefix::InputDeviceManager::instance()->getInputDevice(cefix::SpaceNavigatorInputDevice::identifier()));
	if (snid)
		snid->setEventMask(cefix::SpaceNavigatorInputDevice::ALL_BUTTONS | cefix::SpaceNavigatorInputDevice::AXIS_TZ | cefix::SpaceNavigatorInputDevice::AXIS_ROTATION);
	
	// create the world and apply it
    app->applyWorld(app->createWorld());

    //app->requestPicking();
    //app->requestPostScriptWriter();
    
    //create a viewer-window/fullscreen
    app->realize();
	app->getViewer()->getKeySwitchMatrixManipulator()->addMatrixManipulator('.',"SpaceNavigator", new cefix::SpaceNavigatorManipulator());
	app->run();

	return 0;
}


