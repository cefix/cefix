/*
 *  AppController.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 08.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <osgDB/Registry>

#include "AppController.h"
#include <cefix/CefixOptimizerCallback.h>
#include <cefix/DataFactory.h>
#include <cefix/ApplicationWindow.h>
#include <cefix/Log.h>
#include <cefix/Settings.h>
#include <cefix/StringUtils.h>
#include <cefix/AnimationFactory.h>
#include <cefix/VideoMediaManager.h>
#include <cefix/PickEventHandler.h>
#include <cefix/PostScriptWriter.h>
#include <cefix/Notify.h>
#include <cefix/PostscriptWriter.h>


#include <cefix/StatisticsGroup.h>
#include <cefix/LineStatistics.h>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/Renderer>
#include <cefix/TiledProjectionHelper.h>
#include <cefix/RebootObserver.h>
#include <cefix/Timer.h>
#include <cefix/WidgetThemeManager.h>
#include <cefix/HeartBeatTimer.h>
#include <cefix/ConvertScreenCoord.h>
#include <cefix/Logger.h>
#include <cefix/ConvertScreenCoord.h>
#include <cefix/DebugGeometryFactory.h>

namespace cefix {



class Stats : public osg::Referenced {
		
	public:
		Stats(bool simple);
        ~Stats();
		
		void setVideoStats(double d)   { add("video (ms)", d); }
		void setAnimationStats(double d)   { add("animation (ms)", d); }
		void setEventStats(double d)       { add("event (ms)", d);}
		void setUpdateStats(double d)      { add("update (ms)", d);}
		void setCullAndDrawStats(double d) { add("cull + draw (ms)", d);}
		void setFrameDuration(double d)    
		{
			add("frame (ms)", d); 
            add("fps (f/s)", 1000/d);
		}
		
		cefix::StatisticsGroup* getStatisticsGroup() { return _stats.get(); }
		
		inline void add(const std::string& key, double dt) 
        {
            _customValues[key] = cefix::interpolate(_customValues[key], dt, _interpolationScalar);
        }
        
				
		void registerCustomStats(const std::string& key) 
        {
			if (!_stats->hasStatistic(key)) {
				LineStatistics* ls = new cefix::LineStatistics(key, 100);
				_stats->addStatistic(key, ls);
                _customValues[key] = 0.0f;
			}
		}
		
		void setValue(const std::string& key, double value) {
            _stats->addValue(key, value, 0);
        }
		
		inline void update() 
        {
            if (_t.delta_s(_lastUpdate, _t.tick()) < 1/10.0) {
                return;
            }
            
            _lastUpdate = _t.tick();
            
            for(std::map<std::string, double>::iterator i = _customValues.begin(); i != _customValues.end(); ++i) {
                setValue(i->first, i->second);
            }
			
            _stats->adjustLocations();
		
		}
		
		
		float _interpolationScalar;
		osg::ref_ptr<StatisticsGroup> _stats;
		osg::Timer _t;
		osg::Timer_t _lastUpdate;
        std::map<std::string, double> _customValues;
};

class UpdateStatsCallback : public osg::NodeCallback {

public:
    UpdateStatsCallback(Stats* stats) : osg::NodeCallback(), _stats(stats) {}
    
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
        _stats->update();
        traverse(node, nv);
    }
    
    Stats* _stats;

};

Stats::Stats(bool simple) : osg::Referenced() 
{
    _stats = new cefix::StatisticsGroup();
    _interpolationScalar = 0.07;
    _lastUpdate = _t.tick();
                
    if (!simple) {
		registerCustomStats("video (ms)");
		registerCustomStats("animation (ms)");
		registerCustomStats("event (ms)");
		registerCustomStats("update (ms)");
		registerCustomStats("cull + draw (ms)");
		registerCustomStats("frame (ms)");
	}
    registerCustomStats("fps (f/s)");
    
    _stats->setUpdateCallback(new UpdateStatsCallback(this)); 
}

Stats::~Stats() 
{
    if (_stats) _stats->setUpdateCallback(NULL);
}


class PrivateData {
public:
	PrivateData() : stats(NULL), postscriptWriter(NULL), heart_beat(NULL), qtmm(NULL), af(NULL) { }
	
	osg::ref_ptr<Stats> stats;
	osg::ref_ptr<PostScriptWriter> postscriptWriter;
	osg::ref_ptr<cefix::RebootObserver> rebootObserver;
	osg::ref_ptr<HeartBeatTimer> heart_beat;
	osg::ref_ptr<cefix::VideoMediaManager> qtmm;
	osg::ref_ptr<cefix::AnimationFactory> af;
};

// ----------------------------------------------------------------------------------------------------------
// createLogFiles
// ----------------------------------------------------------------------------------------------------------

void AppController::createLogFiles(int amode) {
	
	// log-files ggf erzeugen
    int mode = Logger::REDIRECT_NOTIFY | Logger::CONSOLE;
        
    if (DataFactory::instance()->getPreferencesFor("/Debug/ErrorLog")) {
       mode |= Logger::FILE;
	   
		if (DataFactory::instance()->getPreferencesFor<int>("/Debug/errorLog/onScreen", true)) {
            mode |= Logger::TEXT_GEODE;
        }
	}
	
	if ((amode != 0) && (mode == 0)) {
		mode = amode;
	}
	
	if (mode != 0) {
        _errorLog = new Logger(DataFactory::instance()->getRootPath()+"/error.log", mode);
        subscribeTo("addToErrorLog", this, &AppController::handleAddErrorLogMessage);
    }
}

// ----------------------------------------------------------------------------------------------------------
// setNotifyLevelFromPrefs
// ----------------------------------------------------------------------------------------------------------

void AppController::setNotifyLevelFromPrefs() {

	
	std::string notifylevel = cefix::strToLower(DataFactory::instance()->getPreferencesFor<std::string>("/viewer/notifyLevel", "WARN"));
    
    std::map<std::string, osg::NotifySeverity> levelmap;
    levelmap["debug_fp"] = osg::DEBUG_FP;
    levelmap["debug_info"] = osg::DEBUG_INFO;
    levelmap["info"] = osg::INFO;
    levelmap["notice"] = osg::NOTICE;
    levelmap["warn"] = osg::WARN;
    levelmap["fatal"] = osg::FATAL;
    levelmap["always"] = osg::ALWAYS;
    
    if ( levelmap.find(notifylevel) == levelmap.end()) {
        osg::setNotifyLevel(osg::WARN);
        log::error("AppController") << "could not parse notifylevel: " << notifylevel << std::endl;
    }
    else 
        osg::setNotifyLevel(levelmap[notifylevel]);

}


// ----------------------------------------------------------------------------------------------------------
// setNotifyLevelFromPrefs
// ----------------------------------------------------------------------------------------------------------

void AppController::setThreadingModelFromPrefs() {

	
	std::string model = cefix::strToLower(DataFactory::instance()->getPreferencesFor<std::string>("/viewer/threadingModel", "SINGLETHREADED"));
    
    std::map<std::string, osgViewer::ViewerBase::ThreadingModel> levelmap;
    levelmap["singlethreaded"]			= osgViewer::ViewerBase::SingleThreaded;
    levelmap["culldrawpercontext"]		= osgViewer::ViewerBase::CullDrawThreadPerContext;
    levelmap["drawpercontext"]			= osgViewer::ViewerBase::DrawThreadPerContext;
    levelmap["cullthreadpercameradrawthreadpercontext"]					= osgViewer::ViewerBase::CullThreadPerCameraDrawThreadPerContext;
    levelmap["threadpercamera"]			= osgViewer::ViewerBase::ThreadPerCamera;
	levelmap["automatic"]			= osgViewer::ViewerBase::AutomaticSelection;
    
    if ( levelmap.find(model) == levelmap.end()) {
        setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
        log::error("AppController") << "could not parse threading model: " << model << std::endl;
    }
    else {
		setThreadingModel(levelmap[model]);
		log::info("AppController") << "set threading model: " << model << std::endl;
	}

}



void AppController::setWindowRectangle(int w, int h, bool include2DLayer)
{
	getMainWindow()->setWindowRectangle(w, h);
	
	if (include2DLayer) {
		ConvertScreenCoord::setHeight(h);
        WindowRotation wrot(getMainWindow()->getRotation());
		wrot.setupHudCamera(DisplayCollection::instance()->getDisplay("main")->get2DCamera(), w, h);
	}
}



void AppController::setStereoSettingsFromPrefs() {

	if (cefix::DataFactory::instance()->getPreferencesFor("/Viewer/stereo/enabled", 0) == 0) 
		return;
		
	std::map<std::string, osg::DisplaySettings::StereoMode>	stereomodemap;
	
	stereomodemap["quad_buffer"] = osg::DisplaySettings::QUAD_BUFFER;
	stereomodemap["anaglyphic"] = osg::DisplaySettings::ANAGLYPHIC;
	stereomodemap["horizontal_split"] = osg::DisplaySettings::HORIZONTAL_SPLIT;
	stereomodemap["vertical_split"] = osg::DisplaySettings::VERTICAL_SPLIT;
	stereomodemap["left_eye"] = osg::DisplaySettings::LEFT_EYE;
	stereomodemap["right_eye"] = osg::DisplaySettings::RIGHT_EYE;
	stereomodemap["horizontal_interlace"] = osg::DisplaySettings::HORIZONTAL_INTERLACE;
	stereomodemap["vertical_interlace"] = osg::DisplaySettings::VERTICAL_INTERLACE;
	stereomodemap["checkerboard"] = osg::DisplaySettings::CHECKERBOARD; 
	
	std::string smode_str = cefix::strToLower(cefix::DataFactory::instance()->getPreferencesFor("/Viewer/stereo/mode", "anaglyphic"));
	osg::DisplaySettings::StereoMode stereomode = stereomodemap.find(smode_str) != stereomodemap.end() ? stereomodemap[smode_str] : osg::DisplaySettings::ANAGLYPHIC;
	osg::DisplaySettings::instance()->setStereoMode(stereomode);
	osg::DisplaySettings::instance()->setStereo(true);
	
	float eyeseparation = cefix::DataFactory::instance()->getPreferencesFor("/Viewer/stereo/eyeseparation", -1.0f);
	float screenWidth = cefix::DataFactory::instance()->getPreferencesFor("/Viewer/stereo/screen/width", -1.0f);
	float screenHeight = cefix::DataFactory::instance()->getPreferencesFor("/Viewer/stereo/screen/height", -1.0f);
	float screenDistance = cefix::DataFactory::instance()->getPreferencesFor("/Viewer/stereo/screenDistance", -1.0f);
	
	if (eyeseparation >= 0) 
		osg::DisplaySettings::instance()->setEyeSeparation(eyeseparation);
	if (screenWidth >= 0) 
		osg::DisplaySettings::instance()->setScreenWidth(screenWidth);
	if (screenHeight >= 0) 
		osg::DisplaySettings::instance()->setScreenHeight(screenHeight);
	if (screenDistance >= 0) 
		osg::DisplaySettings::instance()->setScreenDistance(screenDistance);


}

// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

AppController::AppController(int loggerMode) :
	osgViewer::CompositeViewer(),
	cefix::NotificationListener(),
	_useOptimizer(true),
	_data(new PrivateData()),
	_useHeartBeatTimer(true),
	_inited(false),
	_displayCollection(NULL)
{	

    cefix::DataFactory::instance()->setAppController(this);
	
	setStereoSettingsFromPrefs();
	
	setNotifyLevelFromPrefs();
	setThreadingModelFromPrefs();
	createLogFiles(loggerMode);
	// cefix-Prefs laden
    cefix::PropertyList* cefixPrefs = DataFactory::instance()->getPreferencesPropertyListFor ("/Viewer/cefix");
    if (cefixPrefs) cefix::Settings::instance()->set(cefixPrefs);
    
	subscribeTo("requestRedraw", this, &AppController::handleRequestRedraw);
	
}


void AppController::initImplementation() 
{
	if (_inited) return;
	_inited = true;
	
	cefix::log::info("AppController") << "initImplementation" << std::endl;
	
	init();
	
	// DisplayCollection laden 
	DisplayCollection* display_collection = getDisplayCollection();
	if (!display_collection) 
	{
		display_collection = new DisplayCollection();
		if (!display_collection->readConfigurationFromPrefs("/Viewer/Displays")) 
        {
			display_collection->createDefaultConfiguration();
        }
		setDisplayCollection(display_collection);
	}
	display_collection->applyConfiguration(this);
	
	// theme laden 
	
	std::string theme =cefix::DataFactory::instance()->getPreferencesFor("/Viewer/theme", "");
	if (!theme.empty()) {
		cefix::WidgetThemeManager::instance()->loadThemeAndMakeCurrent(theme);
	}
		
	_data->rebootObserver = new RebootObserver();
	_data->rebootObserver->start();
	
}



// ----------------------------------------------------------------------------------------------------------
// handleRequestRedraw
// ----------------------------------------------------------------------------------------------------------

void AppController::handleRequestRedraw() 
{
	_data->qtmm->idle();
	frame();   
}



// ----------------------------------------------------------------------------------------------------------
// applyWorld
// ----------------------------------------------------------------------------------------------------------

void AppController::applyWorld(osg::Node* node, ApplicationWindow* win) 
{
	initImplementation();

	if (node == NULL) {
		log::error("AppController") << "No scene to apply" << std::endl;
		setDone(true);
		return;
	}
	
	osg::Group* g;
	g = dynamic_cast<osg::Group*>(node);
	if (g == NULL) {
		g = new osg::Group();
		g->addChild(node);
	}
	
	if (win == NULL) win = getMainWindow();
	
	if ((g->getNumChildren() > 0) && (_useOptimizer)) {
        osgUtil::Optimizer o;
		if (!_optimizerCallback.valid()) _optimizerCallback = new CefixOptimizerCallback();
		o.setIsOperationPermissibleForObjectCallback(_optimizerCallback.get());
        o.optimize(g);
    }
    
    if (win && (getMainWindow() == win) && (_errorLog.valid()) && (_errorLog->getTextGeode())) 
	{
		_errorLog->attachTo(get2DLayer());
		_errorLog->setTextPosition(osg::Vec3(10, win->getStoredHeight() - 10, 999.9));
    }
	
	Display* main_display = DisplayCollection::instance()->getDisplay("main");
	if (main_display) 
	{
		main_display->get3DWorld()->addChild(g);
	}
	else 
	{
		osg::notify(osg::FATAL) << "AppController::applyWorld: no valid main-display given, giving up" << std::endl;
	}
}

// ----------------------------------------------------------------------------------------------------------
// handleAddErrorLogMessage
// ----------------------------------------------------------------------------------------------------------

void AppController::handleAddErrorLogMessage(const std::string& msg) {
	if (_errorLog.valid()) {
        _errorLog->add(msg);
    } else {
        osg::notify(osg::ALWAYS) << msg << std::endl;
    }
    
}


// ----------------------------------------------------------------------------------------------------------
// dtor
// ----------------------------------------------------------------------------------------------------------

AppController::~AppController()
{
    cefix::DataFactory::instance()->setAppController(NULL);
	if (_data) delete _data;
}
namespace priv {

#if defined(WIN32)

#include <windows.h>
static void hideConsole() {
	FreeConsole();
}
#else
	static void hideConsole(){}
#endif
}


osg::Group* AppController::get2DLayer(const std::string& identifier)
{
	initImplementation();
	Display* d = DisplayCollection::instance()->getDisplay(identifier);
	return d ? d->get2DWorld() : NULL;
}

void AppController::exportSnapshot(const std::string& filename, unsigned int width, unsigned int height, bool saveTiles, bool hide2dLayer) 
{ 
	unsigned int hudnodemask = get2DLayer()->getNodeMask();
	if (hide2dLayer)
		get2DLayer()->setNodeMask(0x0);
	_mainWindow->exportSnapshot(filename, width, height, saveTiles); 
	get2DLayer()->setNodeMask(hudnodemask);
}

void AppController::requestPicking(ApplicationWindow* win) {
	if (!win) win = getMainWindow();
	if (win) win->requestPicking();
}


void AppController::realize() {
		
	osgViewer::CompositeViewer::realize();	

	for (unsigned int i = 0; i < getNumViews(); ++i) {
		ApplicationWindow* w = dynamic_cast<ApplicationWindow*>(getView(i));
		if (w) w->realize();
	}

	// Consolen-Fenster ggf. ausblenden
	if (cefix::DataFactory::instance()->getPreferencesFor("/debug/showConsole",1) == 0)
		priv::hideConsole();
}

void AppController::oneframe() 
{
	perFrame();
}

int AppController::prepareLaunch() 
{
	initImplementation();
	
	if (_useHeartBeatTimer) {
		_data->heart_beat = new HeartBeatTimer();
		_data->heart_beat->start();
	}
	
	osgViewer::CompositeViewer::viewerInit();

	if (!isRealized())	
		realize();
	if (!isRealized())
		return 1;
	

    cefix::notify("initFinished");
    
    _data->qtmm = cefix::VideoMediaManager::instance();
    _data->af = cefix::AnimationFactory::instance();
	
	return 0;
}
    

void AppController::perFrame() {
    if (_statisticsEnabled && _data->stats.valid()) {
        perFrameImpl<1>();
    } else {
        perFrameImpl<0>();
    }
}

template <int T>
void AppController::perFrameImpl()
{
    static osg::Timer t;
    static double video_duration, animation_duration, event_duration, update_duration, cull_and_draw_duration, frame_duration;
    static osg::Timer_t last, last_frame;
    
	if (T) {
        last = t.tick();
        last_frame = t.tick();
    }
    
	if (_frameCallback.valid()) _frameCallback->operator()(this);
	
	if (T) last = t.tick();
	_data->qtmm->idle();
	if (T) video_duration = t.delta_m(last, t.tick());
	
    if (DebugGeometryFactory::inUse()) DebugGeometryFactory::reset();
    
    if((getRunFrameScheme() == osgViewer::ViewerBase::CONTINUOUS) || checkNeedToDoFrame()) {
        if (T) last = t.tick();
        advance();
        eventTraversal();
        if (T) event_duration = t.delta_m(last, t.tick());
        
        if (T) last = t.tick();
        _data->af->animate();
        if (T) animation_duration = t.delta_m(last, t.tick());
            
        if (T) last = t.tick();
        updateTraversal();		
        if (DebugGeometryFactory::inUse()) DebugGeometryFactory::finish();
        if (T) update_duration = t.delta_m(last, t.tick());
        
        if (T) last = t.tick();
        renderingTraversals();
    }
	if (T) {
        cull_and_draw_duration = t.delta_m(last, t.tick());
	
        frame_duration = t.delta_m(last_frame, t.tick());
        last_frame = t.tick();

        if (_statisticsEnabled && _data->stats.valid()) {
            _data->stats->setVideoStats(video_duration);
            _data->stats->setAnimationStats(animation_duration);
            _data->stats->setEventStats(event_duration);
            _data->stats->setUpdateStats(update_duration);
            _data->stats->setCullAndDrawStats(cull_and_draw_duration);
            _data->stats->setFrameDuration(frame_duration);
        }
    }
} 
	

void AppController::prepareQuit() 
{
	tearDown();
    
    while (getNumViews() > 0) {
		removeView(getView(0));
	}
	setMainWindow(NULL);
	
	osg::ref_ptr<DisplayCollection> dc = DisplayCollection::instance();
    if (dc) dc->clear();
	
	if (_data->heart_beat.valid())
		_data->heart_beat->stop();
   
	// wichtig: datafactory muss zuerst geleert werden
    
    osg::setNotifyHandler(NULL);
    
	osgDB::Registry::instance()->clearObjectCache();

	DataFactory::instance()->cleanUp();
    cefix::VideoMediaManager::instance()->cleanup();	
}


int AppController::run() 
{
	int result = prepareLaunch();
	if (result) return result;
	
	while(!done()) 
	{
		perFrame();
	}
    
    prepareQuit();
	
    return 0;
}


PostScriptWriter* AppController::getPostScriptWriter() {
	return _data->postscriptWriter.get();
}

void AppController::requestPostScriptWriter(ApplicationWindow* win) 
{
	bool success = false;
		
	if (!win) win = getMainWindow();
	
	osg::Camera* cam = win->getCamera();
	if (!cam) {
		log::error("AppController") << "requestPostScriptWriter failed, no valid camera" << std::endl;
		return;
	}
	
	osgViewer::Renderer* renderer = dynamic_cast<osgViewer::Renderer*>(cam->getRenderer());

	if ((renderer)){
		osgUtil::SceneView* sv = renderer->getSceneView(0);
		if (sv) {
			_data->postscriptWriter = PostScriptWriter::install(sv);
			success = true;
		}
	}
	
	if(!success) {
		log::error("AppController") << "could not register postscriptwriter " << std::endl;
	}
	
}

// ----------------------------------------------------------------------------------------------------------
// addStatistics
// ----------------------------------------------------------------------------------------------------------

void AppController::enablePerformanceStatistics(bool f, bool simple) 
{

	_statisticsEnabled = f; 
	if (_statisticsEnabled)
		_data->stats = new Stats(simple);
	else
		_data->stats = NULL;
}


void AppController::setupPerformanceStatistics(bool simple, const std::string& display_id) 
{
	enablePerformanceStatistics(true, simple);
	cefix::Display* display = DisplayCollection::instance()->getDisplay(display_id);
    if (!display) {
        cefix::log::error("AppController") << "could not setup performance-stats for display " << display_id << std::endl;
        return;
    }
    
    getPerformanceStatisticsGroup()->setMaxDimensions(display->get2DConfiguration().width-150, 100);
		
	osg::MatrixTransform* mat = new osg::MatrixTransform(); 
	mat->setMatrix(osg::Matrix::translate(25, display->get2DConfiguration().height - 125,990));
	mat->addChild(getPerformanceStatisticsGroup());
	
	display->get2DWorld()->addChild(mat);

}

void AppController::setDisplayCollection(DisplayCollection* dp)
{
	_displayCollection = DisplayCollection::instance(dp);
}
// ----------------------------------------------------------------------------------------------------------
// getStatisticsGroup
// ----------------------------------------------------------------------------------------------------------

cefix::StatisticsGroup* AppController::getPerformanceStatisticsGroup() 
{
	return (_data->stats.valid()) ? _data->stats->getStatisticsGroup() : NULL; 
}


void AppController::registerCustomPerformanceStatisticsValue(const std::string& key) {
	if ((_statisticsEnabled) && (_data->stats.valid()))
		_data->stats->registerCustomStats(key);
}


void AppController::addCustomPerformanceStatisticsValue(const std::string& key, float value) {
	if ((_statisticsEnabled) && (_data->stats.valid()))
		_data->stats->add(key, value);
}



RebootObserver* AppController::getRebootObserver()  
{
	return _data->rebootObserver.get();
}

/*
void AppController::setupTiledRendering(float width, float height, const osg::Vec4& tileRect, cefix::WindowRotation::Rotation rotation, ApplicationWindow* win, bool applyTo2D) 
{
	if (!win) win = getMainWindow();
	osg::Matrix projection = win->getCamera()->getProjectionMatrix();
	
	TiledProjectionHelper pph(width, height);
	win->getCamera()->setProjectionMatrix(projection * pph.getOffsetForProjection(tileRect[0], tileRect[1], tileRect[2] - tileRect[0], tileRect[3] - tileRect[1]) );
	
	if (applyTo2D) {
		_hudCamera->setProjectionMatrix(_hudCamera->getProjectionMatrix() * pph.getOffsetForOrtho(tileRect[0], tileRect[1], tileRect[2] - tileRect[0], tileRect[3] - tileRect[1]) );
	}
		
}

*/


}


#if defined(CEFIX_LIBRARY_STATIC)
USE_GRAPHICSWINDOW();
#endif

#ifdef CEFIX_FOR_IPHONE
USE_GRAPICSWINDOW_IMPLEMENTATION(IOS);
USE_OSGPLUGIN(imageio);
USE_OSGPLUGIN(avfoundation);
#endif