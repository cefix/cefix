/*
 *  test_pen_pressure.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 04.05.06.
 *  Copyright 2006 digitalmind.de // Stephan Maximilian Huber. All rights reserved.
 *
 */
 
 #define __USE_OSX_AGL_IMPLEMENTATION__

#include <cefix/Log.h>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osgGA/GUIEventHandler>

#include <cefix/AppController.h>

osg::Geode* createCube() { 


    osg::Geode* geode = new osg::Geode();
    osg::Box* box = new osg::Box(osg::Vec3(0,0,0), 1.0f);
    osg::ShapeDrawable* drawable = new osg::ShapeDrawable(box);
    geode->addDrawable(drawable);
    
    return geode;

}

class ScaleCubeFromPressureEventHandler : public osgGA::GUIEventHandler {
    protected:
        osg::MatrixTransform* _mat;
        float _targetPressure, _currentPressure;
        
    public:
        ScaleCubeFromPressureEventHandler(osg::MatrixTransform* mat) : 
            osgGA::GUIEventHandler(), 
            _mat(mat), 
            _targetPressure(0), 
            _currentPressure(0) 
            {}
        
        
        
        virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object* o, osg::NodeVisitor* nv) {
            switch (ea.getEventType()) {
				case osgGA::GUIEventAdapter::FRAME:
					{
						_currentPressure = _targetPressure * 0.1f + _currentPressure * 0.9f;
						osg::Matrix m = osg::Matrix::scale(_currentPressure + 1, _currentPressure + 1, _currentPressure + 1);
						_mat->setMatrix(m);
					}
					break;
            
				    case osgGA::GUIEventAdapter::PEN_PROXIMITY_ENTER:
                    osg::notify(osg::ALWAYS) << "Pen entering proximity-range: " << ea.getTabletPointerType() << std::endl;
                    break;
                    
                case osgGA::GUIEventAdapter::PEN_PROXIMITY_LEAVE:
                    osg::notify(osg::ALWAYS) << "Pen leaving proximity-range: " << ea.getTabletPointerType() << std::endl;
                    break;
                    
                case osgGA::GUIEventAdapter::PEN_PRESSURE:
					std::cout << "druck: " << ea.getEventType() << std::endl;
                    _targetPressure = 10 * ea.getPenPressure();
                    return true;
                    break;
                                        
                case osgGA::GUIEventAdapter::RELEASE:
                    _targetPressure = 0;
                    break;
                    
                case osgGA::GUIEventAdapter::SCROLL:
                    osg::notify(osg::ALWAYS) << "scrolling: ";
					osg::notify(osg::ALWAYS) << ea.getScrollingDeltaX() << " / " << ea.getScrollingDeltaY() << " // ";
                    switch (ea.getScrollingMotion()) {
                        case osgGA::GUIEventAdapter::SCROLL_DOWN:
                            osg::notify(osg::ALWAYS) << "down";
                            break;
                         case osgGA::GUIEventAdapter::SCROLL_UP:
                            osg::notify(osg::ALWAYS) << "up";
                            break;
                         case osgGA::GUIEventAdapter::SCROLL_LEFT:
                            osg::notify(osg::ALWAYS) << "left";
                            break;
                         case osgGA::GUIEventAdapter::SCROLL_RIGHT:
                            osg::notify(osg::ALWAYS) << "right";
                            break;
                         case osgGA::GUIEventAdapter::SCROLL_2D:
                            osg::notify(osg::ALWAYS) << ea.getScrollingDeltaX() << " / " << ea.getScrollingDeltaY();
                            break;
						default:
							break;
                    }
                            
                    osg::notify(osg::ALWAYS) << std::endl;
                    break;
				case osgGA::GUIEventAdapter::KEYDOWN:
					osg::notify(osg::ALWAYS) << "key down" << ea.getKey() << " modmask " << ea.getModKeyMask() << std::endl;
				default:
					// nothing special here
					break;
            }
            return false;
        }
};


int main(int argc, char *argv[])
{
    osg::setNotifyLevel(osg::INFO);
    osg::Group* g = new osg::Group();
    
    
    osg::Geode* cube = createCube();
    osg::MatrixTransform* mat = new osg::MatrixTransform();
    ScaleCubeFromPressureEventHandler* scfpe = new ScaleCubeFromPressureEventHandler(mat);
    mat->setEventCallback(scfpe);
	//mat->setUpdateCallback(scfpe);
    mat->addChild(cube);
    g->addChild(mat);
   
    cefix::AppController* app = new cefix::AppController();
    //app->getMainWindow()->getEventHandlers().push_back(scfpe);
	app->applyWorld(g);
    app->run();
    
    	

	return 0;
}