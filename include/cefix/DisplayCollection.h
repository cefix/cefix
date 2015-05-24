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

#ifndef CEFIX_DISPLAY_COLLECTION_HEADER
#define CEFIX_DISPLAY_COLLECTION_HEADER

#include <cefix/Serializer.h>
#include <cefix/WindowRotation.h>
#include <cefix/DisplayMaskController.h>
#include <osg/GraphicsContext>
#include <osg/Camera>

namespace cefix {

class ApplicationWindow;
class WindowConfiguration;
class DisplayCollection;
class TiledDisplay;

/** a display class holds the configuration for a display, that's the 2D-configuration, the 3d-configuration and optionally a viewport
*/
class Display : public osg::Referenced {
	
public:
	/** the configuration2D-class stores width and height */
	struct Configuration2D {
		Configuration2D() : width(0), height(0) {}
		Configuration2D(float w, float h) : width(w), height(h) {}
		
		float width, height;
		
		void readFrom(Serializer& serializer);
		void writeTo(Serializer& serializer);
		bool valid() const { return ((width > 0) && (height > 0)); }

	};
	
	/** the Configuration3DPerspective holds parameters definiing a perspective projection matrix, 
	    namely a horizontalFieldOfView and a verticalFieldOfView
	 */
	struct Configuration3DPerspective {
		Configuration3DPerspective() : horizontalFieldOfView(), verticalFieldOfView() {}
		Configuration3DPerspective(float h, float v) : horizontalFieldOfView(h), verticalFieldOfView(v) {}
		
		float horizontalFieldOfView, verticalFieldOfView;
		
		void readFrom(Serializer& serializer);
		void writeTo(Serializer& serializer);
		
	};
	
	/** Configuration3DFrustum hold all parameters to construct a frustum from three given points (topleft, bottomleft and bottomright)
		and an eye-point (which is stored inside the DisplayCollection-class) */
	struct Configuration3DFrustum {
	
		Configuration3DFrustum() : topLeftCorner(), bottomLeftCorner(), bottomRightCorner() {}
		Configuration3DFrustum(const osg::Vec3& tl, const osg::Vec3& bl, const osg::Vec3& br) : 
			topLeftCorner(tl), 
			bottomLeftCorner(bl), 
			bottomRightCorner(br) 
		{
		}
	
		osg::Vec3 topLeftCorner, bottomLeftCorner, bottomRightCorner;
		
		void readFrom(Serializer& serializer);
		void writeTo(Serializer& serializer);
		
	};
	
	/** Configuration3D stores a Configuration3DPerspective and a Configuration3DFrustum object, and the near and far-plane. */
	struct Configuration3D {
	
		Configuration3D(float hfov, float vfov, float n, float f) : 
			isFrustum(false), 
			nearPlane(n), 
			farPlane(f), 
			perspective(hfov, vfov), 
			frustum() 
		{
		}
		
		Configuration3D(const osg::Vec3& tl, const osg::Vec3& bl, const osg::Vec3& br, float n, float f) : 
			isFrustum(true), 
			nearPlane(n), 
			farPlane(f), 
			perspective(), 
			frustum(tl, bl, br) 
		{
		}
		
		Configuration3D() : isFrustum(false), nearPlane(1), farPlane(1000.0), perspective(60,-1), frustum() {}
	
		bool isFrustum;
		float nearPlane, farPlane;
		
		Configuration3DPerspective perspective;
		Configuration3DFrustum frustum;
		
		void readFrom(Serializer& serializer);
		void writeTo(Serializer& serializer);
		
	};
	
	/** c'tor, identifier is used to indentify a display globally */
	Display(const std::string& identifier = "main");
	
	
	const Configuration2D& get2DConfiguration() const { return _2d; }
	const Configuration3D& get3DConfiguration() const { return _3d; }
	
	Configuration2D& get2DConfiguration() { return _2d; }
	Configuration3D& get3DConfiguration() { return _3d; }
	
	const std::string& getIdentifier() const { return _identifier; }
	
	void set2DConfiguration(const Configuration2D& data) { _2d = data; }
	void set2DConfiguration(float w, float h) { _2d = Configuration2D(w,h); }
	
	void set3DConfiguration(const Configuration3D& data) { _3d = data; }
	
	void setViewport(float x, float y, float width, float height) { _viewport->setViewport(x,y,width, height); }
	
	virtual void readFrom(Serializer& serializer);
	virtual void writeTo(Serializer& serializer);
	
	/** get a group holding the 3d-world of this display */
	osg::Group* get3DWorld() { return _3dWorld.get(); }
	
	/** get a group holding the 2d-world of this display */
	osg::Group* get2DWorld() { return _2dWorld.get(); }
	
	/** get the 3d-camera */
	osg::Camera* get3DCamera() { return _3dCamera.get(); }
	
	/** get the 2d-camera */
	osg::Camera* get2DCamera() { return _2dCamera.get(); }
	
	/** get the corresponding window */
	cefix::ApplicationWindow* getWindow() const { return _window; }
	
	/** apply the configuration to the given camera */
	virtual void applyTo(osg::Camera* camera);
	
	/** returns true, if this display is a master-display */
	bool isMaster() const { return _isMaster; }
	
	/** returns true, if this display is a slave-display */
	bool isSlave() const { return _isSlave; }
	 
	void setIsMaster(bool f) { _isMaster = f; }
	void setIsSlave(bool f, const std::string& main_display_id = "") { _isSlave = f; _mainDisplayId = main_display_id; }
	
	const osg::Matrix& getSlaveProjectionOffset() const { return _projOffset; }
	const osg::Matrix& getSlaveViewOffset() const { return _viewOffset; }
	bool getSlaveUseMasterSceneData() const { return _useMasterSceneData; }
	
	/** set the slave configuration, the projection-offset, the view-offset */
	void setSlaveConfiguration(const osg::Matrix& proj_offset, const osg::Matrix& view_offset, bool use_master_scene) 
	{
		_projOffset = proj_offset;
		_viewOffset = view_offset;
		_useMasterSceneData = use_master_scene;
		_isSlave = true;
	}

	/** get the viewport */
	osg::Viewport* getViewport() const  { return _viewport.get(); }
	
	/** get the window configuration */
	WindowConfiguration* getWindowConfiguration() const { return _windowConfiguration; }
	
	/** get this display as a TiledDisplay */
	virtual TiledDisplay* asTiledDisplay() { return NULL; }
	
	/** set the MainDisplayId, needed for master-slave configurations */
	void setMainDisplayId(const std::string& s) { _mainDisplayId = s; }
	
	/** get the mainDisplayId */
	const std::string& getMainDisplayId() const { return _mainDisplayId; }
	
	/** get the main-display */
	Display* getMainDisplay() const;
	
	/** get the displaycollection holding this display */
	DisplayCollection* getDisplayCollection() const;

	void setDisplayMaskController(DisplayMaskController* controller) { _displayMaskController = controller; }
	DisplayMaskController* getDisplayMaskController() { return _displayMaskController; }
protected:
	
	void set2DWorld(osg::Group* g) { _2dWorld = g; }
	void set3DWorld(osg::Group* g) { _3dWorld = g; }
	
	/** set the window, the display belongs to */
	void setWindow(cefix::ApplicationWindow* window) { _window = window; }
	
	/** set the windowconfiguration the display belongs to */ 
	void setWindowConfiguration(WindowConfiguration* conf) {_windowConfiguration = conf;}
	
protected:
	osg::ref_ptr<osg::Viewport> _viewport;
	Configuration2D _2d;
	Configuration3D _3d;
	std::string _identifier;
	osg::ref_ptr<osg::Group> _2dWorld, _3dWorld;
	osg::ref_ptr<osg::Camera> _2dCamera, _3dCamera;
	cefix::ApplicationWindow*	_window;
	WindowConfiguration*		_windowConfiguration;
	bool						_isMaster, _isSlave;
	osg::Matrix					_projOffset, _viewOffset;
	bool						_useMasterSceneData;
	std::string _mainDisplayId;
	osg::ref_ptr<DisplayMaskController>	_displayMaskController;
	std::string _displayMaskControllerFileName;

friend class DisplayCollection;
friend class WindowConfiguration;
	
};

/** a display's subclass describing all parameters needed for tiled-rendering. */
class TiledDisplay: public Display {

public:
	/// l, t, w, h defines the cutout-rect, main_display_id holds the identifier for the display which defines the overall characteristics
	TiledDisplay(const std::string& identifier, const std::string& main_display_id, float l, float t, float w, float h, bool is_slave);
	
	/// rect defines the cutout-rect, main_display_id holds the identifier for the display which defines the overall characteristics 
	TiledDisplay(const std::string& identifier, const std::string& main_display_id, const osg::Vec4& rect, bool is_slave);
	TiledDisplay() : Display(), _applyTo2D(false) {}
	
	/// apply the configuration to the given camera
	virtual void applyTo(osg::Camera* Camera);
	
	void readFrom(Serializer& serializer);
	void writeTo(Serializer& serializer);
	
	virtual TiledDisplay* asTiledDisplay() { return this; }
	
	
private:
	
	osg::Vec4 _clippingRect;
	bool _applyTo2D;

};

/** a display's subclass defining a custom projection matrix, suitable for complex frustum based slave-configurations */
class CustomProjectionDisplay : public Display {

public:
	/// ctor
	CustomProjectionDisplay(const std::string& identifier, const osg::Matrix& projection) : Display(identifier), _projection(projection) {}
	CustomProjectionDisplay() : Display(), _projection() {}
	
	/// apply the configuration to the given camera
	virtual void applyTo(osg::Camera* Camera);
	virtual void readFrom(Serializer& serializer);
	virtual void writeTo(Serializer& serializer);
protected:
	osg::Matrix _projection;
};


class DisplayCollection;

/** the windowConfiguration-class encapsulates all parameters describing a window to use for rendering. Every window holds one or more Display-objects*/
class WindowConfiguration : public osg::Referenced {

public:
	typedef std::vector<osg::ref_ptr<Display> > DisplayVector;
	WindowConfiguration();
	
	void readFrom(Serializer& serializer);
	void writeTo(Serializer& serializer);
	
	/** get num displays of this window */
	unsigned int getNumDisplays() const { return _displays.size(); }
	
	/// get display at position i 
	Display* getDisplay(unsigned int i) { return _displays[i].get(); }
	
	/// get the traits-object describing this window-configuration 
	osg::GraphicsContext::Traits* getTraits() { return _traits.get(); }
	
	/// add a display-configuration to this window
	void addDisplay(Display* display) { _displays.push_back(display); }
	
	/// set the window-rotation 
	void setWindowRotation(WindowRotation::Rotation r) { _rotation = r; }
	
	/// get the window rotation 
	WindowRotation::Rotation getWindowRotation() const { return _rotation; }
	
	// if true, this window poses as the main-window, cefix::AppController::getMainWindow()
	bool poseAsMainWindow() const {return _poseAsMainWindow; }
	
	/// sets the traits-object 
	void setTraits(osg::GraphicsContext::Traits* traits) { _traits = traits; }
		
	/// creates a suitable standard display configuration
	Display* createStandardDisplay(const std::string& identifier = "main");
	
	bool hasStatsHandler() const { return _hasStatsHandler; }
	bool hasStateHandler() const { return _hasStateHandler; }
	bool hasViewerHandler() const { return _hasViewerHandler; }
	
	void setHasStatsHandler(bool flag) { _hasStatsHandler = flag; }
	void setHasStateHandler(bool flag) { _hasStateHandler = flag; }
	void setHasViewerHandler(bool flag) { _hasViewerHandler = flag; }
	
	void setTitle(const std::string& title) { _traits->windowName = title; }
	const std::string& getTitle() const { return _traits->windowName; }
	
	/// static helper function, creates a windowconfiguration for a fullscreen-window on screen si
	static WindowConfiguration* createFullScreen(osg::GraphicsContext::ScreenIdentifier si);
    
    enum ComputeWindowCoordsPolicy { COMPUTE_LAZY, COMPUTE_NOW };
	
	/// static helper function, creates a windowconfiguration for a centered window on screen si, if lazy is false, the coordinates are computed asap
	static WindowConfiguration* createCenteredWindowOnScreen(osg::GraphicsContext::ScreenIdentifier si, unsigned int w, unsigned h, ComputeWindowCoordsPolicy policy = COMPUTE_LAZY);
	
	/// static helper function, creates a windowconfiguration for a window on screen si
	static WindowConfiguration* createWindowOnScreen(osg::GraphicsContext::ScreenIdentifier si, unsigned int left, unsigned int top, unsigned int w, unsigned h);
	
	/// set to false to disable this particulare window-configuration
	void setIsEnabled(bool f) { _enabled = f; }
	
	/// return true if enabled
	bool isEnabled() const { return _enabled; }
	
	/// return true, if this window should share the graphics-context of the main-window
	bool shareContextWithMainWindow() const { return _shareContextWithMainWindow; }
	
	/// set to true if this configuration should share its ghrapicscontext with the main-window
	void setShareContextWithMainWindow(bool b) { _shareContextWithMainWindow = b; }
	
    void setIsFullScreen(bool b) { _isFullScreen = b; }
    
    bool isFullScreen() { return _isFullScreen; }
    
    void setMultiTouchEnabled(bool f) { _multiTouchEnabled = f; }
    bool isMultiTouchEnabled() const { return _multiTouchEnabled; }
    
protected:
	/// sets the window created by this configuration
	void setWindow(cefix::ApplicationWindow* window);
	
	/// preflight the configuration
	void preflight();
	
	/// returns false if this configuration describes a window which is slave or something similar
	bool mayBeAddedToViewer() const { return _mayBeAddedToViewer; }

	void setDisplayCollection(DisplayCollection* dc) { _displayCollection = dc; }
	DisplayCollection* getDisplayCollection() const { return _displayCollection; }
	
	
private:
	osg::ref_ptr<osg::GraphicsContext::Traits>			_traits;
	DisplayVector										_displays;
	WindowRotation::Rotation							_rotation;
	bool												_poseAsMainWindow, _hasContext;
	bool												_hasStatsHandler, _hasStateHandler, _hasViewerHandler;
	DisplayCollection*									_displayCollection;
	bool												_mayBeAddedToViewer, _enabled, _shareContextWithMainWindow;
    bool                                                _isFullScreen, _multiTouchEnabled;
	
	friend class DisplayCollection;
	friend class Display;
	friend class TiledDisplay;
	
};

class AppController;
class DisplayCollection : public osg::Referenced {

public:
	typedef std::map<std::string, osg::ref_ptr<Display> > DisplayMap;
	typedef std::vector<osg::ref_ptr<WindowConfiguration> > WindowConfigurationVector;
	typedef DisplayMap::iterator iterator;
	
	DisplayCollection();
	
	inline bool hasDisplay(const std::string& identifier) const { return (_displays.find(identifier) != _displays.end()); }
	Display* getDisplay(const std::string& identifier) { return (hasDisplay(identifier)) ? _displays[identifier] : NULL; }
	bool readConfigurationFromPrefs(const std::string& prefsKey);
	bool loadConfigurationFromFile(const std::string filename);
	void saveConfigurationToFile(const std::string filename);
	void createDefaultConfiguration();
	void applyConfiguration(cefix::AppController* ctrl);
	
	static DisplayCollection* instance(DisplayCollection* dp=NULL);
	
	void addDisplay(Display* display);
	void addWindowConfiguration(WindowConfiguration* data) { _windowConfigurations.push_back(data);  _needsPreflight = true; }
	
	const DisplayMap& getDisplays() const { return _displays; }
	const WindowConfigurationVector& getWindowConfigurations() const { return _windowConfigurations; }
	
	void readFrom(Serializer& serializer);
	void writeTo(Serializer& serializer);
	
	void clear() { _displays.clear(); _windowConfigurations.clear(); }
	
	iterator begin() { return _displays.begin(); }
	iterator end() { return _displays.end(); }
	
	void setEyePosition(const osg::Vec3& eye_pos) { _eyePos = eye_pos; }
	const osg::Vec3& getEyePosition() const { return _eyePos; }
	
    void setCreateDefaultConfigurationWhenEmpty(bool f) { _createDefaultConfigurationWhenEmpty = f; }
    bool createDefaultConfigurationWhenEmpty() const { return _createDefaultConfigurationWhenEmpty; } 
protected:
	void preflight();
	void connectSingleSlaves(cefix::AppController* ctrl);
	void collectUnConnectedDisplays(DisplayMap& unconnected_displays);
private:
	DisplayMap		_displays;
	osg::Vec3		_eyePos;
	WindowConfigurationVector _windowConfigurations;
	bool			_needsPreflight, _createDefaultConfigurationWhenEmpty;
};

void readFromSerializer(cefix::Serializer& s, osg::Viewport& vp);
void writeToSerializer(cefix::Serializer& s, osg::Viewport& vp);


}


#endif