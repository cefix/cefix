/*
 *  ImageFactory_test.cpp
 *  SimpleGLUTViewer
 *
 *  Created by Stephan Huber on Tue Nov 18 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#define __USE_OSX_AGL_IMPLEMENTATION__

#include <cefix/Log.h>
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Material>
#include <osg/Depth>
#include <osg/Stateset>
#include <osg/TexEnv>
#include <osg/io_utils>

#include <osgDB/WriteFile>



#include <cefix/ImageFactory.h>
#include <cefix/ImageUtilities.h>
#include <cefix/QTUtils.h>
#include <cefix/MathUtils.h>
#include <cefix/Perlin.h>
#include <cefix/Pixel.h>

#include <cefix/FilterApplier.h>
#include <cefix/ThreadedFilterApplier.h>

#include <cefix/ImageFilters.h>
#include <cefix/AppController.h>
#include <cefix/DataFactory.h>

#include <cefix/Quad2DGeometry.h>


osg::Geode* createPerlinTest() {
	osg::ref_ptr<cefix::Perlin> perlin = new cefix::Perlin(6, 1, 1, 0);
	osg::Image* img = new osg::Image();
	img->allocateImage(1024, 512, 1, GL_RGBA, GL_UNSIGNED_BYTE);
	//unsigned char* ptr;
	cefix::Pixel pixel(img);
	for(int y = 0; y < 500; ++y) {
		for(int x = 0; x < 1000; ++x) {
			pixel.setColor(x,y, osg::Vec4(
				0.5f * (1+perlin->get(x/150.0f, y/150.0f,0.33)),
				0.5f * (1+perlin->get(x/150.0f, y/150.0f,0.66)),
				0.5f * (1+perlin->get(x/150.0f, y/150.0f,0.1)),
				0.5f * (1+perlin->get(x/150.0f, y/150.0f,0.44))
			));
		}
	}

	osg::Geode* geode = new osg::Geode();
	osg::Texture2D* tex = new osg::Texture2D();
	tex->setImage(img);
	osg::Geometry* geo = cefix::ImageFactory::instance()->getImage(tex, osg::Vec3(-100, -100, -100), osg::Vec4(0,0,0,0));
	geode->addDrawable(geo);
	return geode;
	
}
 
osg::Geode* createImageUtilitiesTest() {

	// unser test-image laden -- ist ein RGBA-Bild
	osg::ref_ptr<osg::Image> source_img = osgDB::readImageFile("rgba_test.png");
	
	if (source_img == NULL)
		return NULL;
		
	
	cefix::Pixel pixel(source_img.get());
	osg::notify(osg::ALWAYS) << pixel.getColor(50,50) << std::endl;
	
	osg::Geode* geode = new osg::Geode();

	// einen vector erzeugen mit den verschiedenen channel-Konstanten
	typedef std::vector<cefix::ImageUtilities::Channel> ChannelVector;
	ChannelVector channels;
	channels.push_back(cefix::ImageUtilities::RED);
	channels.push_back(cefix::ImageUtilities::GREEN);
	channels.push_back(cefix::ImageUtilities::BLUE);
	channels.push_back(cefix::ImageUtilities::ALPHA);
	
	osg::Vec3 origin((source_img->s() + 10) * (-2), 0, 0);
	cefix::ImageFactory::instance()->setRenderPlane(cefix::ImageFactory::PLANE_X_Z);

	// ein ImageUtilites-Objekt erzeugen
	cefix::ImageUtilities util(source_img.get());
	
	
	{
		osg::TextureRectangle* tex = new osg::TextureRectangle();
		tex->setImage(source_img.get());
		osg::Geometry* geo = cefix::ImageFactory::instance()->getImage(tex, origin, osg::Vec4(0,0,0,0));
		origin[0] += source_img->s() + 10;
		geode->addDrawable(geo);
	}
	
	for(ChannelVector::iterator itr = channels.begin(); itr != channels.end(); itr++) {
		
		osg::Image* c_img = util.extractChannel((*itr), GL_ALPHA, GL_ALPHA);
		if (c_img) {
			osg::TextureRectangle* tex = new osg::TextureRectangle();
			tex->setImage(c_img);
			osg::Geometry* geo = cefix::ImageFactory::instance()->getImage(tex, origin, osg::Vec4(0,0,0,0));
			osg::StateSet* s = geo->getOrCreateStateSet();
			s->setTextureAttribute(0,new osg::TexEnv(osg::TexEnv::BLEND));
			s->setMode(GL_BLEND,osg::StateAttribute::ON);
			osg::Vec4Array* colors = new osg::Vec4Array();
			switch (*itr) {
				case cefix::ImageUtilities::RED:
					colors->push_back(osg::Vec4(1,0,0,1));
					break;
					
				case cefix::ImageUtilities::GREEN:
					colors->push_back(osg::Vec4(0,1,0,1));
					break;
				
				case cefix::ImageUtilities::BLUE:
					colors->push_back(osg::Vec4(0,0,1,1));
					break;
				
				case cefix::ImageUtilities::ALPHA:
					colors->push_back(osg::Vec4(0.5f, 0.5f, 0.5f,1));
					break;
			}
			geo->setColorArray(colors);
			geo->setColorBinding(osg::Geometry::BIND_OVERALL);
			geode->addDrawable(geo);
			origin[0] += source_img->s() + 10;
		}
		else {
			osg::notify(osg::ALWAYS) << "could not extract channel!" << (*itr) << std::endl;
		}
	
	}
	
	osg::StateSet* stateset = geode->getOrCreateStateSet();
    stateset->setMode(GL_BLEND,osg::StateAttribute::ON); // Blending einschalten
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	
	return geode;
}


osg::Geode* createImageRotateTest() {

	osg::Geode* geode = new osg::Geode();
	
	osg::Image* source = osgDB::readImageFile("scale_test.png");
	
	//Imageutilites-objekt erzeugen, und source-image zuweisen
	cefix::ImageUtilities util(source);
	
	geode->addDrawable(cefix::Quad2DGeometry::createFromImage(source, osg::Vec3(0,0,-300)));
    
    osg::ref_ptr<osg::Image> ccw = util.rotateCounterClockWise();
    osgDB::writeImageFile(*ccw, "/Users/stephan/Desktop/ccw.jpg");
    geode->addDrawable(cefix::Quad2DGeometry::createFromImage(ccw, osg::Vec3(500,0,-300)));
    
    osg::ref_ptr<osg::Image> cw = util.rotateClockWise();
    osgDB::writeImageFile(*cw, "/Users/stephan/Desktop/cw.jpg");
	geode->addDrawable(cefix::Quad2DGeometry::createFromImage(cw, osg::Vec3(1000,0,-300)));
    
    geode->addDrawable(cefix::Quad2DGeometry::createFromImage(util.rotate180Degrees(), osg::Vec3(1500,0,-300)));

	osg::StateSet* stateset = geode->getOrCreateStateSet();
    stateset->setMode(GL_BLEND,osg::StateAttribute::ON); // Blending einschalten
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	
	return geode;
}


osg::Geode* createImageScaleTest() {

	osg::Geode* geode = new osg::Geode();
	
	osg::Image* source = osgDB::readImageFile("scale_test.png");
	
	
	
	
	//Imageutilites-objekt erzeugen, und source-image zuweisen
	cefix::ImageUtilities util(source);
	
	osg::ref_ptr<osg::Image> with_border = util.addBorder(50, osg::Vec4(1,0,0,1));
	osgDB::writeImageFile(*with_border,"/Users/stephan/Desktop/with_border.png");
	
	osg::ref_ptr<osg::Image> biggerScaled  = util.scale<cefix::GaussianImageFilter>(source->s()*4, source->t()*1.5f);
	osg::Image* smallerScaled = util.scale<cefix::BilinearImageFilter>(source->s()*0.6f, source->t()*0.3f);
	
	if (biggerScaled) {
		osg::TextureRectangle* tex = new osg::TextureRectangle();
		tex->setImage(biggerScaled);
		osg::Geometry* geo = cefix::ImageFactory::instance()->getImage(tex, osg::Vec3(100,100,100), osg::Vec4(0,0,0,0));
		geode->addDrawable(geo);
	}
	
	if (smallerScaled) {
		osg::TextureRectangle* tex = new osg::TextureRectangle();
		tex->setImage(smallerScaled);
		osg::Geometry* geo = cefix::ImageFactory::instance()->getImage(tex, osg::Vec3(500,200,150), osg::Vec4(0,0,0,0));
		geode->addDrawable(geo);
	}
	
	if (source) {
		osg::TextureRectangle* tex = new osg::TextureRectangle();
		tex->setImage(source);
		osg::Geometry* geo = cefix::ImageFactory::instance()->getImage(tex, osg::Vec3(800,300,200), osg::Vec4(0,0,0,0));
		geode->addDrawable(geo);
	}
	

	osg::StateSet* stateset = geode->getOrCreateStateSet();
    stateset->setMode(GL_BLEND,osg::StateAttribute::ON); // Blending einschalten
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	
	return geode;
}


class MyPixelFunctor : public cefix::ThreadedFilterApplier::PixelFunctor {

public:
	inline void operator()(cefix::Pixel& input, cefix::Pixel& output, int x, int y) 
	{
		output.setColor(x, y, input.getColor(x,y) * 0.5 );
	}
	
	void finish() {
		std::cout << "finished" << std::endl;
	}

};
osg::Geode* createCopyPixelTest() {

	osg::Geode* geode = new osg::Geode();
	
	osg::ref_ptr<osg::Image> source = osgDB::readImageFile("images/4.jpg");
	
	cefix::ThreadedFilterApplier fa(0);
    osg::ref_ptr<osg::Image> source_2 = osg::clone(source.get());
	osg::Timer t;
	osg::Timer_t starttick = t.tick();
	fa.apply(source_2.get(), source.get(), new cefix::GaussianBlurFilter(64));
	std::cout << "filter duration " << t.delta_m(starttick, t.tick()) << std::endl;
	osg::Image* img2 = osgDB::readImageFile("4_inverse.jpg");
	
	fa.applyPixelFunctor<MyPixelFunctor>(source.get(), source.get(), new osg::Referenced());
	
	//Imageutilites-objekt erzeugen, und source-image zuweisen
	cefix::ImageUtilities util(source.get());
	
	osg::Image* result = util.copyPixels(img2,100,50);
		
	if (result) {
		osg::TextureRectangle* tex = new osg::TextureRectangle();
		tex->setImage(result);
		osg::Geometry* geo = cefix::ImageFactory::instance()->getImage(tex, osg::Vec3(000,100,1000), osg::Vec4(0,0,0,0));
		geode->addDrawable(geo);
	}
	
	if (img2) {
		osg::TextureRectangle* tex = new osg::TextureRectangle();
		tex->setImage(img2);
		osg::Geometry* geo = cefix::ImageFactory::instance()->getImage(tex, osg::Vec3(700,100,1000), osg::Vec4(0,0,0,0));
		geode->addDrawable(geo);
	}
	
	if (source.valid()) {
		osg::TextureRectangle* tex = new osg::TextureRectangle();
		tex->setImage(source.get());
		osg::Geometry* geo = cefix::ImageFactory::instance()->getImage(tex, osg::Vec3(1400,100,1000), osg::Vec4(0,0,0,0));
		geode->addDrawable(geo);
	}
	

	osg::StateSet* stateset = geode->getOrCreateStateSet();
    stateset->setMode(GL_BLEND,osg::StateAttribute::ON); // Blending einschalten
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	
	return geode;
}


class  MyAppController: public cefix::AppController {

	public:
		MyAppController() : cefix::AppController() {}
		
		virtual osg::Group* createWorld();
		
};

osg::Group* MyAppController::createWorld() 
{
    // osg::setNotifyLevel(osg::INFO);

    

#ifdef WIN32
	osgDB::Registry::instance()->getDataFilePathList().push_back("E:\\cygwin\\home\\stephan\\development\\cefix\\cefix\\tests\\ImageFactory\\images");
#endif
    osg::Group* g = new osg::Group();
     
     // Liste der Bilder erzeugen.
     std::vector<std::string> imageList;
     imageList.push_back("1.jpg");
     imageList.push_back("2.jpg");
     imageList.push_back("3.jpg");
     imageList.push_back("4.jpg");
     
     // Instanz der ImageFactory erzeugen   
     cefix::ImageFactory* iff = cefix::ImageFactory::instance();
     
     osg::Vec3 origin(-500,-250,0); // Ursprung des Bildes
     osg::Vec4 rect(0,0,0,0); // rect des Bildes, hier breite und hoehe=0, so werden breite + Hoehe des Bildes benutzt
     
     // ImageFactory rendert standardmŠssig auf die X_Y-plane (gut fŸr huds etc)
     // wir wollen aber auf die X_Z-Plane rendern:
     iff->setRenderPlane(cefix::ImageFactory::PLANE_X_Z_NEG);
     
     for(   std::vector<std::string>::iterator i = imageList.begin();
            i != imageList.end(); 
            i++) {
        
        // Geometrie erzeugen
    
        osg::Geometry* geo = iff->getImage((*i), origin, rect);
        
        // Geode erzeugen
        osg::Geode* geode = new osg::Geode();
        geode->addDrawable(geo);
        
        // gruppe hinzufuegen
        g->addChild(geode);
        
        // Z um 30 weiter
        origin[1] -= 125;
        
     }
     

     origin.set(0,-250,500);
     iff->setRenderPlane(cefix::ImageFactory::PLANE_X_Y);
     for(   std::vector<std::string>::iterator i = imageList.begin();
            i != imageList.end(); 
            i++) {
        
        // Geometrie erzeugen
    
        osg::Geometry* geo = iff->getImage((*i), origin, rect);
        
        // Geode erzeugen
        osg::Geode* geode = new osg::Geode();
        geode->addDrawable(geo);
        
        // gruppe hinzufuegen
        g->addChild(geode);
        
        // Z um 30 weiter
        origin[2] += 125;
        
     }
     
     int cnt = 0;
     origin.set(500,-250,0);
     iff->setRenderPlane(cefix::ImageFactory::PLANE_Y_Z);
     for(   std::vector<std::string>::iterator i = imageList.begin();
            i != imageList.end(); 
            i++) {
        
        // Geometrie erzeugen
        rect.set(cnt,cnt,cnt+300,cnt+300);
        cnt+=50;

        osg::Geometry* geo = iff->getImage((*i), origin, rect);
        
        // Geode erzeugen
        osg::Geode* geode = new osg::Geode();
        geode->addDrawable(geo);
        
        // gruppe hinzufuegen
        g->addChild(geode);
        
        // Z um 30 weiter
        origin[0] += 125;
        
     }

     
     // wir geben der Gruppe jetzt noch ein transparentes Material mit
     
    osg::StateSet* stateset = g->getOrCreateStateSet();
    
    osg::Material* mat = new osg::Material();
    //mat->setDiffuse(osg::Material::FRONT, osg::Vec4(1,1,1,0.5));
       

    mat->setDiffuse(osg::Material::BACK, osg::Vec4(1,0,1,0.5));
   // mat->setAmbient(osg::Material::BACK, osg::Vec4(1,0,0,0.5));

    stateset->setAttribute(mat);
    
    //indenDpethbuffer-schreiben ausschalten
    
    osg::Depth* depth = new osg::Depth();
    depth->setWriteMask(false);
    //stateset->setAttribute(depth);
        
    stateset->setMode(GL_BLEND,osg::StateAttribute::ON); // Blending einschalten
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN); // in den transparent-bin, damit tiefensortiert
	
	// g->addChild(createImageUtilitiesTest());
	
	// g->addChild(createImageScaleTest());
	
	// g->addChild(createCopyPixelTest());
	
	//g->addChild(createPerlinTest());
	
    g->addChild(createImageRotateTest());
    return g;
}


#pragma mark - 

int main(int argc, char *argv[])
{
	cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	std::cout << "root path: " << cefix::DataFactory::instance()->getRootPath() << std::endl;
    osg::ref_ptr<MyAppController> app = new MyAppController();
	app->setUseOptimizerFlag(false);
	// create the world and apply it
    app->applyWorld(app->createWorld());

    app->requestPicking(false);
    //app->requestPostScriptWriter();
    
    //create a viewer-window/fullscreen
    app->realize();
	app->run();
	
	return 0;
}