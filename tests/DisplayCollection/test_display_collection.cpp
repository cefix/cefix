/*
 *  test_appframework.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 08.11.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */ 

#include <cefix/AppController.h>
#include <cefix/DataFactory.h>
#include <osgDB/ReadFile>		
#include <cefix/LineGridGeometry.h>
#include <cefix/EnumUtils.h>
#include <osgDB/WriteFile>
#include <cefix/TextGeode.h>
#include <cefix/SysUtils.h>


class TestDisplayCollectionAppController : public cefix::AppController {

	public:
		enum TestEnum {TEST_1, TEST_2, TEST_3, TEST_4, TEST_5, TEST_6} ;
		TestDisplayCollectionAppController();
		virtual void init();
		osg::Group* createWorld();

};

TestDisplayCollectionAppController::TestDisplayCollectionAppController()
:	cefix::AppController()
{

}


void TestDisplayCollectionAppController::init() 
{
	cefix::log::info("main") << "init" << std::endl;
	
	osg::ref_ptr<cefix::DisplayCollection> dp = new cefix::DisplayCollection();
		
	int mode = 2;
	
	
	switch(mode) 
	{ 
		case 0:
			{	// 1 Fullscreen-window
				cefix::WindowConfiguration* conf = cefix::WindowConfiguration::createFullScreen(0);
				dp->addWindowConfiguration(conf);
			}
			break;
		
		case 1:
			{
				// 1 centered window on screen 0
				cefix::WindowConfiguration* conf = cefix::WindowConfiguration::createCenteredWindowOnScreen(0, 800, 400);
				dp->addWindowConfiguration(conf);
			}
			break;
		
		case 2:
			{	// 1 window on screen 0
				cefix::WindowConfiguration* conf = cefix::WindowConfiguration::createWindowOnScreen(0, 100, 100, 400, 600);
				cefix::Display* main_display = conf->createStandardDisplay();
				main_display->setDisplayMaskController(new cefix::DisplayMaskController());
				main_display->getDisplayMaskController()->setEditable(true);
				
				conf->addDisplay(main_display);
				conf->setWindowRotation(cefix::WindowRotation::TILTED_RIGHT);
				
				dp->addWindowConfiguration(conf);
			}
			break;
		
		case 3:
			{
				// two windows
				cefix::WindowConfiguration* conf = cefix::WindowConfiguration::createWindowOnScreen(0, 100, 100, 400, 400);
				dp->addWindowConfiguration(conf);
				
				cefix::WindowConfiguration* second_win_conf = cefix::WindowConfiguration::createWindowOnScreen(0, 500, 100, 600, 400);
				second_win_conf->addDisplay(second_win_conf->createStandardDisplay("second_display"));
				dp->addWindowConfiguration(second_win_conf);
			}
			break;
			
		case 4:
			{
				// two windows from file
				dp->loadConfigurationFromFile("two_windows_example.xml");
			}
			break;
		
		case 5:
			{
				// 1 centered window on screen 0
				cefix::WindowConfiguration* conf = cefix::WindowConfiguration::createCenteredWindowOnScreen(0, 800, 600);
				cefix::Display* main_display = conf->createStandardDisplay();
				main_display->setViewport(0, 200, 800, 400);
				main_display->set2DConfiguration(cefix::Display::Configuration2D(800,400));
				conf->addDisplay(main_display);
				
				// zweites Display im gleichen Fenster
				cefix::Display* display = new cefix::Display("second_display");
				display->setViewport(0,0,800,200);
				display->set2DConfiguration(cefix::Display::Configuration2D(800,200));
				display->set3DConfiguration(cefix::Display::Configuration3D(60, -1, 1, 1000));
				conf->addDisplay(display);
				
				
				dp->addWindowConfiguration(conf);
			}
			break;
			
		case 6:
			{
				// 1 centered window on screen 0
				cefix::WindowConfiguration* conf = cefix::WindowConfiguration::createCenteredWindowOnScreen(0, 800, 600);
				cefix::Display* main_display = conf->createStandardDisplay();
				main_display->set2DConfiguration(cefix::Display::Configuration2D(800,400));
				main_display->setViewport(0, 200, 800, 400);
				conf->addDisplay(main_display);
				{
					// zweites Display im gleichen Fenster, aber als Slave
					cefix::Display* display = new cefix::Display("slave_display_1");
					display->setViewport(0,0,200,200);
					display->setIsSlave(true);
					display->set2DConfiguration(cefix::Display::Configuration2D(200,200));
					display->set3DConfiguration(cefix::Display::Configuration3D(60, -1, 1, 1000));
					display->setSlaveConfiguration(osg::Matrix::scale(2,1,1),osg::Matrix::identity(), false);
					
					conf->addDisplay(display);
				}
				{
					// drittes Display im gleichen Fenster, aber nicht als Slave
					cefix::Display* display = new cefix::Display("slave_display_2");
					display->setViewport(200,0,200,200);
					display->setIsSlave(false);
					display->set2DConfiguration(cefix::Display::Configuration2D(200,200));
					display->set3DConfiguration(cefix::Display::Configuration3D(60, -1, 1, 1000));
					
					conf->addDisplay(display);
				}
				{
					// drittes Display im gleichen Fenster, aber als Slave
					cefix::Display* display = new cefix::Display("slave_display_3");
					display->setViewport(400,0,400,200);
					display->setIsSlave(true);
					display->set2DConfiguration(cefix::Display::Configuration2D(400,200));
					display->set3DConfiguration(cefix::Display::Configuration3D(60, -1, 1, 1000));
					
					conf->addDisplay(display);
				}
				
				dp->addWindowConfiguration(conf);
			}
			break;
			
		case 7:
			{
				// jetzt wirds kompliziert, hier haben wir ein Setup, wo jedes Fenster ein Ausschnitt aus einem größeren Viewport ist, 
				// z.b. mehrere Monitore auf einer Fläche, wobei z.b. die Stege zwischen den Monitoren berücksichtigt werden.
				
				// erstmal unser master-display, das die "komplett-ansicht" beschreibt
				unsigned int w, h;
				osg::GraphicsContext::getWindowingSystemInterface()->getScreenResolution(0,w,h);
				cefix::Display* main_display = new cefix::Display("main");
				main_display->setViewport(0,0,w, h);
				main_display->set2DConfiguration(w, h);
				main_display->set3DConfiguration(cefix::Display::Configuration3D(60, -1, 1, 1000));
				dp->addDisplay(main_display);
				
				bool use_slaves = true;
				bool use_deco = true;
				// jetzt die vier Fenster-Konfigurationen erzeugen
				
				{
					cefix::WindowConfiguration* win_conf = cefix::WindowConfiguration::createWindowOnScreen(0, 100, 100, 350, 350);
					win_conf->addDisplay(new cefix::TiledDisplay("first_display", "main", 100,100, 350, 350, use_slaves));
					win_conf->getTraits()->windowDecoration = use_deco;
					win_conf->setTitle("1");
					dp->addWindowConfiguration(win_conf);
				}
				
				{
					cefix::WindowConfiguration* win_conf = cefix::WindowConfiguration::createWindowOnScreen(0, 500,100, 650, 350);
					win_conf->addDisplay(new cefix::TiledDisplay("second_display", "main", 500, 100, 650, 350, use_slaves));
					win_conf->getTraits()->windowDecoration = use_deco;
					win_conf->setTitle("2");
					dp->addWindowConfiguration(win_conf);
				}
				
				{
					cefix::WindowConfiguration* win_conf = cefix::WindowConfiguration::createWindowOnScreen(0, 550, 500, 650, 350);
					win_conf->addDisplay(new cefix::TiledDisplay("third_display", "main", 550,500, 650, 350, use_slaves));
					win_conf->getTraits()->windowDecoration = use_deco;
					win_conf->setTitle("3");
					dp->addWindowConfiguration(win_conf);
				}
				
				{
					cefix::WindowConfiguration* win_conf = cefix::WindowConfiguration::createWindowOnScreen(0, 100, 470, 350, 350);
					win_conf->addDisplay(new cefix::TiledDisplay("fourth_display", "main", 100, 470, 350, 350, use_slaves));
					win_conf->getTraits()->windowDecoration = use_deco;
					win_conf->setTitle("4");
					dp->addWindowConfiguration(win_conf);
				}

				
			}
			break;
		case 8:
			{
				// load case 7 from file
				dp->loadConfigurationFromFile("four_tiled_slaves.xml");
			}
			break;
		case 9:
			{
				// create 1 display with custom frustum, update eye-pos via mouse:
				
				cefix::WindowConfiguration* conf = cefix::WindowConfiguration::createCenteredWindowOnScreen(0, 500, 500);
				cefix::Display* display = conf->createStandardDisplay("main");
				conf->addDisplay(display);
				
				osg::Vec3 top_left		( -1,  1,  -1);
				osg::Vec3 bottom_left	( -1, -1,  -1);
				osg::Vec3 bottom_right	(  1, -1,  -1);
				osg::Vec3 eye(0.90,0.3,0);
				
				display->set3DConfiguration(cefix::Display::Configuration3D(top_left, bottom_left, bottom_right, 1,1000));
				
				dp->setEyePosition(eye);
				
				dp->addWindowConfiguration(conf);
				
			}
			break;
		
		case 10:
			{
				// erstmal unser master-display, das die "komplett-ansicht" beschreibt
				unsigned int w, h;
				osg::GraphicsContext::getWindowingSystemInterface()->getScreenResolution(0,w,h);
				cefix::Display* main_display = new cefix::CustomProjectionDisplay("main", osg::Matrix::identity());
				main_display->set2DConfiguration(w, h);
				dp->addDisplay(main_display);
			
				
				{
					cefix::WindowConfiguration* conf = cefix::WindowConfiguration::createWindowOnScreen(0, 100, 100, 400, 800);
					cefix::Display* display = new cefix::Display("left");
					conf->addDisplay(display);
					display->setViewport(0,0,400, 800);
					
					osg::Vec3 top_left		( -1,  1,  -1);
					osg::Vec3 bottom_left	( -1, -1,  -1);
					osg::Vec3 bottom_right	(  0, -1,  -1);
					display->setIsSlave(true, "main");
					
					display->set3DConfiguration(cefix::Display::Configuration3D(top_left, bottom_left, bottom_right, 1,1000));
					dp->addWindowConfiguration(conf);
				}
				
				{
					cefix::WindowConfiguration* conf = cefix::WindowConfiguration::createWindowOnScreen(0, 500, 100, 800, 800);
					cefix::Display* display = new cefix::Display("right");
					conf->addDisplay(display);
					display->setViewport(0,0,800, 800);
					
					osg::Vec3 top_left		( 0,  1,  -1);
					osg::Vec3 bottom_left	( 0, -1,  -1);
					osg::Vec3 bottom_right	( 2, -1,  -0.5);
					display->setIsSlave(true, "main");
					
					display->set3DConfiguration(cefix::Display::Configuration3D(top_left, bottom_left, bottom_right, 1,1000));
					dp->addWindowConfiguration(conf);
				}
				
				
								
			}
			
			break;
		
		case 11:
			{
				// erstmal unser master-display, das die "komplett-ansicht" beschreibt
				unsigned int w, h;
				osg::GraphicsContext::getWindowingSystemInterface()->getScreenResolution(0,w,h);
				cefix::Display* main_display = new cefix::CustomProjectionDisplay("main", osg::Matrix::identity());
				main_display->set2DConfiguration(w, h);
				dp->addDisplay(main_display);
				
				bool use_deco(false);
				bool use_vsync(false);
				bool share_context(false);
				
				// eye-point setzen
				dp->setEyePosition(osg::Vec3(0,0.5, 0.5));
				
				
				// die sechs seiten eines würfels
				{
					cefix::WindowConfiguration* win_conf = cefix::WindowConfiguration::createWindowOnScreen(0, 100, 450, 350, 350);

					cefix::Display* display = new cefix::Display("left_display");
					display->setViewport(0,0,350, 350);
					display->setIsSlave(true, "main");
					display->set3DConfiguration(cefix::Display::Configuration3D(osg::Vec3(-1,1,1), osg::Vec3(-1,-1,1), osg::Vec3(-1,-1,-1),1,1000));
					win_conf->addDisplay(display);
					win_conf->getTraits()->windowDecoration = use_deco;
					win_conf->getTraits()->vsync = use_vsync;
					win_conf->setTitle("left");
					win_conf->setShareContextWithMainWindow(share_context);
					dp->addWindowConfiguration(win_conf);
				}
				{
					cefix::WindowConfiguration* win_conf = cefix::WindowConfiguration::createWindowOnScreen(0, 450, 450, 350, 350);
					cefix::Display* display = new cefix::Display("back_display");
					display->setViewport(0,0,350, 350);
					display->setIsSlave(true, "main");
					display->set3DConfiguration(cefix::Display::Configuration3D(osg::Vec3(-1,1,-1), osg::Vec3(-1,-1,-1), osg::Vec3(1,-1,-1),1,1000));
					win_conf->addDisplay(display);
					win_conf->getTraits()->windowDecoration = use_deco;
					win_conf->getTraits()->vsync = use_vsync;
					win_conf->setTitle("back");
					win_conf->setShareContextWithMainWindow(share_context);
					dp->addWindowConfiguration(win_conf);
				}
				{
					cefix::WindowConfiguration* win_conf = cefix::WindowConfiguration::createWindowOnScreen(0, 800, 450, 350, 350);
					cefix::Display* display = new cefix::Display("right_display");
					display->setViewport(0,0,350, 350);
					display->setIsSlave(true, "main");
					display->set3DConfiguration(cefix::Display::Configuration3D(osg::Vec3(1,1,-1), osg::Vec3(1,-1,-1), osg::Vec3(1,-1,1),1,1000));
					win_conf->addDisplay(display);
					win_conf->getTraits()->windowDecoration = use_deco;
					win_conf->getTraits()->vsync = use_vsync;
					win_conf->setTitle("right");
					win_conf->setShareContextWithMainWindow(share_context);
					dp->addWindowConfiguration(win_conf);
				}
				{
					cefix::WindowConfiguration* win_conf = cefix::WindowConfiguration::createWindowOnScreen(0, 1150, 450, 350, 350);
					cefix::Display* display = new cefix::Display("front_display");
					display->setViewport(0,0,350, 350);
					display->setIsSlave(true, "main");
					display->set3DConfiguration(cefix::Display::Configuration3D(osg::Vec3(1,1,1), osg::Vec3(1,-1,1), osg::Vec3(-1,-1,1),1,1000));
					win_conf->addDisplay(display);
					win_conf->getTraits()->windowDecoration = use_deco;
					win_conf->getTraits()->vsync = use_vsync;
					win_conf->setTitle("front");
					win_conf->setShareContextWithMainWindow(share_context);
					dp->addWindowConfiguration(win_conf);
				}
				
				{
					cefix::WindowConfiguration* win_conf = cefix::WindowConfiguration::createWindowOnScreen(0, 450, 800, 350, 350);
					cefix::Display* display = new cefix::Display("bottom_display");
					display->setViewport(0,0,350, 350);
					display->setIsSlave(true, "main");
					display->set3DConfiguration(cefix::Display::Configuration3D(osg::Vec3(-1,-1,-1), osg::Vec3(-1,-1,1), osg::Vec3(1,-1,1),1,1000));
					win_conf->addDisplay(display);
					win_conf->getTraits()->windowDecoration = use_deco;
					win_conf->getTraits()->vsync = use_vsync;
					win_conf->setTitle("bottom");
					win_conf->setShareContextWithMainWindow(share_context);
					dp->addWindowConfiguration(win_conf);
				}
				
				{
					cefix::WindowConfiguration* win_conf = cefix::WindowConfiguration::createWindowOnScreen(0, 450, 100, 350, 350);
					cefix::Display* display = new cefix::Display("top_display");
					display->setViewport(0,0,350, 350);
					display->setIsSlave(true, "main");
					display->set3DConfiguration(cefix::Display::Configuration3D(osg::Vec3(-1,1,1), osg::Vec3(-1,1,-1), osg::Vec3(1,1,-1),1,1000));
					win_conf->addDisplay(display);
					win_conf->getTraits()->windowDecoration = use_deco;
					win_conf->getTraits()->vsync = use_vsync;
					win_conf->setTitle("top");
					win_conf->setShareContextWithMainWindow(share_context);
					dp->addWindowConfiguration(win_conf);
				}
				
			}
			
			break;
			
        case 12:
			{
				// load case 11 from file
				dp->loadConfigurationFromFile("cubemap.xml");
                break;
			}
            
        case 13:
			{
				// load case 11 from preferences.xml
				dp->readConfigurationFromPrefs("/viewer/displays_inline");
                break;
			}
        case 14:
			{
				// load case 11 from preferences.xml
				dp->readConfigurationFromPrefs("/viewer/displays");
                break;
			}
			

	}
		
	if (mode >=0) {
		setDisplayCollection(dp);
	}
	dp->saveConfigurationToFile("displays.xml");
	
}

osg::Group* TestDisplayCollectionAppController::createWorld()
{
	std::cout << "ui scale: " << cefix::SysUtils::getUIScale() << std::endl;
    
    osg::Group* g = new osg::Group();
	osg::Node* cow = osgDB::readNodeFile("lines.osg");
	g->addChild(cow);
	
	cefix::LineGridGeometry* geo = new cefix::LineGridGeometry(osg::Vec3(2000 / 20,2000 / 20, 0), osg::Vec3(20,20,0));
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(geo);
	get2DLayer();
	
	if (0 && cefix::DisplayCollection::instance()->hasDisplay("second_display")) 
	{
		cefix::Display* display = cefix::DisplayCollection::instance()->getDisplay("second_display");
		display->get3DWorld()->addChild(g);
		osg::Node *scene = display->getWindow()->getSceneData();
		std::cout << scene << std::endl;
		osgDB::writeNodeFile(*scene, "second_window_data.osg");
	}
	
	// jedem Display noch nen Text, ne kuh und ein gitter mitgeben
	
	for(cefix::DisplayCollection::iterator i = cefix::DisplayCollection::instance()->begin(); i != cefix::DisplayCollection::instance()->end(); ++i)
	{
		cefix::Display* display = i->second;
		cefix::Utf8TextGeode* tg = new cefix::Utf8TextGeode("system.xml", 8, display->getIdentifier());
		
		tg->setPosition(osg::Vec3(50,10,0));
		tg->setTextColor(osg::Vec4(1,1,0,1));
		tg->refresh();
		display->get2DWorld()->addChild(tg);
		display->get2DWorld()->addChild(geode);
		display->get3DWorld()->addChild(cow);
	}
	
	return g;
}



#pragma mark - 

int main(int argc, char *argv[])
{
	osg::setNotifyLevel(osg::DEBUG_INFO);
	cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	osg::ref_ptr<TestDisplayCollectionAppController> app = new TestDisplayCollectionAppController();
	app->setUseOptimizerFlag(false);
	//app->setThreadingModel(osgViewer::ViewerBase::DrawThreadPerContext);
	// create the world and apply it
    app->applyWorld(app->createWorld());

    app->requestPicking();
    //app->requestPostScriptWriter();
    
    //create a viewer-window/fullscreen
    app->realize();
	
	/*
	for(cefix::DisplayCollection::iterator i = cefix::DisplayCollection::instance()->begin(); i != cefix::DisplayCollection::instance()->end(); ++i)
	{
		cefix::Display* display = i->second;
		osgDB::writeNodeFile(*display->get3DCamera(), display->getIdentifier()+".dot");
		osgDB::writeNodeFile(*display->get3DCamera(), display->getIdentifier()+".osg");
	}
	*/
	app->run();
	app = NULL;
	
	return 0;
}
