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

#ifndef APPLICATION_WINDOW_HEADER
#define APPLICATION_WINDOW_HEADER
#include <osgGA/TrackballManipulator>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/View>

#include <cefix/ViewPointManipulator.h>
#include <cefix/WindowRotation.h>
#include <cefix/DisplayCollection.h>

#ifdef __APPLE__
    #ifdef CEFIX_FOR_IPHONE
        #include <osgViewer/api/IOS/GraphicsWindowIOS>
    #else
        #include <osgViewer/api/Cocoa/GraphicsWindowCocoa>
    #endif
#endif

namespace cefix {
class ApplicationWindowData;
class PickEventHandler;
class WindowPickable;

class CEFIX_EXPORT ApplicationWindow : public osgViewer::View {
	
	public:
    
        #ifdef __APPLE__
        #ifdef CEFIX_FOR_IPHONE
        typedef UIView NativeView;
        typedef UIWindow NativeWindow;
		#else
        typedef NSView NativeView;
        typedef NSWindow NativeWindow;
        #endif
        #else
        typedef void* NativeView;
        typedef void* NativeWindow;
        #endif
				
		ApplicationWindow(WindowConfiguration* data);
		ApplicationWindow(float w, float h, WindowRotation::Rotation rot = WindowRotation::NONE);
		
		void setFullscreen(bool flag);
		
		
		int getStoredHeight() const { return _height; }
		int getStoredWidth() const  { return _width; }
        
        int getActualWidth() const { int x,y,w,h; getGraphicsWindow()->getWindowRectangle(x,y,w,h); return w; }
        int getActualHeight() const { int x,y,w,h; getGraphicsWindow()->getWindowRectangle(x,y,w,h); return h; }
		
		WindowRotation::Rotation getRotation() { return _configuration->getWindowRotation(); }
		void setRotation(WindowRotation::Rotation rotation) { _configuration->setWindowRotation(rotation); }
		
		void setSceneData(osg::Group* group);
		
		osgGA::TrackballManipulator* getTrackballManipulator() { return _trackballManipulator.get(); }
		void setTrackballManipulator(osgGA::TrackballManipulator* trackball) { _trackballManipulator = trackball; }
		
		ViewPointManipulator* getViewPointManipulator() const { return _viewpointManipulator.get(); }
		
		PickEventHandler* getPickEventHandler() const { return _pickEventHandler.get(); }
		
		virtual void realize();
		
		osg::Camera* createHudCamera(osg::Group* camera_childs, int aw = 0, int ah = 0, const osg::Matrix& offset = osg::Matrix());

		osg::Camera* createHudCamera(int aw = 0, int ah = 0, const osg::Matrix& offset = osg::Matrix())
		{
			return createHudCamera(NULL, aw, ah, offset);
		}
		
		bool exportSnapshot(const std::string& filename, unsigned int width, unsigned int height, bool saveTiles = false, bool withalpha = false);
		
		bool getFullscreen() { return _fullscreen; }
		
		void requestPicking();
		
		osgViewer::GraphicsWindow* getGraphicsWindow() const { return _window.get(); }
		
		const std::string& getPrefsKey() { return _prefsKey; }
		
		void setWindowTitle(const std::string& title) { if (_window.valid()) getGraphicsWindow()->setWindowName(title); }
		std::string getWindowTitle() { return (_window.valid()) ? getGraphicsWindow()->getWindowName() : ""; }
		
		
		void initWithViewPointManipulatorWhenValid(bool b) { _useViewPointManipulatorWhenValid = b; }
		
		bool hasTrackballManipulator() const { return _hasTrackballManipulator; }
		void setHasTrackballManipulator(bool f) { _hasTrackballManipulator = f; }
		
		void useCursor(bool cursorOn) { _useCursor = cursorOn; if (_window.valid()) getGraphicsWindow()->useCursor(cursorOn); }
		void setShowMouseCursor(bool flag) { useCursor(flag); }
		void hideCursor() { useCursor(false); }
		void showCursor() { useCursor(true); }
		
		void setWindowRectangle(int w, int h) { setWindowRectangle(-1, -1, w, h); }
		void setWindowRectangle(int l, int t, int w, int h);
		
		WindowPickable* getWindowPickable() const;
		void setMultiTouchEnabled(bool b); 
		bool isMultiTouchEnabled() const { return _isMultiTouchEnabled; }
        
        #ifdef __APPLE__
        #ifdef CEFIX_FOR_IPHONE
        NativeView* getNativeView() { return (NativeView*)(dynamic_cast<osgViewer::GraphicsWindowIOS*>(_window.get()))->getView(); }
        NativeWindow* getNativeWindow() { return (NativeWindow*)(dynamic_cast<osgViewer::GraphicsWindowIOS*>(_window.get()))->getWindow(); }
        #else
        NativeView* getNativeView() { return NULL; }
        NativeWindow* getNativeWindow() { return (NativeWindow*)(dynamic_cast<osgViewer::GraphicsWindowCocoa*>(_window.get()))->getWindow(); }
        #endif
        #else
        NativeView* getNativeView() { return NULL; }
        NativeWindow* getNativeWindow() { return NULL; }
        #endif
		
	protected:
		~ApplicationWindow();
	private:
		void init();
		void preInit();
		
		void createView(WindowConfiguration* data);
		
		
		
		int _left, _top, _width, _height;
		bool _fullscreen, _decoration, _useFSAA;
		osg::ref_ptr<osgViewer::GraphicsWindow>	_window;
		osg::ref_ptr<WindowConfiguration>		_configuration;
		
		osg::ref_ptr<ViewPointManipulator>			_viewpointManipulator;
		osg::ref_ptr<osgGA::TrackballManipulator>	_trackballManipulator;
		osg::ref_ptr<PickEventHandler>				_pickEventHandler;
		std::string									_prefsKey;
		bool										_useViewPointManipulatorWhenValid, 
													_hasTrackballManipulator, 
													_windowRealized, 
													_useCursor,
													_hasCameraManipulator;
		bool										_isMultiTouchEnabled;
				



};

} // end of namespace
#endif