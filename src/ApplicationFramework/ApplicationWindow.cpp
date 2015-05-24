/*
 *  AppControllerView.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 08.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <osgDB/WriteFile>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <cefix/ViewerEventHandlers.h>

#include "ApplicationWindow.h"
#include <cefix/DataFactory.h>
#include <cefix/Timer.h>

#include <cefix/PickEventHandler.h>
#include <osgGA/MultiTouchTrackballManipulator>


#ifdef WIN32
#include <osgViewer/api/Win32/GraphicsWindowWin32>
#endif
#ifdef __APPLE__
#ifdef CEFIX_FOR_IPHONE
#else
#include <osgViewer/api/Cocoa/GraphicsWindowCocoa>
#endif
#endif

namespace cefix {


class DumpAllEventHandler: public osgGA::GUIEventHandler
{
public:
	DumpAllEventHandler() : osgGA::GUIEventHandler() {}
	
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		if (ea.getEventType() != osgGA::GUIEventAdapter::FRAME)
		{
			std::cout << &aa << " " << ea.getEventType() << " " << ea.getX() << "/" << ea.getY() << std::endl;
		}
		return false;
	}

private:


};


/*



// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

ApplicationWindow::ApplicationWindow(const std::string& prefskey): 
	osgViewer::View(),
	_fullscreen(false),
	_prefsKey(prefskey),
	_hasTrackballManipulator(true)
{
	_rotation = WindowRotation::UNKNOWN;
	
	preInit();
	initWithViewPointManipulatorWhenValid(true);
	
	
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

	traits->width				=  DataFactory::instance()->getPreferencesFor(prefskey+"/width", 800);
	traits->height				=  DataFactory::instance()->getPreferencesFor(prefskey+"/height", 600);
	traits->screenNum			=  DataFactory::instance()->getPreferencesFor(prefskey+"/screenNum", 1) -1;
	traits->x					= DataFactory::instance()->getPreferencesFor(prefskey+"/left", -1);
	traits->y					= DataFactory::instance()->getPreferencesFor(prefskey+"/top", -1);
	traits->windowDecoration  	= DataFactory::instance()->getPreferencesFor(prefskey+"/Decoration", true);
	traits->windowName			=  DataFactory::instance()->getPreferencesFor(prefskey+"/name", std::string("cefix Render Window"));
	traits->supportsResize		= DataFactory::instance()->getPreferencesFor(prefskey+"/ResizeAllowed", false);
	traits->samples				=  DataFactory::instance()->getPreferencesFor(prefskey+"/fsaa", 0);
	traits->useMultiThreadedOpenGLEngine = DataFactory::instance()->getPreferencesFor(prefskey+"/multithreadedOpenGL", false);
	traits->vsync				= DataFactory::instance()->getPreferencesFor(prefskey+"/vsync", true);
	_useCursor = traits->useCursor	= DataFactory::instance()->getPreferencesFor(prefskey+"/showMouseCursor", true);
	
	
		
	if ((traits->x < 0) || (traits->y < 0)) 
	{
		osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
        
		if (wsi == NULL) {
			osg::notify(osg::WARN) << "ApplicationWindow :: no windowingSystemInterface found, aborting ..." << std::endl;
			exit(-1);
		}
		unsigned int screen_width, screen_height;
		wsi->getScreenResolution(*traits, screen_width, screen_height);
		
		if (traits->x < 0) traits->x = (screen_width - traits->width) / 2;
		if (traits->y < 0) traits->y = (screen_height - traits->height) / 2;
	}
		
	float nearplane				=  DataFactory::instance()->getPreferencesFor(prefskey+"/perspective/nearplane", 1.0f);
	float farplane				=  DataFactory::instance()->getPreferencesFor(prefskey+"/perspective/farplane", 1000.0f);
	float hfov					=  DataFactory::instance()->getPreferencesFor(prefskey+"/perspective/horizontalFieldOfView", -1.0f);
	float vfov					=  DataFactory::instance()->getPreferencesFor(prefskey+"/perspective/verticalFieldOfView", -1.0f);

		
		
	// FSAA
	
	if (traits->samples)
		traits->sampleBuffers = 1; 
	
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	
	if (DataFactory::instance()->getPreferencesFor(prefskey+"/adjustScreenSize", false) &&
		DataFactory::instance()->getPreferencesFor(prefskey+"/fullscreen", false) ) 
	{
		traits->x = 0;
		traits->y = 0;
		osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
		if (wsi) {
			osg::notify(osg::INFO) << "adjusting screen to " << traits->width << "x" << traits->height << std::endl;
			wsi->setScreenResolution(*traits, traits->width, traits->height);
		}
    
	}
	
	setCamera( createView(traits.get()) );
	
	WindowRotation w_rot(_rotation);
	w_rot.setViewPort(0,0, getWidth(), getHeight());
	getCamera()->setProjectionMatrix(w_rot.computeProjectionMatrix(hfov, vfov, nearplane, farplane));
	
	// Licht
	osg::View::LightingMode lightingmode = osg::View::NO_LIGHT;
	if (cefix::DataFactory::instance()->getPreferencesFor(prefskey+"/light/sky", false) )
		lightingmode = osg::View::SKY_LIGHT;
	
	if (cefix::DataFactory::instance()->getPreferencesFor(prefskey+"/light/head", true) )
		lightingmode = osg::View::HEADLIGHT;	
	
	setLightingMode(lightingmode);
	// noch standardmanipulatoren hinzufügen
	
	if (cefix::DataFactory::instance()->getPreferencesFor(prefskey+"/options/stats", true) ) 
    {
		osgViewer::StatsHandler* statshandler = new osgViewer::StatsHandler();
		addEventHandler(statshandler);
	}
	
    if (cefix::DataFactory::instance()->getPreferencesFor(prefskey+"/options/state", true) ) 
    {
		addEventHandler(new osgGA::StateSetManipulator(getCamera()->getOrCreateStateSet()));
    }
	
	if (cefix::DataFactory::instance()->getPreferencesFor(prefskey+"/options/viewer", true) ) {
		cefix::ViewerEventHandler* viewereventhandler = new cefix::ViewerEventHandler();
		addEventHandler(viewereventhandler);
	}
	addEventHandler(new cefix::DefaultEventHandler());	
	
	init();

}


*/

// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

ApplicationWindow::ApplicationWindow(WindowConfiguration* config) : 
	osgViewer::View(), 
	_fullscreen(false),
	_configuration(config),
	_hasTrackballManipulator(true),
	_isMultiTouchEnabled(false)
{
	#ifdef CEFIX_FOR_IPHONE
		_isMultiTouchEnabled = true;
	#endif
	
	preInit();
	initWithViewPointManipulatorWhenValid(true);
	
	createView(config);
    setFullscreen(config->isFullScreen());
	
	if (config->hasStatsHandler()) 
    {
		osgViewer::StatsHandler* statshandler = new osgViewer::StatsHandler();
		addEventHandler(statshandler);
	}
	
    if (config->hasStateHandler() && getCamera() ) 
    {
		addEventHandler(new osgGA::StateSetManipulator(getCamera()->getOrCreateStateSet()));
    }
	
	if (config->hasViewerHandler() ) {
		cefix::ViewerEventHandler* viewereventhandler = new cefix::ViewerEventHandler();
		addEventHandler(viewereventhandler);
	}

	init();
}


ApplicationWindow::ApplicationWindow(float w, float h, WindowRotation::Rotation rot) : 
	osgViewer::View(), 
	_fullscreen(false),
	_configuration(NULL),
	_hasTrackballManipulator(true)
{
	preInit();
	initWithViewPointManipulatorWhenValid(true);
	
	_width = w;
	_height = h;
	initWithViewPointManipulatorWhenValid(true);
	osg::Camera* camera = new osg::Camera();
	WindowRotation w_rot(rot);
	w_rot.setViewport(0,0, w, h);
	camera->setProjectionMatrix(w_rot.computeProjectionMatrix(-1, -1, 1, 1000));
	setCamera(camera);
	
	osgViewer::StatsHandler* statshandler = new osgViewer::StatsHandler();
	addEventHandler(statshandler);
	
	init();
}


/*
ApplicationWindow::ApplicationWindow(int left, int top, int width, int height, cefix::WindowRotation::Rotation rotation) :
	osgViewer::View(),
	_fullscreen(false),
	_rotation(rotation),
	_hasTrackballManipulator(true)
{
	preInit();
	initWithViewPointManipulatorWhenValid(true);
	
	
	osg::GraphicsContext::Traits* traits = new osg::GraphicsContext::Traits();
	
	traits->width				=  width;
	traits->height				=  height;
	traits->x					=  left;
	traits->y					=  top;
	traits->windowDecoration  	=  true;
	traits->windowName			=  std::string("cefix Render Window");
	traits->supportsResize		=  0;
	traits->samples				=  0;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->vsync = false;
		
	if ((traits->x < 0) || (traits->y < 0)) 
	{
		osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
        
		unsigned int screen_width, screen_height;
		wsi->getScreenResolution(*traits, screen_width, screen_height);
		
		if (traits->x < 0) traits->x = (screen_width - traits->width) / 2;
		if (traits->y < 0) traits->y = (screen_height - traits->height) / 2;
	}
	
	setCamera(createView(traits));
	
	WindowRotation w_rot(_rotation);
	w_rot.setViewPort(0,0, getWidth(), getHeight());
	
	float nearplane				=  1.0f;
	float farplane				=  1000.0f;
	float hfov					=  -1.0f;
	float vfov					=  -1.0f;
	
	getCamera()->setProjectionMatrix(w_rot.computeProjectionMatrix(hfov, vfov, nearplane, farplane));
	
	init();
}

ApplicationWindow::ApplicationWindow(osgViewer::GraphicsWindow* graphicsWindow) : 
	osgViewer::View(),
	_fullscreen(false),
	_rotation(WindowRotation::NONE),
	_hasTrackballManipulator(true)
{
	preInit();
	initWithViewPointManipulatorWhenValid(true);
	
	
	osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	camera->setGraphicsContext(graphicsWindow);
	const osg::GraphicsContext::Traits* traits = graphicsWindow->getTraits();
	_left = traits->x;
	_top = traits->y;
	_width = traits->width;
	_height = traits->height;
	
	_window = graphicsWindow;
	
	camera->setViewport(new osg::Viewport(0,0, traits->width, traits->height));
	GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	camera->setDrawBuffer(buffer);
	camera->setReadBuffer(buffer);
	
	setCamera(camera.get());
	init();
}


ApplicationWindow::ApplicationWindow(float w, float h, WindowRotation::Rotation rotation) : 
	osgViewer::View(),
	_fullscreen(false),
	_window(NULL),
	_rotation(rotation),
	_hasTrackballManipulator(true)
{
	_width = w;
	_height = h;
	initWithViewPointManipulatorWhenValid(true);
	osg::Camera* camera = new osg::Camera();
	WindowRotation w_rot(_rotation);
	w_rot.setViewPort(0,0, w, h);
	camera->setProjectionMatrix(w_rot.computeProjectionMatrix(-1, -1, 1, 1000));
	setCamera(camera);
	init();
}

*/

ApplicationWindow::~ApplicationWindow()
{

}

void ApplicationWindow::preInit()
{
	_useCursor = true;
	_hasCameraManipulator = true;
    _pickEventHandler = NULL;
}

// ----------------------------------------------------------------------------------------------------------
// createView
// ----------------------------------------------------------------------------------------------------------

void ApplicationWindow::createView(WindowConfiguration* data) 
{
	cefix::log::info("ApplicationWindow") << "createView" << std::endl;
	
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(*data->getTraits());

	if (traits->x  == -1) {
		unsigned int screenWidth, screenHeight;
		osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
		wsi->getScreenResolution(*traits, screenWidth, screenHeight);
		
		traits->x = osg::maximum<int>(0, (screenWidth-traits->width) / 2);
		traits->y = osg::maximum<int>(0, (screenHeight-traits->height) / 2);
	}

	_left = traits->x;
	_top = traits->y;
	_width = traits->width;
	_height = traits->height;
	
	WindowRotation w_rot(data->getWindowRotation());
	w_rot.setViewport(0,0, _width, _height);
	traits->width  = w_rot.getWidth();
	traits->height = w_rot.getHeight();
	
	_decoration = traits->windowDecoration;
	_useCursor = traits->useCursor;
	
	osg::ref_ptr<osg::Camera> camera(NULL);
	camera = new osg::Camera();
	GLenum buffer = traits->doubleBuffer ? GL_BACK : GL_FRONT;
	camera->setDrawBuffer(buffer);
	camera->setReadBuffer(buffer);
	
	
	osg::ref_ptr<osg::Group> root = new osg::Group();
	osg::ref_ptr<osg::GraphicsContext> gc(NULL);
	
	gc = osg::GraphicsContext::createGraphicsContext(traits);
	if (!gc.valid())
	{
		osg::notify(osg::NOTICE)<<"  GraphicsWindow has not been created successfully."<<std::endl;
	}

	_window = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());
	if (_window && _window->getEventQueue() && _window->getEventQueue()->getCurrentEventState())
        _window->getEventQueue()->getCurrentEventState()->setWindowRectangle(traits->x, traits->y, traits->width, traits->height );
	
    if(getEventQueue() && getEventQueue()->getCurrentEventState())
        getEventQueue()->getCurrentEventState()->setWindowRectangle(traits->x, traits->y, traits->width, traits->height );
    
	for(unsigned int i = 0; i < data->getNumDisplays(); ++i) 
	{
		Display* display = data->getDisplay(i);
		if (display->isMaster()) 
		{
			display->applyTo(camera);
			camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
			root->addChild(display->get3DWorld());
			root->addChild(display->get2DCamera());
		} 
		else if (display->isSlave()) 
		{
			if ((display->asTiledDisplay() || (data->getNumDisplays() == 1)))
			{
				display->applyTo(camera);
				camera->setReferenceFrame(osg::Transform::RELATIVE_RF);

				root->addChild(display->get3DWorld());
				root->addChild(display->get2DCamera());
				_hasCameraManipulator = false;
			} 
			else {
				display->applyTo(new osg::Camera());
				display->get3DCamera()->setGraphicsContext(gc.get());
				
				osg::Group* g = new osg::Group();
				g->addChild(display->get3DWorld());
				g->addChild(display->get2DCamera());
				display->get3DCamera()->addChild(g);
				
				addSlave(display->get3DCamera(), display->getSlaveProjectionOffset(), display->getSlaveViewOffset(),display->getSlaveUseMasterSceneData());	
			}
			
		}
		else {
			display->applyTo(new osg::Camera());
			display->get3DCamera()->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
			
			root->addChild(display->get3DCamera());
			osg::Group* g = new osg::Group();
			g->addChild(display->get3DWorld());
			g->addChild(display->get2DCamera());
			display->get3DCamera()->addChild(g);
		}
	}
	
	if (camera) camera->setGraphicsContext(gc.get());
	setCamera(camera);
	if (root->getNumChildren() > 0) setSceneData(root);
	    
	setMultiTouchEnabled(_isMultiTouchEnabled || data->isMultiTouchEnabled());
}

void ApplicationWindow::setMultiTouchEnabled(bool b) 
{
	_isMultiTouchEnabled = b;
#ifdef __APPLE__
#ifdef CEFIX_FOR_IPHONE
#else
	osgViewer::GraphicsWindowCocoa* win = getGraphicsWindow() ? dynamic_cast<osgViewer::GraphicsWindowCocoa*> (getGraphicsWindow()) : NULL;
		if (win)
			win->setMultiTouchEnabled(_isMultiTouchEnabled);
	
#endif
#endif
}

void ApplicationWindow::setWindowRectangle(int l, int t, int w, int h) 
{	
	if (l < 0) {
		unsigned int screenWidth, screenHeight;
		osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
		wsi->getScreenResolution(*_window->getTraits(), screenWidth, screenHeight);
		
		l = osg::maximum<int>(0, (screenWidth-w) / 2);
		t = osg::maximum<int>(0, (screenHeight-h) / 2);
	}
	
	_left = l;
	_top = t;
	int temp_w = _width = w;
	int temp_h = _height = h;
	
	if (_windowRealized) {
		WindowRotation w_rot(_configuration->getWindowRotation());
		if (w_rot.isTilted())
			std::swap(temp_w,temp_h);
		getGraphicsWindow()->setWindowRectangle(_left, _top, temp_w, temp_h);
	}
}

// ----------------------------------------------------------------------------------------------------------
// createHud
// ----------------------------------------------------------------------------------------------------------

osg::Camera* ApplicationWindow::createHudCamera(osg::Group* camera_childs, int aw, int ah, const osg::Matrix& offset) {
	int w = (aw == 0) ? getStoredWidth() : aw;
	int h =  (ah == 0) ? getStoredHeight() : ah;
	
	WindowRotation w_rot(_configuration->getWindowRotation());
	osg::ref_ptr<osg::Camera> cam = w_rot.createHudCamera(w,h, offset);
	if (camera_childs) {
		cam->addChild(camera_childs);
		
		osg::StateSet* stateset = camera_childs->getOrCreateStateSet();
		stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
		stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		stateset->setRenderBinMode(osg::StateSet::OVERRIDE_RENDERBIN_DETAILS);
	}
	return cam.release();
}



// ----------------------------------------------------------------------------------------------------------
// setFullscreen
// ----------------------------------------------------------------------------------------------------------

void ApplicationWindow::setFullscreen(bool flag) 
{
	if (!_windowRealized) { _fullscreen = flag; return; }
	if (_fullscreen == flag) return;
	_fullscreen = flag;
	int x, y;
	unsigned int w, h;
	bool decoration;

	if (_fullscreen) {
		osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
		wsi->getScreenResolution(*_window->getTraits(), w, h);
		x = y = 0;
		decoration = false;
	} else {
		x = _left;
		y = _top;
		w = _width;
		h = _height;
		WindowRotation w_rot(_configuration->getWindowRotation());
		if (w_rot.isTilted())
			std::swap(w,h);
			
		decoration = _decoration;
	}
	
	//getCamera()->setViewport(new osg::Viewport(0,0, w,h));
	_window->setWindowDecoration(decoration);
	_window->setWindowRectangle(x, y, w, h);
	
}




void ApplicationWindow::setSceneData(osg::Group* group) 
{
	if (group && _window.valid() && _window->getTraits()->sampleBuffers) {
        //FSAA aktivieren
        #ifndef GL_MULTISAMPLE_ARB
        #define GL_MULTISAMPLE_ARB                0x809D
        #endif
        group->getOrCreateStateSet()->setMode(GL_MULTISAMPLE_ARB, osg::StateAttribute::ON); 
		//_2dLayer->getOrCreateStateSet()->setMode(GL_MULTISAMPLE_ARB, osg::StateAttribute::OFF);
    }
		
	osgViewer::View::setSceneData(group);
}


void ApplicationWindow::init() 
{
	_viewpointManipulator = new ViewPointManipulator();
    if (isMultiTouchEnabled())
        _trackballManipulator = new osgGA::MultiTouchTrackballManipulator();
    else
        _trackballManipulator = new osgGA::TrackballManipulator();
	_windowRealized = false;
	
	// addEventHandler(new DumpAllEventHandler());
}

WindowPickable* ApplicationWindow::getWindowPickable() const 
{ 
	return _pickEventHandler->getWindowPickable(); 
}


void ApplicationWindow::realize() 
{
	if (_windowRealized) return;
	if (_window.valid()) _window->setCursor(osgViewer::GraphicsWindow::LeftArrowCursor);
	
	_windowRealized = true;
	
	if (_window.valid()) {
		if (_fullscreen) 
		{
			_fullscreen = false;
			setFullscreen(true);
		}
		else 
		{
			// TODO check windowRotation!
			int temp_w = _width;
			int temp_h = _height;
			WindowRotation w_rot(_configuration->getWindowRotation());
			if (w_rot.isTilted()) std::swap(temp_w, temp_h);
			if ((_left != _window->getTraits()->x) || 
				(_top != _window->getTraits()->y) ||
				(temp_w != _window->getTraits()->width) ||
				(temp_h != _window->getTraits()->height))
			{
				setWindowRectangle(_left, _top, _width, _height);
			}
		}
	}
	
	if (getCameraManipulator() == NULL && _hasCameraManipulator) 
	{
		if (_useViewPointManipulatorWhenValid && _viewpointManipulator->hasAttachedNode())
			setCameraManipulator(_viewpointManipulator.get());
		else
			if (hasTrackballManipulator())
				setCameraManipulator(getTrackballManipulator());

	}	
	osg::ref_ptr<osgGA::GUIEventAdapter> initEvent = _eventQueue->createEvent();
    initEvent->setEventType(osgGA::GUIEventAdapter::FRAME);
    
    if (getCameraManipulator())
    {
        getCameraManipulator()->init(*initEvent, *this);
    }
	
	
	// Cursor sicherheitshalber nochmals ausschalten falls gewünscht.
	if (_window.valid() && !_useCursor) {
		_window->useCursor(false);
		//cefix::Timer* t = cefix::FunctorTimer::create(1 * 1000, this, &ApplicationWindow::hideCursor,0);
		//t->start();
	}
	
	#ifdef WIN32
	if (_window.valid()) {
		osgViewer::GraphicsWindowWin32* win = dynamic_cast<osgViewer::GraphicsWindowWin32*>(_window.get());
		if (win) {
			BringWindowToTop(win->getHWND());
			SetForegroundWindow(win->getHWND());
			SetFocus(win->getHWND());
		}
	}
	#endif
}

void ApplicationWindow::requestPicking() 
{
	if (!_pickEventHandler.valid())
    {
        _pickEventHandler = new PickEventHandler(getCamera());
		_pickEventHandler->setMultiTouchEnabled(_isMultiTouchEnabled);
		
        getEventHandlers().push_front(_pickEventHandler.get());
	}
}


bool ApplicationWindow::exportSnapshot(const std::string& filename, unsigned int width, unsigned int height, bool saveTiles, bool withAlpha) 
{
	osg::Camera* cam = getCamera(); 
	if (!cam) 
		return false; 

	double left, right, bottom, top, zNear, zFar; 
	osg::Matrix old_projection = cam->getProjectionMatrix();
	cam->getProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zFar); 

	double ar = width / static_cast<double>(height); 
	double target_frustum_h = top - bottom; 
	double target_frustum_w = ar * target_frustum_h; 
	double target_left = -target_frustum_w * 0.5; 
	double target_right = target_frustum_w * 0.5; 
	double target_bottom = -target_frustum_h * 0.5; 
	double target_top = target_frustum_h * 0.5; 

	int vpwidth, vpheight,x,y;
	_window->getWindowRectangle(x,y,vpwidth, vpheight);
		
	double newbottom = target_bottom; 
	double newtop = newbottom + (target_top - target_bottom) * vpheight / height; 
	double newfrustum_h = newtop - newbottom; 
	int t = 0; 
	int s = 0; 

	typedef std::vector<osg::ref_ptr<osg::Image> > Image_list; 
	Image_list images; 
	
	osg::ref_ptr<osg::Image> composition;
	if (!saveTiles) {
		composition = new osg::Image();
		composition->allocateImage(width, height, 1, withAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE);
	}
	
	_window->makeCurrent();
    
    _window->runOperations();
    _window->swapBuffers();
	
	while (newbottom < target_top) 
	{ 
		double frame_height = vpheight; 

		if (newtop > target_top) {
			frame_height = height % vpheight; 
			if (frame_height == 0) frame_height = vpheight;
		}

		double newleft = target_left; 
		double newright = newleft + (target_right - target_left) * vpwidth / width; 
		double newfrustum_w = newright - newleft; 
		s = 0; 

		while (newleft < target_right) 
		{ 
			double frame_width = vpwidth; 

			if (newright > target_right) {
				frame_width = width % vpwidth;
				if (frame_width == 0) frame_width = vpwidth;
			}

			cam->setProjectionMatrixAsFrustum(newleft, newright, newbottom, newtop, zNear, zFar); 
			
            //std::cout << "l:" << newleft << " r: " << newright << " b:" << newbottom << " t:" << newtop << " n:" << zNear << " f:" << zFar << std::endl;
            std::cout << "fw: " << frame_width << " fh: " << frame_height << std::endl;
            
            _window->runOperations();
			_window->swapBuffers();
            
            _window->runOperations();
			_window->swapBuffers();
			
			if (frame_width > 0) {
				std::ostringstream oss; 
				oss.clear();
				oss.str("");
				oss << osgDB::getNameLessExtension(filename) << "_" << s << "_" << t << "." << osgDB::getFileExtension(filename); 
				osg::ref_ptr<osg::Image> img = new osg::Image; 
				
				img->readPixels(0, 0, frame_width, frame_height, withAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE); 
				
				if (saveTiles) 
				{ 
					if (!osgDB::writeImageFile(*img.get(), oss.str())) 
					{ 
						std::cerr << "Error writing " << oss.str() << "\n"; 
						_window->releaseContext();
						cam->setProjectionMatrix(old_projection);
						return false; 
					} 
				} 
				else {
					int yo = vpheight * t;
					int xo = vpwidth * s;
                    std::cout << "copy into " << xo << "/" << yo << std::endl;
					composition->copySubImage(xo, yo, 0, img.get()); 
				}
				
				++s; 
			}
			newleft += newfrustum_w; 
			newright += newfrustum_w; 
		} 

		newbottom += newfrustum_h; 
		newtop += newfrustum_h; 
		++t; 
	} 
	
	_window->releaseContext();
	cam->setProjectionMatrix(old_projection);
	bool result(true);


	if (!saveTiles) {		
		if (!osgDB::writeImageFile(*composition.get(), filename)) 
		{ 
			std::cerr << "Error writing mosaic " << filename << "\n"; 
			result = false; 
		} 
	}
	return result; 

}

} // end of namespace
