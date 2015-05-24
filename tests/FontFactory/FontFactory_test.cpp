/*
 *  ImageFactory_test.cpp
 *  SimpleGLUTViewer
 *
 *  Created by Stephan Huber on Tue Nov 18 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */
 
#define __USE_OSX_AGL_IMPLEMENTATION__

#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Material>
#include <osg/Depth>
#include <osg/Stateset>
#include <cefix/Log.h>
#include <osg/io_utils>
#include <osgDB/WriteFile>




#include <cefix/FontManager.h>
#include <cefix/MathUtils.h>
#include <cefix/FontManager.h>
#include <cefix/TextRenderer.h>
#include <cefix/TextGeode.h>
#include <cefix/TabbedTextGeode.h>
#include <cefix/QTUtils.h>
#include <cefix/PropertyList.h>
#include <cefix/StringUtils.h>

#include <cefix/AppController.h>
#include <cefix/DataFactory.h>
#include <cefix/AllocationObserver.h>
#include <cefix/Settings.h>
#include <cefix/ColorUtils.h>
#include <cefix/AnimationFactory.h>

#include <cefix/FilterApplier.h>
#include <cefix/ImageFilters.h>


class TestAppController : public cefix::AppController {
	public:
		TestAppController() : cefix::AppController() {
			
			osg::notify(osg::ALWAYS) << "size of wchart_t" << sizeof(wchar_t) << std::endl;
			cefix::Settings::instance()->setReportMissingCodePointsFlag(true);
#ifdef WIN32
			osgDB::Registry::instance()->getDataFilePathList().push_back("E:\\cefix\\cefix\\tests\\FontFactory");
#endif
            setRunFrameScheme(osgViewer::ViewerBase::ON_DEMAND);
		}
		
		osg::Group* createWorld() {
			osg::Group* world = new osg::Group();
			
			if (0) {
				// stats über font-usage:
				cefix::AllocationObserver::instance()->setEnabledFlag(true);
				
				cefix::AllocationObserver::instance()->enableStatisticsForKey("Font");
				cefix::AllocationObserver::instance()->enableStatisticsForKey("FontTexture");
				cefix::AllocationObserver::instance()->enableStatisticsForKey("FontImage");
				cefix::AllocationObserver::instance()->enableStatisticsForKey("TextRendererIterator");
				
				cefix::AllocationObserver::instance()->enableStatisticsForKey("Group");
				cefix::AllocationObserver::instance()->enableStatisticsForKey("Geode");
				cefix::AllocationObserver::instance()->enableStatisticsForKey("MatrixTransform");
				cefix::AllocationObserver::instance()->enableStatisticsForKey("Geometry");
				cefix::AllocationObserver::instance()->enableStatisticsForKey("StateSet");
				cefix::AllocationObserver::instance()->enableStatisticsForKey("Camera");

				cefix::AllocationObserver::instance()->enableStatisticsForKey("Depth");
				cefix::AllocationObserver::instance()->enableStatisticsForKey("BlendFunc");
				cefix::AllocationObserver::instance()->enableStatisticsForKey("Texture2D");
				get2DLayer()->addChild(cefix::AllocationObserver::instance()->getStatistics());
				
				
				enablePerformanceStatistics(true);
				get2DLayer()->addChild(getPerformanceStatisticsGroup());				
				
			}
			
			
			
			
			/*
			 world->addChild(createStandardTextTest());
			world->addChild(createStandardTextTest("text_jp.xml", osg::Vec3(0,00,-100)));
			world->addChild(createStandardTextTest("text_jp Kopie.xml", osg::Vec3(1000,00,00)));
			world->addChild(createUTF16Test());
			
			world->addChild(createMultiTextureFontTest());
			
			*/
			world->addChild(createTabbedTextTest());
			world->addChild(createAttributedTextTest(osg::Vec3(0,0,0)));
			
			/*
			world->addChild(createDynamicTexts());
			
			world->addChild(createFilteredFont());
			*/						
			return world;
		
		}
	private:
		osg::Group* createStandardTextTest(const std::string& filename = "text.xml", const osg::Vec3& anchor = osg::Vec3(0,0,0));
		osg::Group* createTabbedTextTest(); 
		osg::Group* createMultiTextureFontTest();
		osg::Group* createUTF16Test();
		osg::Group* createDynamicTexts();
		osg::Group* createFilteredFont();
		osg::Group* createAttributedTextTest(const osg::Vec3& anchor);

};


class CreateDynamicTextsAnimation : public cefix::AnimationBase, public cefix::NotificationListener {

	public:
		CreateDynamicTextsAnimation(osg::Group* g) 
		:	cefix::AnimationBase(), 
			cefix::NotificationListener(),  
			_g(g), 
			_enabled(false) 
		{
			subscribeTo("keypress", this, &CreateDynamicTextsAnimation::handleKeyPress); 

		}
		
	protected:
		void handleKeyPress(const int& key) {
			if (key == 'x')
				_enabled = !_enabled;
			std::cout << "enabled " << _enabled;
		}
		void animate(float elapsed) {
			if (!_enabled) return;
			
			_g->removeChildren(0, _g->getNumChildren());
			for (unsigned int i = 0; i < 500; ++i) {
				cefix::AsciiTextGeode* t = new cefix::AsciiTextGeode("system.xml", 1, "was geht was  auch nicht, und warum ist das so ");
				t->setTextColor(cefix::HSVtoRGB(cefix::randomf(360.0f), 1,1));
				t->setPosition(osg::Vec3(cefix::floatRange(-10,10).random(), cefix::floatRange(-10,10).random(), cefix::floatRange(-10,10).random()));
				t->setSupportsDisplayList(false);
				
				_g->addChild(t);
				t->refresh();
			}
			_g->setCullingActive(false);
		}
	private:
		osg::ref_ptr<osg::Group> _g;
		bool						_enabled;

};

 
osg::Group* TestAppController::createFilteredFont() {
	
	cefix::Font* blurredFont = cefix::FontManager::instance()->cloneFont("Inconsolata12.xml", "InconsolataBlurred12.xml");
	if (!blurredFont) {
		std::cout << "font nicht gefunden" << std::endl;
		return new osg::Group();
	}
	
	cefix::FilterApplier().apply(blurredFont->getImages(), new cefix::DilateFilter(5));
	cefix::FilterApplier().apply(blurredFont->getImages(), new cefix::GaussianBlurFilter(25.0));
	
	cefix::AsciiTextGeode* normal = new cefix::AsciiTextGeode("Inconsolata12.xml", 12, "Die Inconsolata ist fuer eine nichtproportionale Schrift ganz huebsch");
	normal->setPosition(osg::Vec3(0,0,0));
	normal->setTextColor(osg::Vec4(1,1,1,1));

	cefix::AsciiTextGeode* blurred = new cefix::AsciiTextGeode("InconsolataBlurred12.xml", 12, "Die Inconsolata ist fuer eine nichtproportionale Schrift ganz huebsch");
	blurred->setPosition(osg::Vec3(0,0,-0.1));
	blurred->setTextColor(osg::Vec4(1,1,0,1));
	
	osg::Group* g = new osg::Group();
	g->addChild(normal);
	g->addChild(blurred);
	
	return g;

	
}

osg::Group* TestAppController::createDynamicTexts() {
	
	osg::Group* g = new osg::Group();
	cefix::AnimationFactory::instance()->add(new CreateDynamicTextsAnimation(g));
	
	return g;
}

osg::Group* TestAppController::createMultiTextureFontTest() {

	osg::Group* group = new osg::Group();
	cefix::AsciiTextGeode* tg = new cefix::AsciiTextGeode("Helvetica_Neue_Ult_72.xml",72);
	tg->setTextColor(osg::Vec4(1,0,0,1));
	tg->setText("MultiTextureFonts are working, too! (äöüÄÖÜß?=)(/&$%&/");
	std::cout << (*tg) << std::endl;
	group->addChild(tg);
	
	cefix::AsciiTextGeode* cloned = dynamic_cast<cefix::AsciiTextGeode*>(tg->clone(osg::CopyOp::DEEP_COPY_ALL));
	cloned->setPosition(osg::Vec3(-100,-100,0));
	group->addChild(cloned);
	return group;
}

osg::Group* TestAppController::createUTF16Test() {
	osg::Group* g = new osg::Group();
	
	osg::Vec3 origin;
	origin.set(0,0,200);
	
    std::wstring utf16 = L"Ein UTF-16-string, der sollte auch korrekt gerendert werden: \u00E4\u00F6\u00FC";
    cefix::Font* font = cefix::FontManager::instance()->loadFont("VendettaLight_24.xml");
    osg::ref_ptr<cefix::TextRenderer> renderer = new cefix::TextRenderer();
    cefix::TextRenderer::Container* container = renderer->renderParagraph<std::wstring>(font, 24, utf16, origin);
	
	// hier ein beispiel fuer farbigen Text, das wuerde auch auf buchstabenbasis funktionieren
	// geht aber nur ohne material, bzw wenn color + material kombiniert wird
	osg::Vec4Array* c = new osg::Vec4Array();
	c->push_back(osg::Vec4(0,1,1,1));
	container->setColorArray(c);
	container->setColorBinding(osg::Geometry::BIND_OVERALL);
	container->dirty();
    osg::Geode * geode = new osg::Geode(); 
    container->attachTo(geode);
    g->addChild(geode);
	
	
	{
		cefix::UnicodeTextGeode* tg = new cefix::UnicodeTextGeode(font, 24, utf16);
		g->addChild(tg);
		std::cout << (*tg) << std::endl;
	}
	return g;

}


osg::Group* TestAppController::createStandardTextTest(const std::string& filename, const osg::Vec3& anchor) {

	
    cefix::PropertyList* l = new cefix::PropertyList(filename);
    std::wstring text = l->get("text")->asWstring();
    if (text.empty()) {
        text = L"Could not load data from text.xml";
    }
	
    osg::Group* g = new osg::Group();
    
    cefix::FontManager* fm = cefix::FontManager::instance();
     
    std::vector<std::string> fontlist;

    fontlist.push_back(std::string("Palatino_32.xml"));
    fontlist.push_back(std::string("VendettaLight_24.xml"));

    
	
    osg::Vec3 origin(anchor);
    
    for(    std::vector<std::string>::iterator i = fontlist.begin();
            i != fontlist.end();
            i++) 
    { 
        // Font laden
        cefix::Font* font = fm->loadFont((*i));
    
        // Text-Renderer erzeugen
        osg::ref_ptr<cefix::TextRenderer> renderer = new cefix::TextRenderer();
        osg::ref_ptr<cefix::TextRenderer::Container> container = renderer->renderParagraph<std::wstring>(font, 12, text, origin);
        	
        if (container.valid()) {
			osg::Geode * geode = new osg::Geode(); 	
			container->attachTo(geode);
			g->addChild(geode);
		}
        origin[0] += 350;
    }
    
	return g;
}


osg::Group* TestAppController::createAttributedTextTest(const osg::Vec3& anchor) {

	cefix::AttributedTextGeode<std::string>* tg = new cefix::AttributedTextGeode<std::string>("system.xml", 9, "");
	tg->setText("Palatino\nZeile 1\nZeile 2\nZeile 3\n\n*Vendetta\nZeile 1\nZeile 2\nZeile 3\n*\n#System\nZeile 1\nZeile 2\nZeile 3\n#\n");
	tg->setTextColor(osg::Vec4(1,1,1,1),0);
	tg->setFont("Palatino_32.xml",0);
	tg->setFontSize(32,0);
	tg->setLineSpacing(32, 0);
	
	
	
	tg->setFont("VendettaLight_24.xml", '*');
	tg->setFontSize(24,'*');
	tg->setTextColor(osg::Vec4(1,0,1,1),'*');
	tg->setLineSpacing(24, '*');
	
	tg->setFont("system.xml", '#');
	tg->setFontSize(8,'#');
	tg->setTextColor(osg::Vec4(1,1,0,1),'#');
	tg->setLineSpacing(9, '#');
	
    tg->setPosition(anchor);
	tg->refresh();
	tg->refresh();
	osg::Group* g = new osg::Group();
	g->addChild(tg);  
	
	{
		cefix::AttributedTextGeode<std::string>* tg2 = osg::clone(tg, osg::CopyOp::DEEP_COPY_ALL);
		tg2->setPosition(anchor+osg::Vec3(400, 0, 0));
		tg2->setText("Clone Palatino\nZeile 1\nZeile 2\nZeile 3\n\n*Vendetta\nZeile 1\nZeile 2\nZeile 3\n*\n#System\nZeile 1\nZeile 2\nZeile 3\n#\n");
		g->addChild(tg2);
		
	}
	return g;
}


osg::Group* TestAppController::createTabbedTextTest() 
{
	// osg::setNotifyLevel(osg::DEBUG_INFO);
	osg::Group* g = new osg::Group();
	float size = 10;
    std::vector< osg::ref_ptr< cefix::TabbedTextGeode<std::string> > > texts;
    for (int i = 0; i < 3; i++) {
        ostringstream ss;
        
		ss << "Text " << i;
        osg::Vec3 pos(cefix::in_between(-200, 200),cefix::in_between(-200, 200),cefix::in_between(-200, 200));
        osg::Vec4 c(1,1,0,1);

        cefix::TabbedTextGeode<std::string>* tg = new cefix::TabbedTextGeode<std::string>("Palatino_32.xml", size, ss.str());
        tg->setPosition(pos);
        if (i % 3 == 1)
			tg->setRenderPlane(cefix::FontCharData::NEGATIVE_XZ_PLANE);
        if (i % 3 == 0)
            tg->setRenderPlane(cefix::FontCharData::YZ_PLANE);
            
        tg->setTextAlignment(cefix::TabbedTextGeode<std::string>::RenderIterator::ALIGN_LEFT);
        tg->setTextColor(c);
        //tg->setSupportsDisplayList(false); // to get faster rendering

		tg->setFont("system.xml", '|');
		tg->setFontSize(14, '|');
		tg->setTextColor(osg::Vec4(1,1,0,0.7), '|');
		tg->setFont("VendettaLight_24.xml", '/');
		tg->setFontSize(24, '/');
		tg->setTextColor(osg::Vec4(1,0,1,0.7), '/');
		tg->setLineSpacing(48, '/');
		
		tg->setTabAt(200, cefix::TabbedTextRenderIterator::TAB_LEFT);
		tg->setTabAt(400, cefix::TabbedTextRenderIterator::TAB_CENTER);
		tg->setTabAt(600, cefix::TabbedTextRenderIterator::TAB_RIGHT);
		
		ss.str("");
		ss << "links\tlinks\tmitte\trechts\n";
		ss << "einszweidrei\tlinks\t|mitte|\t/rechts\nneue zeile mit grossem abstand/\t" << "|" << i << "| Text\t" << i << "nochntext\trechts text\n";
		tg->setText(ss.str());
		
		tg->refresh();
		cefix::TabbedTextRenderIterator* itr = dynamic_cast<cefix::TabbedTextRenderIterator*>(tg->getRenderIterator());
		tg->addDrawable(itr->createDebugGeometry());
		tg->getTabbedTextIterator()->dumpFontMap();
        texts.push_back(tg);
        g->addChild(tg);
		
		cefix::TabbedTextGeode<std::string>* cloned = dynamic_cast<cefix::TabbedTextGeode<std::string>*>( tg->clone(osg::CopyOp::DEEP_COPY_ALL) );
		if (cloned) {
			cloned->setPosition(cloned->getPosition() - osg::Vec3(0,200,200));
			cloned->refresh();
			
			g->addChild(cloned);
			osgDB::writeNodeFile(*cloned, "cloned.osg");
		}
		
		std::cout << tg->getTextColor() << std::endl;
    }
     
     // wir geben der Gruppe jetzt noch ein transparentes Material mit
     
    osg::StateSet* stateset = g->getOrCreateStateSet();
    
    osg::Material* mat = new osg::Material();
    mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1,1,1,0.8));
    //mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1,1,1,0.5));
    //stateset->setAttribute(mat);
    

			
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON); // Blending einschalten
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN); // in den transparent-bin, damit tiefensortiert
	
	//osg::setNotifyLevel(osg::NOTICE);
	return g;
}
	
	

#pragma mark -

int main(int argc, char *argv[]) 
{
	osg::setNotifyLevel(osg::NOTICE);

	cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	osg::ref_ptr<TestAppController> app = new TestAppController();
	app->setThreadingModel(osgViewer::ViewerBase::SingleThreaded);
	
	app->setUseOptimizerFlag(true);
	app->applyWorld(app->createWorld());
    app->realize();
	cefix::AllocationObserver::instance()->observeSceneGraph(app->getMainWindow()->getSceneData());
	app->run();

	return 0;
}


