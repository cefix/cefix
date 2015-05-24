/*
 *  ViewerEventHandler.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 12.06.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include "ViewerEventHandlers.h"
#include "ApplicationWindow.h"
#include "DataFactory.h"
#include <cefix/Settings.h>
#include <cefix/Notify.h>

namespace cefix {


std::string ViewerEventHandler::getLastUsedFileIndex(const std::string& file, const std::string& ext) 
{
	std::string path = osgDB::getFilePath(file);
	std::string prefix = osgDB::getSimpleFileName(file);
	
	osgDB::DirectoryContents content = osgDB::getDirectoryContents(path);
	unsigned int cnt = 0;
	for(osgDB::DirectoryContents::iterator i = content.begin(); i != content.end(); ++i) 
	{
		if ((osgDB::getFileExtension(*i) == ext) && (i->length() > prefix.length()+ext.length()+1) && (i->substr(0,prefix.length()) == prefix)) {
			std::string suffix = osgDB::getNameLessExtension(*i);
			suffix = suffix.substr(prefix.length()+1, suffix.length());
			unsigned int t = atoi(suffix.c_str());
			cnt = osg::maximum(cnt, t);
		}
	}
	++cnt;
	std::ostringstream ss;
	ss << path << "/" << prefix <<  std::setfill('0') << std::setw(4) << cnt << "." << ext;
	
	return ss.str();
}

bool ViewerEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) 
{
	if (ea.getHandled()) return false;

	switch(ea.getEventType())
    {
		case(osgGA::GUIEventAdapter::DRAG):
        case(osgGA::GUIEventAdapter::MOVE):
            _x = ea.getX();
            _y = ea.getY();
            break;
            
        case(osgGA::GUIEventAdapter::SCROLL):
			{
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (cefix::Settings::instance()->emulateScrollWheel() && view) {
				unsigned btn = 1;
				unsigned int mask = ea.getModKeyMask();
				unsigned int t = mask & osgGA::GUIEventAdapter::MODKEY_ALT;
				if (t > 0)
					btn = 2;
				t = mask & osgGA::GUIEventAdapter::MODKEY_SHIFT;
				if (t > 0)
					btn = 3;
                if ((ea.getScrollingDeltaX() != 0) || (ea.getScrollingDeltaY() != 0))
                {
                    std::cout << _x << "/" << _y << " +x: " << ea.getScrollingDeltaX() << " +y: " << ea.getScrollingDeltaY() << std::endl;
                    view->getEventQueue()->mouseButtonPress(_x +  ea.getScrollingDeltaX(), _y - ea.getScrollingDeltaY(), btn);
                    view->getEventQueue()->mouseMotion(_x, _y);
                    view->getEventQueue()->mouseButtonRelease(_x, _y, btn);
                }
                ea.setHandled(true);
				return true;
			}
			}
			break;
		case(osgGA::GUIEventAdapter::KEYDOWN):
			{
				osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
				if (!view) return false;
                
                cefix::notify("keyPress",ea.getKey());

				switch (ea.getKey()) {
					case 'O':
						{
							std::string file = cefix::DataFactory::instance()->getRootPath()+"/output.png";
							osg::ref_ptr<osg::Image> image = new osg::Image();
							ApplicationWindow* win = dynamic_cast<ApplicationWindow*>(&aa);
							osg::Camera* cam = (win) ? win->getCamera() : NULL;
							
							if (cam) 
							{
								win->getGraphicsWindow()->makeCurrent();
								osg::ref_ptr<osg::Image> img = new osg::Image();
								img->readPixels(0, 0, cam->getViewport()->width(), cam->getViewport()->height(), GL_RGBA, GL_UNSIGNED_BYTE);
								std::string path = cefix::DataFactory::instance()->getRootPath();
								if (!path.empty()) path +="/";
								std::string file = getLastUsedFileIndex(path+"output_", "png");
								if (osgDB::writeImageFile(*img.get(), file))
									osg::notify(osg::INFO) << "image written to " << file << std::endl;
								else 
									osg::notify(osg::INFO) << "could not write image to " << file << std::endl;
								
								win->getGraphicsWindow()->releaseContext();
							}
							return true;
							
						}
						break;
							
					case 'o':
						{
							std::string path = cefix::DataFactory::instance()->getRootPath();
							if (!path.empty()) path +="/";
							std::string file = getLastUsedFileIndex(path+"output_", "osg");
							if (osgDB::writeNodeFile(*view->getScene()->getSceneData(), file))
								osg::notify(osg::INFO) << "scene written to " << file << std::endl;
							else 
								osg::notify(osg::INFO) << "could not write scene to " << file << std::endl;
							
							return true;
						}
						break;
					case 'f':
						{
							ApplicationWindow* win = dynamic_cast<ApplicationWindow*>(&aa);
							if (win) win->setFullscreen(!win->getFullscreen());
							return true;
						}
						break;
					case 'C':
					case 'c':
						{
							ApplicationWindow* win = dynamic_cast<ApplicationWindow*>(&aa);
							osgGA::CameraManipulator* current = win->getCameraManipulator();
							osgGA::CameraManipulator* newmanip = NULL;
							if (current == win->getViewPointManipulator())
								newmanip = win->getTrackballManipulator();
							else
								newmanip = win->getViewPointManipulator();

							win->setCameraManipulator(newmanip);

							if (current) newmanip->setByMatrix(current->getMatrix());
							if (ea.getKey() == 'C') {
								newmanip->home(ea, aa);
							}
														
						}
						break;
					case 'm':
					case 'M':
						{
							ApplicationWindow* win = dynamic_cast<ApplicationWindow*>(&aa);
							_showMouseCursor = !_showMouseCursor;
                            cefix::log::info("ViewerEventHandler") << "cursor state: " << _showMouseCursor << std::endl;
							if (win) win->useCursor(_showMouseCursor);
						}
                        break;
                    case 'v':
					case 'V':
						{
							ApplicationWindow* win = dynamic_cast<ApplicationWindow*>(&aa);
							bool use_vsync = !win->getGraphicsWindow()->getSyncToVBlank();
                            cefix::log::info("ViewerEventHandler") << "new vsync state: " << use_vsync << std::endl;
							if (win) win->getGraphicsWindow()->setSyncToVBlank(use_vsync);
						}
                        break;
				}
			}
			break;
		
		default:
			return false;
	}
	
	return false;
		
}


}