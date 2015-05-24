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

#ifndef APPCONTROLLER_HEADER
#define APPCONTROLLER_HEADER

#include <osgUtil/Optimizer>

#include <osgViewer/CompositeViewer>
#include <cefix/NotificationListener.h>
#include <cefix/WindowRotation.h>
#include <cefix/ApplicationWindow.h>

namespace cefix {

	class Logger;
	class StatisticsGroup;
	class PrivateData;
	class PostScriptWriter;
	class DisplayCollection;
	class RebootObserver;
	
	class CEFIX_EXPORT AppController : public osgViewer::CompositeViewer, public cefix::NotificationListener {
		public:
		
			class FrameCallback : public osg::Referenced {

				public:
					FrameCallback() : osg::Referenced() {}
					
					virtual void operator()(AppController* ctrl)  = 0;				
			};

			AppController(int loggerMode = 0);
			
			virtual osg::Node* createWorld() {return new osg::Group(); }
			
			void applyWorld(osg::Node* node, ApplicationWindow* win = NULL);
			
			void realize();
			
			void oneframe();
			virtual int run(); 
			
			ApplicationWindow* getMainWindow() { if (!_mainWindow.valid()) initImplementation(); return _mainWindow.get(); }
			void setMainWindow(ApplicationWindow* win) { _mainWindow = win; }
			
			void handleAddErrorLogMessage(const std::string& msg);
			
			void setUseOptimizerFlag(bool flag) { _useOptimizer = flag; }
			
			osg::Group* get2DLayer(const std::string& identifier="main");
			
			void setOptimizerCallback(osgUtil::Optimizer::IsOperationPermissibleForObjectCallback* cb) { _optimizerCallback = cb; }
					
			void handleRequestRedraw();
			
			
			void exportSnapshot(const std::string& filename, unsigned int width, unsigned int height, bool saveTiles = false, bool hide2dLayer = true);
						
			void requestPicking(ApplicationWindow* win = NULL);
						
			void requestPostScriptWriter(ApplicationWindow* win = NULL);


			virtual void enablePerformanceStatistics(bool f, bool simple_stats = false);
			
			
			/// enable performance statistics and add them to the 2d-layer
			void setupPerformanceStatistics(bool simple_stats = false, const std::string& display_id = "main");

			cefix::StatisticsGroup* getPerformanceStatisticsGroup();
			void addCustomPerformanceStatisticsValue(const std::string& key, float value);
			void registerCustomPerformanceStatisticsValue(const std::string& key);
			void setFrameCallback(FrameCallback* cb) { _frameCallback = cb; }
			
			PostScriptWriter* getPostScriptWriter();
			
			/*
			void setupTiledRendering(float width, float height, const osg::Vec4& tileRect, 
						cefix::WindowRotation::Rotation rotation = cefix::WindowRotation::NONE, ApplicationWindow* win = NULL, bool applyTo2D = true);
			*/
			
			void setWindowRectangle(int w, int h, bool include2DLayer = true);
			void setFullscreen(bool f) { getMainWindow()->setFullscreen(f); }
			
			/** if set to true (standard) then the app writes every second a heart beat into a speciel lock_file, so other apps can control the healthiness of this app */
			void setUseHeartBeatTimer(bool f) { _useHeartBeatTimer = f; }
			
			
			/** init is called right after the constructor, here you can setup your windows etc, do not create your scene here */
			virtual void init() {}
            
            /** tearDown is called when the app is about to quit, but all instances (like DataFactory) are still in place */
            virtual void tearDown() {}
			
			void setDisplayCollection(DisplayCollection* dp);
			DisplayCollection* getDisplayCollection() { return _displayCollection; }
			
			RebootObserver* getRebootObserver() ; 
			
			virtual int prepareLaunch();
			void perFrame();
			void prepareQuit();
			
		protected:
            template <int T>
            void perFrameImpl();
        
            void initImplementation();
			
			~AppController();
			
			
		private:
			void setNotifyLevelFromPrefs();
			void setThreadingModelFromPrefs();
			void setStereoSettingsFromPrefs();
			void createLogFiles(int mode = 0);
			
			
			
			osg::ref_ptr<Logger>			_errorLog;
			osg::ref_ptr<ApplicationWindow> _mainWindow;
			bool							_useOptimizer, _statisticsEnabled;
			osg::ref_ptr<osgUtil::Optimizer::IsOperationPermissibleForObjectCallback> _optimizerCallback;
			PrivateData*					_data;

			osg::ref_ptr<FrameCallback>				_frameCallback;
			bool							_useHeartBeatTimer;
			bool							_inited;
			DisplayCollection*				_displayCollection;
	};
}


#endif
