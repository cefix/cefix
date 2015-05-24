/*
 *  movieplayer.cpp
 *  Circles
 *
 *  Created by Stephan Huber on 16.10.04.
 *  Copyright 2004 __MyCompanyName__. All rights reserved.
 *  
 */
 

#define __USE_OSX_AGL_IMPLEMENTATION__
  
#include <osg/ImageStream>
#include <cefix/Log.h>
#include <osg/Depth>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osg/BlendFunc> 
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>
#include <osg/ClearNode>
#include <osg/Projection>
#include <osg/Material>
#include <osg/TextureRectangle>
#include <osg/Fog>
#include <osg/MatrixTransform>
#include <cefix/Settings.h>
#include <osgUtil/Optimizer>
#include <osgDB/Registry>
#include <cefix/AnimationFactory.h>

#include <cefix/MathUtils.h>
#include <cefix/QTUtils.h>
#include <cefix/MovieData.h>
#include <cefix/SequenceGrabberManager.h>
#include <cefix/VideoMediaManager.h>
#include <cefix/SequenceGrabber.h>
#include <cefix/QuicktimeMovie.h>
#include <cefix/QuicktimeMovieExporter.h>
#include <cefix/CoreVideoTexture.h>
#include <cefix/DebugGeometryFactory.h>
#include <cefix/FileUtils.h>

#include <cefix/AppController.h>

#include <cefix/NotificationListener.h>

static bool s_use_core_video = false;
static bool s_use_texture_rectangle = true;
static bool s_use_separate_idle_thread = true;
static int s_max_allowed_movies = 1;

osg::Geometry* createTexturedQuadGeometry(const osg::Vec3& pos,float width,float height, osg::Image* image)
{
	bool useCoreVideo = s_use_core_video;
    bool useTextureRectangle = s_use_texture_rectangle;
	
	if (useCoreVideo)
    {
        osg::ref_ptr<osg::Geometry> pictureQuad = osg::createTexturedQuadGeometry(pos,
                                           osg::Vec3(width,0.0f,0.0f),
                                           osg::Vec3(0.0f,0.0f,height),
                                           image->s(),image->t());
        
		cefix::CoreVideoTexture* t = new cefix::CoreVideoTexture(image);                    
        pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                    t,
                    osg::StateAttribute::ON);
        
        osg::Vec4Array* colors = new osg::Vec4Array();
        colors->push_back(osg::Vec4(1,1,1,1));
        pictureQuad->setColorArray(colors);
        pictureQuad->setColorBinding(osg::Geometry::BIND_OVERALL);
                                
        return pictureQuad.release();
    }
	
    if (useTextureRectangle)
    {
         osg::ref_ptr<osg::Geometry> pictureQuad = osg::createTexturedQuadGeometry(pos,
                                           osg::Vec3(width,0.0f,0.0f),
                                           osg::Vec3(0.0f,0.0f,height),
                                           image->s(),image->t());
        
		osg::TextureRectangle* t = new osg::TextureRectangle(image);
		t->setUnRefImageDataAfterApply(false);
		//t->setClientStorageHint(true); 
		pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                    t,
                    osg::StateAttribute::ON);
        
        osg::Vec4Array* colors = new osg::Vec4Array();
        colors->push_back(osg::Vec4(1,1,1,1));
        pictureQuad->setColorArray(colors);
        pictureQuad->setColorBinding(osg::Geometry::BIND_OVERALL);
                                
        return pictureQuad.release();
    }
    else
    {
        osg::ref_ptr<osg::Geometry> pictureQuad = osg::createTexturedQuadGeometry(pos,
                                           osg::Vec3(width,0.0f,0.0f),
                                           osg::Vec3(0.0f,0.0f,height),
                                           1.0f,1.0f);
        osg::Texture* tex = new osg::Texture2D(image);
        tex->setResizeNonPowerOfTwoHint(false);
        pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                    tex,
                    osg::StateAttribute::ON);

        return pictureQuad.release();
    }
}






osg::Geode* createLiveStream(osg::Vec3 pos) {
	enum Mode {IMAGE, MOVIE, LIVE};
    
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    
#ifdef WIN32
	osgDB::Registry::instance()->getDataFilePathList().push_back("C:/Users/stephan");
	//osgDB::Registry::instance()->getDataFilePathList().push_back("C:/Dokumente und Einstellungen/stephan.WINTEILCHEN/Eigene Dateien/cefix/cefix/tests/Quicktime");
	
#endif
    std::string fileName = "dummy.mov";

    
    int w = 640; 
    int h = 480;
	osg::Image* image = NULL;
	Mode mode = LIVE;

    
    std::vector< osg::ref_ptr<osg::Image> > imageList;
    
	switch (mode){
		case IMAGE:
			imageList.push_back(osgDB::readImageFile("dummy.png"));
			break;

		case MOVIE:
			{
                int maxAllowedMovies = s_max_allowed_movies;
                int currentNdx = 0;
                
				std::string path = osgDB::findDataFile("/Users/stephan/Documents/Projekte/cefix/cefix/tests/Quicktime/movies");
                osgDB::DirectoryContents cnt = osgDB::getDirectoryContents(path);
                for(osgDB::DirectoryContents::iterator itr = cnt.begin(); itr != cnt.end(); itr++) {
                
                    std::string  file =  path + "/" + (*itr);
                    if  (osgDB::getFileExtension(file) == "mov") {
                        currentNdx++;
						std::cout << "reading " << file << std::endl;
                        osg::ref_ptr<osg::ImageStream> is = dynamic_cast<osg::ImageStream*>(osgDB::readImageFile(file));
                        
                        if ((is) && (currentNdx <= maxAllowedMovies)) {
                            
                            is->play();
                            imageList.push_back(is);
                        }
                        
                    }
                }
				
				
				// test http-streaming
				if (0) {
					osg::setNotifyLevel(osg::DEBUG_INFO);
					// http://digitalmind.de/sites/digitalmind.de/files/livefeedbackSchnappschuss001.mp4
					osg::ref_ptr<osg::ImageStream> is = dynamic_cast<osg::ImageStream*>(osgDB::readImageFile("http://digitalmind.de/sites/digitalmind.de/files/livefeedbackSchnappschuss001.mp4"));
					osg::setNotifyLevel(osg::NOTICE);
					if ((is) && (currentNdx <= maxAllowedMovies)) {
						is->play();
						imageList.push_back(is);
					}

				}
            }
			break;

		case LIVE:
			{
                const cefix::SequenceGrabberDeviceInfoList& devices = cefix::SequenceGrabberManager::instance()->getDeviceInfoList();
                for(cefix::SequenceGrabberDeviceInfoList::const_iterator i = devices.begin(); i != devices.end(); ++i) {
                    std::cout << (*i).getGrabberId() << "/" << (*i).getId() << std::endl;
                }
				cefix::SequenceGrabber* sq = cefix::SequenceGrabberManager::instance()->get(devices[0].getKey(), 160,120);
				sq->showSettings();
                sq->start();
				imageList.push_back(sq->getImage());
				if (devices.size() > 1) {
					cefix::SequenceGrabber* sq2 = cefix::SequenceGrabberManager::instance()->get(devices[1], 160,120);
					sq2->start();
					imageList.push_back(sq2->getImage());
				}

			}
			break;
	}
	if (imageList.size() == 0) {
		//osg::notify(osg::ALWAYS) << "could not load image/movie" << std::endl;
		return geode;
	}
	
	if (0) {// Image kopieren
		osg::Image* copyimage = new osg::Image(*(imageList[0].get()), osg::CopyOp::DEEP_COPY_ALL);
		copyimage->setFileName("");
		imageList.push_back(copyimage);
	}
    w = imageList[0]->s();
    h = imageList[0]->t();
    
    int maxImages = imageList.size() ;
    int max = (maxImages > 10) ? maxImages : 10;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            pos = osg::Vec3((x-5) * (w+10), 0, (y-5) * (h+10));
            osg::ref_ptr<osg::Geometry> g;
            image = imageList[(x % maxImages)].get();
            g = createTexturedQuadGeometry(pos,image->s(),image->t(),image); 
			
			//g->setUseDisplayList(false);
            geode->addDrawable(g);
        }
    }
    
    osg::StateSet* state = geode->getOrCreateStateSet();
    
    state->setMode(GL_BLEND,osg::StateAttribute::ON);
    state->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    
    osg::Depth* depth = new osg::Depth();
    depth->setWriteMask(false);
    state->setAttribute(depth);
    
    osg::BlendFunc* b = new osg::BlendFunc();
    b->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
    state->setAttribute(b);
	
	return geode.release();
}



		
class MyAppController : public cefix::AppController {

	public:	
		osg::Group* createWorld() {
			
            cefix::Settings::instance()->setUseCoreVideo(s_use_core_video);
            cefix::Settings::instance()->setUseThreadedMediaIdling(s_use_separate_idle_thread);
            
		
			osg::Group* g = new osg::Group();
			
			
			// Hintergrund
			
			osg::ClearNode* cn = new osg::ClearNode();
			cn->setClearColor(osg::Vec4(1,1,1,1));
			cn->setClearMask(GL_DEPTH_BUFFER_BIT);
			//g->addChild(cn);
			
            g->addChild(createLiveStream(osg::Vec3(0,0,0)));
			//cefix::AnimationFactory::instance()->add(new TestImageLoading());
			return g;

		}

};

#pragma mark - 

int main(int argc, char *argv[])
{
	osg::setNotifyLevel(osg::WARN);
	
	time_t t(0);
	cefix::getModifiedTimeStamp("/Users/stephan/Desktop/background.psd", t);
	cefix::Date d = cefix::Date::getFromUnixTimeStamp(t);
	std::cout << "modified: " << d << std::endl;
	d -= 60*60*24;
	cefix::setModifiedTimeStamp("/Users/stephan/Desktop/background.psd", d.getAsUnixTimeStamp());
	
	cefix::getModifiedTimeStamp("/Users/stephan/Desktop/background.psd", t);
	d = cefix::Date::getFromUnixTimeStamp(t);
	std::cout << "new modified: " << d << std::endl;

	
    osg::ref_ptr<MyAppController> app = new MyAppController();
	//app->setThreadingModel(osgViewer::ViewerBase::AutomaticSelection);
	app->setUseOptimizerFlag(true);
	// create the world and apply it
    app->applyWorld(app->createWorld());

    app->requestPicking();
    //app->requestPostScriptWriter();
    
    //create a viewer-window/fullscreen
    app->realize();
	app->run();

	app = NULL;
	return 0;
}

