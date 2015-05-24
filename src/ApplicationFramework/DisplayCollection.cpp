/*
 *  DisplayCollection.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 21.04.10.
 *  Copyright 2010 Digital Mind. All rights reserved.
 *
 */

#include "DisplayCollection.h"
#include <cefix/AppController.h>
#include <cefix/DataFactory.h>
#include <cefix/EnumUtils.h>
#include <cefix/TiledProjectionHelper.h>
#include <osgDB/FileUtils>
#include <cefix/Log.h>
#include <cefix/DisplayMaskController.h>
#include <osg/io_utils>
#include <cefix/SysUtils.h>


namespace cefix {

inline osg::Matrix compute_projection_matrix(const osg::Vec3& screen_a, const osg::Vec3& screen_b, const osg::Vec3& screen_c, const osg::Vec3& eye, float near, float far)
{
	osg::Vec3 va, vb, vc;
	osg::Vec3 vr, vu, vn;
	
	osg::Vec3::value_type l,r,b,t,d;
	osg::Matrix M;
	
	vr = screen_b - screen_a;
	vu = screen_c - screen_a;
	
	vr.normalize();
	vu.normalize();
	
	vn = vr ^ vu;
	vn.normalize();
	
	va = screen_a - eye;
	vb = screen_b - eye;
	vc = screen_c - eye;
	
	d = -(va * vn);
	
	if (d == 0) {
		 d = -0.00001;
		 log::error("DispayCollection") << "could not compute frustum, va * vn = 0" << std::endl;
	}
	
	l = (vr * va) * near / d;
	r = (vr * vb) * near / d;
	b = (vu * va) * near / d;
	t = (vu * vc) * near / d;
	
	osg::Matrix result;
	result.makeFrustum(l, r, b, t, near, far);
	
	osg::Matrix::value_type *mptr = M.ptr();
	for(unsigned int i=0; i < 16; ++i) { mptr[i] = 0; }
	mptr[ 0] = vr[0];	mptr[ 4] = vr[1];  mptr[ 8] = vr[2];
	mptr[ 1] = vu[0];	mptr[ 5] = vu[1];  mptr[ 9] = vu[2];
	mptr[ 2] = vn[0];	mptr[ 6] = vn[1];  mptr[10] = vn[2];
	mptr[15] = 1.0;
		
	return osg::Matrix::translate(-eye) * M * result ;
}



void readFromSerializer(cefix::Serializer& s, osg::Viewport& vp)
{
	s >> "x" >> vp.x();
	s >> "y" >> vp.y();
	s >> "width" >> vp.width();
	s >> "height" >> vp.height();
}



void writeToSerializer(cefix::Serializer& s, osg::Viewport& vp)
{
	s << "x" << vp.x();
	s << "y" << vp.y();
	s << "width" << vp.width();
	s << "height" << vp.height();
}



static void writeToSerializer(Serializer& serializer, const osg::GraphicsContext::Traits& traits)
{
	serializer << "x"								<< traits.x;
	serializer << "y"								<< traits.y;
	serializer << "width"							<< traits.width;
	serializer << "height"							<< traits.height;
	serializer << "screenNum"						<< traits.screenNum;
	serializer << "displayNum"						<< traits.displayNum;
	serializer << "hostName"						<< traits.hostName;
	serializer << "windowName"						<< traits.windowName;
	serializer << "windowDecoration"				<< traits.windowDecoration;
	serializer << "supportsResize"					<< traits.supportsResize;
	serializer << "red"								<< traits.red;
	serializer << "blue"							<< traits.blue;
	serializer << "green"							<< traits.green;
	serializer << "stencil"							<< traits.stencil;
	serializer << "sampleBuffers"					<< traits.sampleBuffers;
	serializer << "samples"							<< traits.samples;
	serializer << "pbuffer"							<< traits.pbuffer;
	serializer << "quadBufferStereo"				<< traits.quadBufferStereo;
	serializer << "doubleBuffer"					<< traits.doubleBuffer;
	serializer << "vsync"							<< traits.vsync;
	serializer << "useMultiThreadedOpenGLEngine"	<< traits.useMultiThreadedOpenGLEngine;
	serializer << "useCursor"						<< traits.useCursor;
}



static void readFromSerializer(Serializer& serializer, osg::GraphicsContext::Traits& traits)
{
	serializer >> "x"								>> traits.x;
	serializer >> "y"								>> traits.y;
	serializer >> "width"							>> traits.width;
	serializer >> "height"							>> traits.height;
	serializer >> "screenNum"						>> traits.screenNum;
	serializer >> "displayNum"						>> traits.displayNum;
	serializer >> "hostName"						>> traits.hostName;
	serializer >> "windowName"						>> traits.windowName;
	serializer >> "windowDecoration"				>> traits.windowDecoration;
	serializer >> "supportsResize"					>> traits.supportsResize;
	serializer >> "red"								>> traits.red;
	serializer >> "blue"							>> traits.blue;
	serializer >> "green"							>> traits.green;
	serializer >> "stencil"							>> traits.stencil;
	serializer >> "sampleBuffers"					>> traits.sampleBuffers;
	serializer >> "samples"							>> traits.samples;
	serializer >> "pbuffer"							>> traits.pbuffer;
	serializer >> "quadBufferStereo"				>> traits.quadBufferStereo;
	serializer >> "doubleBuffer"					>> traits.doubleBuffer;
	serializer >> "vsync"							>> traits.vsync;
	serializer >> "useMultiThreadedOpenGLEngine"	>> traits.useMultiThreadedOpenGLEngine;
	serializer >> "useCursor"						>> traits.useCursor;


}



void Display::Configuration2D::readFrom(Serializer& s)
{
	s >> "width"  >>	s.defaultValue(width, 800.0f);
	s >> "height" >> s.defaultValue(height, 600.0f);
}



void Display::Configuration2D::writeTo(Serializer& s)
{
	s << "width" << width;
	s << "height" << height;
}	



void Display::Configuration3DPerspective::readFrom(Serializer& s)
{
	s >> "horizonal_field_of_view" >> s.defaultValue(horizontalFieldOfView, 60.0f);
	s >> "vertical_field_of_view"  >> s.defaultValue(verticalFieldOfView, -1.0f);
}



void Display::Configuration3DPerspective::writeTo(Serializer& s)
{
	s << "horizonal_field_of_view" << horizontalFieldOfView;
	s << "vertical_field_of_view" << verticalFieldOfView;
}	



void Display::Configuration3DFrustum::readFrom(Serializer& s)
{
	s >> "top_left_corner"     >> s.defaultValue(topLeftCorner, osg::Vec3(-3,0,0));
	s >> "bottom_left_corner"  >> s.defaultValue(bottomLeftCorner, osg::Vec3(0,0,0));
	s >> "bottom_right_corner" >> s.defaultValue(bottomRightCorner, osg::Vec3(4,0,0));
}



void Display::Configuration3DFrustum::writeTo(Serializer& s)
{
	s << "top_left_corner"		<< topLeftCorner;
	s << "bottom_left_corner"	<< bottomLeftCorner;
	s << "bottom_right_corner"	<< bottomRightCorner;
}	



void Display::Configuration3D::readFrom(Serializer& s)
{
	s >> "near_plane"     >> s.defaultValue(nearPlane,1.0f);
	s >> "far_plane"  >> s.defaultValue(farPlane, 1000.0f);
	s >> "is_frustum" >> s.defaultValue(isFrustum, false);
	if (isFrustum)	
		frustum.readFrom(s);
	else 
		perspective.readFrom(s);
}



void Display::Configuration3D::writeTo(Serializer& s)
{
	s << "near_plane"			<< nearPlane;
	s << "far_plane"			<< farPlane;
	s << "is_frustum"			<< isFrustum;
	if (isFrustum)
		frustum.writeTo(s);
	else 
		perspective.writeTo(s);
}	



#pragma mark  - - - Display - - - 


Display::Display(const std::string& identifier)
:	osg::Referenced(),
	_viewport(new osg::Viewport()),
	_identifier(identifier),
	_2dCamera(NULL),
	_3dCamera(NULL),
	_window(NULL),
	_windowConfiguration(NULL),
	_isMaster(false),
	_isSlave(false),
	_projOffset(osg::Matrix::identity()),
	_viewOffset(osg::Matrix::identity()),
	_useMasterSceneData(false),
	_mainDisplayId(""),
	_displayMaskController(NULL),
	_displayMaskControllerFileName("")
{
	_2dWorld = new osg::Group();
	_3dWorld = new osg::Group();
	_2dWorld->setName("display "+identifier+" 2d world");
	_3dWorld->setName("display "+identifier+" 3d world");
	
	osg::StateSet* stateset = _2dWorld->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
}



void Display::readFrom(Serializer& s) 
{
	s >> "viewport" >> *_viewport;
	s >> "c2d" >> _2d;
	s >> "c3d" >> _3d;
	s >> "identifier" >> s.defaultValue(_identifier, "main");
	s >> "is_master" >> s.defaultValue(_isMaster, false);
	s >> "is_slave" >> s.defaultValue(_isSlave, false);
	s >> "display_mask_file" >> _displayMaskControllerFileName;
	
	if (_isSlave) {
		s >> "main_display_id" >> _mainDisplayId;
		s >> "projection_offset" >> s.defaultValue(_projOffset, osg::Matrix::identity());
		s >> "view_offset" >> s.defaultValue(_viewOffset, osg::Matrix::identity());
		s >> "use_master_scene_data" >> s.defaultValue(_useMasterSceneData, false);
	}
	
	_2dWorld->setName("display "+_identifier+" 2d world");
	_3dWorld->setName("display "+_identifier+" 3d world");
	
	if(!_displayMaskControllerFileName.empty()) {
		_displayMaskController = new DisplayMaskController();
		_displayMaskController->load(_displayMaskControllerFileName);
	}
}



void Display::writeTo(Serializer& s) 
{
	s << "identifier" << _identifier;
	s << "viewport" << *_viewport;
	s << "c2d" << _2d;
	s << "c3d" << _3d;
	s << "is_master" << _isMaster;
	s << "is_slave" << _isSlave;
	s << "display_mask_file" << _displayMaskControllerFileName;
	
	if (_isSlave) {
		s << "main_display_id" << _mainDisplayId;
		s << "projection_offset" << _projOffset;
		s << "view_offset" << _viewOffset;
		s << "use_master_scene_data" << _useMasterSceneData;
	}
}



void Display::applyTo(osg::Camera* camera)
{	
	// Projektion
	{
		WindowRotation w_rot(_windowConfiguration ? _windowConfiguration->getWindowRotation() : WindowRotation::NONE);
		w_rot.setViewport(_viewport.get());
		camera->setViewport(w_rot.getViewport());
		if (_3d.isFrustum) 
		{
			WindowConfiguration* conf = getWindowConfiguration();
			if (!conf) {
				log::error("TiledDisplay::preflight") << "no valid window configuration found, aborting!" << std::endl;
				return;
			}
			
			DisplayCollection* dc = conf->getDisplayCollection();
			if (!dc) {
				log::error("TiledDisplay::preflight") << "no valid display collection found, aborting!" << std::endl;
				return;
			}
			
			osg::Vec3 eye_pos = dc->getEyePosition();
			camera->setProjectionMatrix(compute_projection_matrix(
				_3d.frustum.bottomLeftCorner, 
				_3d.frustum.bottomRightCorner, 
				_3d.frustum.topLeftCorner, 
				eye_pos, 
				_3d.nearPlane, 
				_3d.farPlane ));
			
			 std::cout<< "projektions-matrix for "  << _identifier << " : " << camera->getProjectionMatrix() << std::endl;


			if (isSlave()) {
				_projOffset = camera->getProjectionMatrix();
				camera->setProjectionMatrix(osg::Matrix::identity());
			}
			
			if (!_2d.valid()) {
				_2d = cefix::Display::Configuration2D(camera->getViewport()->width(), camera->getViewport()->height());
			}
			
		} else 
		{
			camera->setProjectionMatrix(w_rot.computeProjectionMatrix(_3d.perspective.horizontalFieldOfView, _3d.perspective.verticalFieldOfView, _3d.nearPlane, _3d.farPlane));
		}
	}
	
	
	// 2D-HUD
	if (_2d.valid()) 
	{
		WindowRotation w_rot(_windowConfiguration ? _windowConfiguration->getWindowRotation() : WindowRotation::NONE);
		osg::ref_ptr<osg::Camera> hud_camera = w_rot.createHudCamera(_2d.width, _2d.height);
		//hud_camera->setViewport(_viewport.get());
		if (!hud_camera->containsNode(_2dWorld.get()))
			hud_camera->addChild(_2dWorld.get());
		_2dCamera = hud_camera;
		
		if (_displayMaskController.valid()) {
			get2DWorld()->addChild(_displayMaskController->getOrCreateDisplayMask(_2d.width, _2d.height));
		}
	}
	
	_3dCamera = camera;
}


DisplayCollection* Display::getDisplayCollection() const
{
	WindowConfiguration* conf = getWindowConfiguration();
	if (!conf) {
		log::error("Display::getDisplayCollection") << "no valid window configuration found, aborting!" << std::endl;
		return NULL;
	}
	
	DisplayCollection* dc = conf->getDisplayCollection();
	if (!dc) {
		log::error("Display::getDisplayCollection") << "no valid display collection found, aborting!" << std::endl;
		return NULL;
	}
	
	return dc;
}

Display* Display::getMainDisplay() const
{
	
	DisplayCollection* dc = getDisplayCollection();
	Display* display = (dc) ? dc->getDisplay(_mainDisplayId) : NULL;
	if (!display) {
		log::error("Display::getMaindisplay") << "no valid display '" << _mainDisplayId << "' found, aborting!" << std::endl;
		return NULL;
	}
	
	return display;
}



#pragma mark  - - - TiledDisplay - - - 

TiledDisplay::TiledDisplay(const std::string& identifier, const std::string& main_display_id, float l, float t, float w, float h, bool is_slave)
:	Display(identifier),
	_clippingRect(l, t, l+w, t+h),
	_applyTo2D(true)
{
	_isMaster = !is_slave;
	_isSlave = is_slave;
	setIsSlave(is_slave, main_display_id);
}



TiledDisplay::TiledDisplay(const std::string& identifier, const std::string& main_display_id,const osg::Vec4& rect, bool is_slave)
:	Display(identifier),
	_clippingRect(rect),
	_applyTo2D(true)
{
	_isMaster = !is_slave;
	_isSlave = is_slave;
	setIsSlave(is_slave, main_display_id);
}



void TiledDisplay::applyTo(osg::Camera* cam) 
{
	
	// dummy values, so the 2d-camera gets created
	
	float w(_clippingRect[2] - _clippingRect[0]);
	float h(_clippingRect[3] - _clippingRect[1]);
	
	setViewport(0,0,w,h);
	set2DConfiguration(w, h);
	
	Display::applyTo(cam);
	
	Display* display = getMainDisplay();
	
	if (!display) {
		log::error("Display::applyTo") << "no valid main-display found: '" << getMainDisplayId() << "'" << std::endl;
		return;
	}
	
	// std::cout << _identifier <<" tiled rendering into " << _clippingRect << std::endl;
	
	osg::Matrix projection = display->get3DCamera()->getProjectionMatrix();
	
	TiledProjectionHelper pph(display->getViewport()->width(), display->getViewport()->height());
	WindowRotation w_rot(_windowConfiguration ? _windowConfiguration->getWindowRotation() : WindowRotation::NONE);
	
	osg::Matrix proj_offset = pph.getOffsetForProjection(_clippingRect[0], _clippingRect[1], w,h);
	
	if (isSlave()) 
	{
		setSlaveConfiguration(proj_offset, osg::Matrix::identity(), false);
	}
	else 
	{
		get3DCamera()->setProjectionMatrix(projection * proj_offset );
	}
		
	if (_applyTo2D) 
	{
		w_rot.setupHudCamera(get2DCamera(), w,h, pph.getOffsetForOrtho(_clippingRect[0], _clippingRect[1], w,h));
	}
	
}



void TiledDisplay::readFrom(cefix::Serializer& s)
{
	s >> "identifier" >> _identifier;
	s >> "clipping_rect" >> _clippingRect;
	s >> "apply_to_2d" >> _applyTo2D;
	s >> "is_master" >> _isMaster;
	s >> "is_slave" >> _isSlave;
	s >> "main_display_id" >> _mainDisplayId;
}



void TiledDisplay::writeTo(cefix::Serializer& s)
{
	s << "identifier" << _identifier;
	s << "clipping_rect" << _clippingRect;
	s << "apply_to_2d" << _applyTo2D;
	s << "is_master" << _isMaster;
	s << "is_slave" << _isSlave;
	s << "main_display_id" << _mainDisplayId;
}

#pragma mark - - -  CustomProjectionDisplay - - - 

void CustomProjectionDisplay::applyTo(osg::Camera* cam) 
{
	Display::applyTo(cam);
	cam->setProjectionMatrix(_projection);	
}



void CustomProjectionDisplay::readFrom(cefix::Serializer& s)
{
	s >> "identifier" >> _identifier;
	s >> "projection" >> _projection;
}



void CustomProjectionDisplay::writeTo(cefix::Serializer& s)
{
	s << "identifier" << _identifier;
	s << "projection" << _projection;
}


#pragma mark  - - - WindowConfiguration - - - 


WindowConfiguration::WindowConfiguration()
:	osg::Referenced(),
	_traits(new osg::GraphicsContext::Traits()),
	_rotation(WindowRotation::NONE),
	_poseAsMainWindow(false),
	_hasStatsHandler(true),
	_hasStateHandler(true),
	_hasViewerHandler(true),
	_displayCollection(NULL),
	_mayBeAddedToViewer(true),
	_enabled(true),
	_shareContextWithMainWindow(false),
    _isFullScreen(false),
    _multiTouchEnabled(false)

{
        #ifdef CEFIX_FOR_IPHONE
            setMultiTouchEnabled(true);
        #endif
        setIsFullScreen(false);
}



WindowConfiguration* WindowConfiguration::createFullScreen(osg::GraphicsContext::ScreenIdentifier si)
{
	osg::ref_ptr<WindowConfiguration> conf = new WindowConfiguration();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	
	traits->screenNum = si.screenNum;
	traits->displayNum = si.displayNum;
	traits->hostName = si.hostName;
	
	unsigned int w, h;
	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    
    if (wsi)
        wsi->getScreenResolution(si, w,h);
    else {
        cefix::log::error("WindowConfiguration") << "could not get windowing system interface" << std::endl;
        return NULL;
    }
	traits->x = 0;
	traits->y = 0;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->vsync = true;
	traits->windowDecoration = false;
    traits->useMultiThreadedOpenGLEngine = false;
	
	conf->setTraits(traits.get());
	return conf.release();
}


	
WindowConfiguration* WindowConfiguration::createCenteredWindowOnScreen(osg::GraphicsContext::ScreenIdentifier si, unsigned int w, unsigned h, ComputeWindowCoordsPolicy policy)
{
	osg::ref_ptr<WindowConfiguration> conf = new WindowConfiguration();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits();
	
	traits->screenNum = si.screenNum;
	traits->displayNum = si.displayNum;
	traits->hostName = si.hostName;
	if(policy == COMPUTE_LAZY) {
        traits->x = -1;
        traits->y = -1;
	} else {
        unsigned int screen_width, screen_height;
        osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
        wsi->getScreenResolution(*traits, screen_width, screen_height);
        traits->x = (screen_width - w) /2.0;
        traits->y = (screen_height - h) /2.0;
    }
    traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->vsync = true;
	traits->windowDecoration = true;
    traits->useMultiThreadedOpenGLEngine = false;
	
	conf->setTraits(traits.get());
	return conf.release();
}



WindowConfiguration* WindowConfiguration::createWindowOnScreen(osg::GraphicsContext::ScreenIdentifier si, unsigned int left, unsigned int top, unsigned int w, unsigned h)
{
	osg::ref_ptr<WindowConfiguration> conf = new WindowConfiguration();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits();
	
	traits->screenNum = si.screenNum;
	traits->displayNum = si.displayNum;
	traits->hostName = si.hostName;

	traits->x =left;
	traits->y = top;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->vsync = true;
	traits->windowDecoration = true;
	
	conf->setTraits(traits.get());
	return conf.release();
}



void WindowConfiguration::readFrom(Serializer& s) 
{
	s.registerClass<TiledDisplay>("tiled_display");
	s.registerClass<CustomProjectionDisplay>("custom_projection_display");
		
	s >> "enabled" >> s.defaultValue(_enabled, true);
	s >> "traits" >> *_traits;
	s >> "displays" >> _displays;
	s >> "pose_as_main_window" >> s.defaultValue(_poseAsMainWindow, false);
	s >> "has_stats_handler" >> s.defaultValue(_hasStatsHandler, true);
	s >> "has_state_handler" >> s.defaultValue(_hasStateHandler, true);
	s >> "has_viewer_handler" >> s.defaultValue(_hasViewerHandler, true);
	s >> "share_context_with_main_window" >> s.defaultValue(_shareContextWithMainWindow, false);

	std::string rot;
	s >> "window_rotation" >> rot;
	_rotation = WindowRotation::getRotationEnumsAsString().value(rot, WindowRotation::NONE);
    
    s >> "fullscreen" >> s.defaultValue(_isFullScreen, false);
    #ifdef CEFIX_FOR_IPHONE
    s >> "multi_touch_enabled" >> s.defaultValue(_multiTouchEnabled, true);
    #else
    s >> "multi_touch_enabled" >> s.defaultValue(_multiTouchEnabled, false);
    #endif
}	



void WindowConfiguration::writeTo(Serializer& s) 
{
	s.registerClass<TiledDisplay>("tiled_display");
	s.registerClass<CustomProjectionDisplay>("custom_projection_display");
	
	s << "enabled" << _enabled;
	s << "traits" << *_traits;
	s << "displays" << _displays;
	s << "pose_as_main_window" << _poseAsMainWindow;
	s << "has_stats_handler" << _hasStatsHandler;
	s << "has_state_handler" << _hasStateHandler;
	s << "has_viewer_handler" << _hasViewerHandler;
	s << "share_context_with_main_window" << _shareContextWithMainWindow;
	s << "window_rotation" << WindowRotation::getRotationEnumsAsString().string(_rotation);
    s << "fullscreen" << _isFullScreen;
    s << "multi_touch_enabled" << _multiTouchEnabled;
}	



void WindowConfiguration::preflight()
{
	if (getNumDisplays() == 0) 
	{
		addDisplay(createStandardDisplay());
	}
	unsigned int num_master(0);
	for(unsigned int i = 0; i < getNumDisplays(); ++i) {
		getDisplay(i)->setWindowConfiguration(this);
		if (getDisplay(i)->isMaster()) num_master++;
	}
	
	if ((num_master == 0) && (!getDisplay(0)->isSlave()))
		getDisplay(0)->setIsMaster(true);
	else if (num_master > 1) 
	{
		// alle master bis auf den ersten zurücksetzen
		log::error("WindowConfiguration::preflight") << "more than one masters, resetting others ..." << std::endl;
		
		bool start_clear = false;
		for(unsigned int i = 0; i < getNumDisplays(); ++i) 
		{
			if (getDisplay(i)->isMaster()) 
			{
				if (start_clear)
					getDisplay(i)->setIsMaster(false);
				else
					start_clear = true;
			}
		}
	}
	
	if ((getNumDisplays() == 1) && (getDisplay(0)->isSlave())) 
	{
		// für den fall, das diese Konfiguration nur ein Display hat, und dieses ein slave ist, dann nicht als View dazuadden lassen
		_mayBeAddedToViewer = false;
	}
	
}



Display* WindowConfiguration::createStandardDisplay(const std::string& identifier)
{
	// create standard display
	
	osg::ref_ptr<Display> display = new Display(identifier);
	double scale = getTraits() ? SysUtils::getUIScale(*getTraits()) : 1.0;
	Display::Configuration2D conf_2d;
	conf_2d.width = _traits->width / scale;
	conf_2d.height = _traits->height / scale;
	
	Display::Configuration3D conf_3d;
	conf_3d.isFrustum = false;
	conf_3d.perspective.horizontalFieldOfView = 60;
	conf_3d.perspective.verticalFieldOfView = -1;
	conf_3d.nearPlane = 1;
	conf_3d.farPlane = 1000;
	
	display->set2DConfiguration(conf_2d);
	display->set3DConfiguration(conf_3d);
	
	display->setViewport(0,0, _traits->width, _traits->height);
	display->setIsMaster(true);
	
	return display.release();
}



void WindowConfiguration::setWindow(cefix::ApplicationWindow* win) 
{
	for(unsigned int i = 0; i < getNumDisplays(); ++i) 
	{
		getDisplay(i)->setWindow(win);
	}
}



#pragma mark  - - - DisplayCollection - - - 


DisplayCollection* DisplayCollection::instance(DisplayCollection* dp)
{
	static osg::ref_ptr<DisplayCollection> s_ptr;
	if (dp) s_ptr = dp;
	return s_ptr.get();
}



DisplayCollection::DisplayCollection() 
:	osg::Referenced(),
    _createDefaultConfigurationWhenEmpty(true)
{

}


void DisplayCollection::addDisplay(Display* display) { 
	if (hasDisplay(display->getIdentifier()))
		log::error("WindowConfiguration::addDisplay") << "there's already a display named " << display->getIdentifier() << "overwriting it now" << std::endl;
	
	_displays[display->getIdentifier()] = display; 
}



bool DisplayCollection::readConfigurationFromPrefs(const std::string& prefsKey)
{
	osg::ref_ptr<cefix::AbstractProperty> prop = cefix::DataFactory::instance()->getPreferencesFor(prefsKey);
    if (!prop) {
        log::error("DisplayCollection::readConfigurationFromPrefs") << "could not find prefskey " << prefsKey << std::endl;
        return false;
    }
    if (prop->getType() == cefix::AbstractProperty::STRING) {
        return loadConfigurationFromFile(prop->asString());
    } else if (prop->getType() == cefix::AbstractProperty::PROPERTY_LIST) {
        cefix::Serializer s(prop->asPropertyList());
        s >> "Displays" >> this;
        return true;
    } 
    
    return false;   
}



bool DisplayCollection::loadConfigurationFromFile(const std::string filename)
{
	std::string file = osgDB::findDataFile(filename);
	if(file.empty())
	{
		log::error("DisplayCollection::loadConfiguration") << "could not find configuration file " << filename << std::endl;
		return false; 
	}
	
	osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList(file);
	cefix::Serializer s(pl);
	s >> "Displays" >> this;
	
	return true;
}

void DisplayCollection::saveConfigurationToFile(const std::string filename)
{
	preflight();
	
	cefix::PropertyList* pl = new cefix::PropertyList();
	cefix::Serializer s(pl);
	s << "displays" << this;
	pl->saveAsXML(filename);
}

void DisplayCollection::createDefaultConfiguration() 
{
	cefix::DataFactory* df = cefix::DataFactory::instance();
	
	std::string prefskey("/Viewer/mainWindow");
	if (df->getPreferencesPropertyListFor(prefskey) == NULL)
		prefskey = "/Viewer";
	
	
	#ifdef CEFIX_FOR_IPHONE
		osg::ref_ptr<WindowConfiguration> data = WindowConfiguration::createFullScreen(0);	
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = data->getTraits();
		std::string w_rot("NONE");
	#else
		osg::ref_ptr<WindowConfiguration> data = new WindowConfiguration();
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
		
		traits->doubleBuffer		= true;
		traits->width				= df->getPreferencesFor(prefskey+"/width", 800);
		traits->height				= df->getPreferencesFor(prefskey+"/height", 600);
		traits->screenNum			= df->getPreferencesFor(prefskey+"/screenNum", 1) - 1;
		traits->x					= df->getPreferencesFor(prefskey+"/left", -1);
		traits->y					= df->getPreferencesFor(prefskey+"/top", -1);
		traits->windowDecoration  	= df->getPreferencesFor(prefskey+"/Decoration", true);
		traits->windowName			= df->getPreferencesFor(prefskey+"/name", std::string("cefix Render Window"));
		traits->supportsResize		= df->getPreferencesFor(prefskey+"/ResizeAllowed", false);
		traits->samples				= df->getPreferencesFor(prefskey+"/fsaa", 0);
		traits->useMultiThreadedOpenGLEngine = df->getPreferencesFor(prefskey+"/multithreadedOpenGL", false);
		traits->vsync				= df->getPreferencesFor(prefskey+"/vsync", true);
		traits->useCursor			= df->getPreferencesFor(prefskey+"/showMouseCursor", true);
		std::string w_rot			= df->getPreferencesFor(prefskey+"/Rotation","NONE");
		
		data->setIsFullScreen(df->getPreferencesFor(prefskey+"/FullScreen", false));
		
		if (traits->x < 0) {
			unsigned int screenWidth, screenHeight;
			osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
			wsi->getScreenResolution(*traits, screenWidth, screenHeight);

			
			traits->x = osg::maximum<int>(0, (screenWidth-traits->width) / 2);
			traits->y = osg::maximum<int>(0, (screenHeight-traits->height) / 2);
		}
	#endif
	
	if (traits->samples && (traits->sampleBuffers == 0))
		traits->sampleBuffers = 1; 
	
	data->setWindowRotation(WindowRotation::getRotationEnumsAsString().value(w_rot, WindowRotation::NONE));
	data->setTraits(traits);
	
	// create standard display
	data->preflight();
	
	Display::Configuration3D& conf_3d = data->getDisplay(0)->get3DConfiguration();
	
	conf_3d.nearPlane				=  df->getPreferencesFor(prefskey+"/perspective/nearplane", 1.0f);
	conf_3d.farPlane				=  df->getPreferencesFor(prefskey+"/perspective/farplane", 1000.0f);
	conf_3d.perspective.horizontalFieldOfView =  df->getPreferencesFor(prefskey+"/perspective/horizontalFieldOfView", -1.0f);
	conf_3d.perspective.verticalFieldOfView   =  df->getPreferencesFor(prefskey+"/perspective/verticalFieldOfView", -1.0f);
	conf_3d.isFrustum = false;
	
	
	addWindowConfiguration(data);
}



void DisplayCollection::preflight() 
{
	if (!_needsPreflight) return;
	for(WindowConfigurationVector::iterator itr = _windowConfigurations.begin(); itr != _windowConfigurations.end(); ++itr)
	{
		(*itr)->setDisplayCollection(this);
		(*itr)->preflight();
		
		// Displays die noch nicht Teil der DisplayCollection sind hinzufügen
		for(unsigned int i = 0; i < (*itr)->getNumDisplays(); ++i) 
		{
			Display* display = (*itr)->getDisplay(i);
			if (!hasDisplay(display->getIdentifier()))
				addDisplay(display);
		}
	}
	
	DisplayMap unconnected_displays;
	collectUnConnectedDisplays(unconnected_displays);
	for(DisplayMap::iterator i = unconnected_displays.begin(); i != unconnected_displays.end(); ++i) 
	{
		i->second->applyTo(new osg::Camera());
	}
	
	_needsPreflight = false;
}



void DisplayCollection::applyConfiguration(cefix::AppController* ctrl)
{
	if ((_windowConfigurations.size() == 0) && createDefaultConfigurationWhenEmpty()) {
        log::error("DisplayCollection") << "no valid windowconfigurations found, setting up standard configuration..." << std::endl;
        createDefaultConfiguration();
    }
    
    preflight();
	
	WindowConfiguration* main_window_conf(NULL);
	osg::ref_ptr<cefix::ApplicationWindow> main_window(NULL);
	
	
	// erst das main-window suchen und erzeugen
	for(WindowConfigurationVector::iterator itr = _windowConfigurations.begin(); itr != _windowConfigurations.end(); ++itr)
	{
		if (!(*itr)->isEnabled())
			continue;
		if ((!main_window_conf) || ((*itr)->poseAsMainWindow()))
			main_window_conf = *itr;
	}
	
	if (main_window_conf) 
	{
		main_window = new cefix::ApplicationWindow(main_window_conf);
		ctrl->setMainWindow(main_window);
		
		if (main_window_conf->mayBeAddedToViewer()) {
			main_window_conf->setWindow(main_window);
			ctrl->addView(main_window);
		}
	}
	
	// jetzt alle anderen Fenster erzeugen
	
	for(WindowConfigurationVector::iterator itr = _windowConfigurations.begin(); itr != _windowConfigurations.end(); ++itr)
	{
		if (!(*itr)->isEnabled())
			continue;
		if (*itr == main_window_conf)
				continue;
		
		if ((*itr)->shareContextWithMainWindow() && main_window)
			(*itr)->getTraits()->sharedContext = main_window->getCamera()->getGraphicsContext();
			
		osg::ref_ptr<cefix::ApplicationWindow> win = new cefix::ApplicationWindow(*itr);
		
		if ((*itr)->mayBeAddedToViewer())
		{
			ctrl->addView(win);
			(*itr)->setWindow(win);
		}
	}

	connectSingleSlaves(ctrl);
}



void DisplayCollection::connectSingleSlaves(cefix::AppController* ctrl) 
{

	// erstmal alle tiled-displays in gruppen sortieren
	typedef std::map<std::string, std::vector<Display*> > DisplayGroups;
	DisplayGroups groups;
	
	for(DisplayMap::iterator i = _displays.begin(); i != _displays.end(); ++i) 
	{
		if((i->second->isSlave()) && (i->second->getMainDisplayId() != "")) 
		{
			Display* td = i->second.get();
			groups[td->getMainDisplayId()].push_back(td);
		}
	}
	
	// für jede gruppe eine master-view erzeugen, und die tiled-displays als slave adden
	for(DisplayGroups::iterator i = groups.begin(); i != groups.end();++i) 
	{
		Display* master_display = getDisplay(i->first);
		osg::ref_ptr<ApplicationWindow> view = new ApplicationWindow(master_display->getViewport()->width(), master_display->getViewport()->height() );
		master_display->get3DCamera()->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
		view->setCamera(master_display->get3DCamera());
		// die master-camera darf keinen view-port haben, sonst hauts osg bei der berechnung durcheinander
		master_display->get3DCamera()->setViewport(NULL);
		master_display->get3DCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
		master_display->setWindow(view);
		view->setSceneData(master_display->get3DWorld());
		view->realize();
		
		
		std::vector<Display*>& displays(i->second);
		for(std::vector<Display*>::iterator j = displays.begin(); j != displays.end(); ++j) 
		{
			if ((*j)->isSlave()) 
			{
				view->addSlave((*j)->get3DCamera(), (*j)->getSlaveProjectionOffset(), (*j)->getSlaveViewOffset(), false);
			}
		}
		
		ctrl->addView(view.get());
	}
}



void DisplayCollection::collectUnConnectedDisplays(DisplayMap& unconnected_displays)
{
	for(DisplayMap::iterator i = _displays.begin(); i != _displays.end(); ++i) {
		if(i->second->getWindowConfiguration() == NULL) {
			unconnected_displays.insert(std::make_pair(i->first, i->second));
		}
	}
}


void DisplayCollection::readFrom(Serializer& s)
{
	s.registerClass<TiledDisplay>("tiled_display");
	s.registerClass<CustomProjectionDisplay>("custom_projection_display");
	
	s >> "displays" >> _displays;
	s >> "windows" >> _windowConfigurations;
	s >> "eye_pos" >> _eyePos;
	_needsPreflight = true;
}



void DisplayCollection::writeTo(Serializer& s)
{
	s.registerClass<TiledDisplay>("tiled_display");
	s.registerClass<CustomProjectionDisplay>("custom_projection_display");
	
	DisplayMap unconnected_displays;
	collectUnConnectedDisplays(unconnected_displays);
	
	s << "displays" << unconnected_displays; 
	s << "windows" << _windowConfigurations;
	s << "eye_pos" << _eyePos;
}


}
