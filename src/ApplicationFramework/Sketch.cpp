/*
 *  Sketch.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"
#include <cefix/PropertyList.h>
#include <cefix/Log.h>
#include <cefix/DataFactory.h>

namespace cefix {

class SketchEventHandler: public osgGA::GUIEventHandler {
public:
	SketchEventHandler(Sketch* sketch) : osgGA::GUIEventHandler(), _sketch(sketch) {}
	
    void handleMultiTouch(osgGA::GUIEventAdapter::TouchData* data) {
        
        for(osgGA::GUIEventAdapter::TouchData::iterator i = data->begin(); i != data->end(); ++i) {
            switch(i->phase) {
                case osgGA::GUIEventAdapter::TOUCH_BEGAN:
                    _sketch->touchBegan(i->id, data);
                    break;
                
                case osgGA::GUIEventAdapter::TOUCH_MOVED:
                    _sketch->touchMoved(i->id, data);
                    break;
                
                case osgGA::GUIEventAdapter::TOUCH_STATIONERY:
                    _sketch->touchIsStationary(i->id, data);
                    break;
                
                case osgGA::GUIEventAdapter::TOUCH_ENDED:
                    _sketch->touchEnded(i->id, data);
                    break;
                default:
                    break;
            }
        }
    }
	
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *o, osg::NodeVisitor *nv)
	{
        switch (ea.getEventType()) {
			case osgGA::GUIEventAdapter::FRAME:
				_sketch->mouseX = ea.getX();
				_sketch->mouseY = ea.getY();
				break;
			
			case osgGA::GUIEventAdapter::PUSH:
                if(ea.isMultiTouchEvent()) {
                    handleMultiTouch(ea.getTouchData());
                } else {
                    _sketch->isMousePressed = true;
                    _sketch->mouseButton = ea.getButton();
                    _sketch->mousePressed();
                }
				break;
			
			case osgGA::GUIEventAdapter::RELEASE:
				if(ea.isMultiTouchEvent()) {
                    handleMultiTouch(ea.getTouchData());
                } else {
                    _sketch->mouseClicked();
                    _sketch->mouseReleased();
                    _sketch->isMousePressed = false;
                    _sketch->mouseButton = 0;
                }
				break;
			
			case osgGA::GUIEventAdapter::MOVE:
				_sketch->mouseMoved();
				break;
			
			case osgGA::GUIEventAdapter::DRAG:
				if(ea.isMultiTouchEvent()) {
                    handleMultiTouch(ea.getTouchData());
                } else {
                    _sketch->mouseDragged();
				}
                break;
			
			case osgGA::GUIEventAdapter::KEYDOWN:
				_sketch->key = ea.getKey();
				_sketch->isKeyPressed =true;
				_sketch->keyPressed();
				break;
			
			case osgGA::GUIEventAdapter::KEYUP:
				_sketch->keyTyped();
				_sketch->keyReleased();
				_sketch->isKeyPressed = false;
				break;
			default:
				break;
		}
		
		return _sketch->blockOtherInputHandler;
	}
private:
	Sketch* _sketch;

};
    
    
class SketchDrawCallback: public osg::Camera::DrawCallback {
    
public:
    SketchDrawCallback(cefix::Sketch* sketch) : osg::Camera::DrawCallback(), _sketch(sketch) {}
    
    virtual void operator() (osg::RenderInfo &renderInfo) const
    {
        #ifndef OSG_GLES1_AVAILABLE
			glPushAttrib(GL_ALL_ATTRIB_BITS);
		#endif
	   
        if(_sketch->getDrawCallbackUsage() == cefix::SketchBase::UsePreDrawCallback) 
        {
            osg::Viewport* vp = renderInfo.getCurrentCamera()->getViewport();
            glViewport(vp->x(), vp->y(), vp->width(), vp->height());
            
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
			
            #ifdef OSG_USE_FLOAT_MATRIX
				{
				osg::Matrixf m(renderInfo.getCurrentCamera()->getProjectionMatrix());
				glLoadMatrixf(m.ptr());
				}
			#else
				glLoadMatrixd(renderInfo.getCurrentCamera()->getProjectionMatrix().ptr());
            #endif
            
			glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
			
			#ifdef OSG_USE_FLOAT_MATRIX
				{
					osg::Matrixf m(renderInfo.getCurrentCamera()->getViewMatrix());
					glLoadMatrixf(m.ptr());
				}
			#else
				glLoadMatrixd(renderInfo.getCurrentCamera()->getViewMatrix().ptr());
			#endif
        }
        
        _sketch->draw();
        
        if(_sketch->getDrawCallbackUsage() == cefix::SketchBase::UsePreDrawCallback) 
        {
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
        }
        #ifndef OSG_GLES1_AVAILABLE
			glPopAttrib(); 
		#endif
    }
private:
    mutable Sketch* _sketch;
    
};

SketchBase::SketchBase() 
:	AppController(),
	blockOtherInputHandler(true),
    _drawCallbackUsage(UseNoCallback)
{
}


void SketchBase::init() 
{
	setUseOptimizerFlag(false);
	
	cefix::AppController::init();
    
    setDrawCallbackUsage(_drawCallbackUsage, _drawCallback);
}

void SketchBase::setDrawCallbackUsage(DrawCallbackUsage usage, osg::Camera::DrawCallback* callback) 
{
    
    _drawCallbackUsage = usage;
    _drawCallback = callback;
        
    osg::Camera* cam = getMainWindow() ? getMainWindow()->getCamera() : NULL;
    if (cam)
    {
        cam->setPreDrawCallback(NULL);
        cam->setPostDrawCallback(NULL);
        switch(getDrawCallbackUsage()) {
            case UsePreDrawCallback:
                cam->setClearMask(0x0);
                cam->setPreDrawCallback(callback);
                break;
            case UsePostDrawCallback:
                cam->setPostDrawCallback(callback);
                break;
            default:
                break;
                
        }
    }
}   


void SketchBase::setApplicationPath(char* argv, const std::string& mediaFolderName) 
{
	cefix::DataFactory::instance()->setApplicationPath(argv, mediaFolderName);
}

int SketchBase::prepareLaunch()
{
	int result = cefix::AppController::prepareLaunch();
	
	requestPicking();
	
	applyWorld(createWorld());
	
	return result;
}

unsigned int SketchBase::getScreenWidth(unsigned int screenNum)
{
	unsigned int w, h;
	osg::GraphicsContext::getWindowingSystemInterface()->getScreenResolution(screenNum, w,h);
	return w;
}

unsigned int SketchBase::getScreenHeight(unsigned int screenNum) 
{
	unsigned int w, h;
	osg::GraphicsContext::getWindowingSystemInterface()->getScreenResolution(screenNum, w,h);
	return h;
}

void SketchBase::size(int w, int h) 
{
    setWindowRectangle(w, h);
    Display* display = getDisplayCollection() ? getDisplayCollection()->getDisplay("main") : NULL;
    if (display) display->set2DConfiguration(w, h);
}

void SketchBase::setScreenSize(unsigned int w, unsigned int h, unsigned int screenNum) {
	osg::GraphicsContext::getWindowingSystemInterface()->setScreenResolution(screenNum, w,h);
}

Sketch::Sketch()
:	SketchBase(),
	key(0),
	mouseX(0),
	mouseY(0),
	isMousePressed(0),
	isKeyPressed(0),
	mouseButton(0)
{
}


void Sketch::setDrawCallbackUsage(DrawCallbackUsage usage, osg::Camera::DrawCallback* callback) 
{    
    SketchBase::setDrawCallbackUsage(usage, callback ? callback : (usage != UseNoCallback) ? new SketchDrawCallback(this) : NULL);
}


int Sketch::prepareLaunch()
{
	int result = cefix::SketchBase::prepareLaunch();
	getMainWindow()->addEventHandler(new SketchEventHandler(this));
	
	return result;
}





}