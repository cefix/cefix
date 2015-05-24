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

#ifndef CEFIX_SKETCH_HEADER
#define CEFIX_SKETCH_HEADER

#include <cefix/AppController.h>

namespace cefix {

/** small helper class mimicking a processing sketch for rapid development, bundles all mouse- and keyboard-events */

class SketchBase : public AppController {

public:
    enum DrawCallbackUsage { UseNoCallback = 0, UsePreDrawCallback, UsePostDrawCallback } ;
	/** ctor */
	SketchBase();
	/** init is called right after the constructor, here you can setup your windows etc, do not create your scene here */
	virtual void init();
	 
    virtual void update() {}
    
	
	/** get the root node */
	osg::Group* getWorld() { return _world.get(); }
	
	/** get the width of the main window */
	int getWidth() { return getMainWindow()->getActualWidth(); }
	
	/** getthe height of the main window */
	int getHeight() { return getMainWindow()->getActualHeight(); }
	
	/** set the app-path */
	static void setApplicationPath(char* argv, const std::string& mediafoldername = "media");

	/** get the width of screen screenNum */
	static unsigned int getScreenWidth(unsigned int screenNum = 0);
	
	/** get the height of screen screenNum */
	static unsigned int getScreenHeight(unsigned int  screenNum = 0);
	
	/** get the width and height  of screen screenNum */
	static void setScreenSize(unsigned int w, unsigned int h, unsigned int screenNum=0);
	
	bool blockOtherInputHandler;

	/// prepare the sketch for launch
	virtual int prepareLaunch();
	
	virtual void setDrawCallbackUsage(DrawCallbackUsage usage, osg::Camera::DrawCallback* callback = NULL);
    DrawCallbackUsage getDrawCallbackUsage() const { return _drawCallbackUsage; }

	
private:	
	
	
        
   
	
protected:	
    	
	virtual osg::Group* createWorld() 
	{
		initImplementation();
		_world = new osg::Group();
		
		return _world.get();
	}
	
		
protected:
	/** set the window size */
	void size(int w, int h);
	
	/** set the window title */
	void title(const std::string& t) 
	{
		getMainWindow()->setWindowTitle(t);
	}
	
	/** set to true, if you want the standard osg manipulator */
	void allowOsgHandler(bool f) 
	{
		blockOtherInputHandler = !f;
	}

private:
	osg::ref_ptr<osg::Group> _world;
    DrawCallbackUsage _drawCallbackUsage;
    osg::ref_ptr<osg::Camera::DrawCallback> _drawCallback;
};


class Sketch : public SketchBase {
public:
	Sketch();
	
	/** setup your scene */
	virtual void setup() {}
   
    virtual void draw() {}
	
	/** mouse clicked */
	virtual void mouseClicked() {}
	
	/** mouse down */
	virtual void mousePressed() {}
	
	/** mouse up */
	virtual void mouseReleased() {}
	
	/** mouse moved */
	virtual void mouseMoved() {}
	
	/** mouse dragged */
	virtual void mouseDragged() {}
    
    virtual void touchBegan(unsigned int touch_id, osgGA::GUIEventAdapter::TouchData* touch_data) {  }
    
    virtual void touchMoved(unsigned int touch_id, osgGA::GUIEventAdapter::TouchData* touch_data) {  }
    
    virtual void touchIsStationary(unsigned int touch_id, osgGA::GUIEventAdapter::TouchData* touch_data) {  }
    
    virtual void touchEnded(unsigned int touch_id, osgGA::GUIEventAdapter::TouchData* touch_data) {   }
	
	/** key down */
	virtual void keyPressed() {}
	
	/** key up */
	virtual void keyReleased() {}
	
	/** key typed */
	virtual void keyTyped() {}
	
	virtual int prepareLaunch();


	unsigned int key;
	int mouseX, mouseY;
	bool isMousePressed, isKeyPressed;
	int mouseButton;

protected:
    virtual void setDrawCallbackUsage(DrawCallbackUsage usage, osg::Camera::DrawCallback* callback = NULL);
	
	virtual osg::Group* createWorld() 
	{
		osg::Group* g = SketchBase::createWorld();
		setup();
		return g;
	}
    
    virtual void updateTraversal() {
		cefix::AppController::updateTraversal();
		update();
        if(getDrawCallbackUsage() == UseNoCallback) 
            draw();
	}

};

}


#endif