/*
 *  test_textonspline.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 08.11.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include <cefix/Sketch.h>
#include <cefix/DataFactory.h>
#include <cefix/TextGeode.h>
#include <cefix/TextOnSplineRenderIterator.h>
#include <cefix/CatMullRom.h>
#include <cefix/MathUtils.h>


class TestSketch : public cefix::Sketch {

	public:
		TestSketch() : 
			cefix::Sketch() 
		{
			allowOsgHandler(true);
            setDrawCallbackUsage(UsePreDrawCallback);
		}
		
		osg::Node* createTextGeode(const std::string& txt) {
		
			osg::Vec3Array* v = new osg::Vec3Array();

			for(unsigned int i = 0; i < 5; ++i) {
				v->push_back(osg::Vec3(cefix::floatRange(-100,100).random(), cefix::floatRange(-100,100).random(), cefix::floatRange(-100,100).random() ));
			}
			
			// Text erzeugen
			cefix::AsciiTextGeode* textgeode = new cefix::AsciiTextGeode("Interstate-Black_72.xml", 18);
			textgeode->setText(txt+"//"+txt+"//"+txt+"//"+txt+"//"+txt);
						textgeode->setTextColor(osg::Vec4(1,1,1,1));
			osg::Geometry* debuggeometry = NULL;
			if (1) {

				// Kurve erzeugen
				cefix::spline::Curve* cmr = new cefix::spline::CatMullRom(new cefix::spline::ControlPoint::List(v));
			
				// Renderer erzeugen
				typedef cefix::TextOnSplineRenderIteratorT<cefix::TextOnSplineResolvers::Curve, true> TOSRI;
				TOSRI* itr = new TOSRI(cefix::TextOnSplineResolvers::Curve(cmr, osg::Vec3(0,0,1)));
				textgeode->setRenderIterator(itr);
				
			} else {
				// Renderer erzeugen
				typedef cefix::TextOnSplineRenderIteratorT<cefix::TextOnSplineResolvers::SimpleLineStrip, true> TOSRI;
				TOSRI* itr = new TOSRI(cefix::TextOnSplineResolvers::SimpleLineStrip(v, osg::Vec3(0,0,1)));
				textgeode->setRenderIterator(itr);
				debuggeometry = itr->getDebugGeometry();
			
			}
			
			osg::Group* group = new osg::Group();
			group->addChild(textgeode);
			if (debuggeometry) {
				osg::Geode* geode = new osg::Geode();
				geode->addDrawable(debuggeometry);
				group->addChild(geode);
			}
			return group;
		}
		
		
    void setup() {

        osg::Group* g = getWorld();
        
        for(unsigned int i = 0; i < 1; ++i) 
        {
            g->addChild(createTextGeode("Text on splines is nice ..."));
        }
        
        getMainWindow()->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
    
    }
    
    void draw() 
    {
        if(getDrawCallbackUsage() == UsePreDrawCallback) 
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBegin(GL_TRIANGLES);
        }
        
        glColor3f(1,0,0);
        glVertex3f(0,0,0);
        glVertex3f(100,0,0);
        glVertex3f(0,100,0);
        glEnd();
    }


};


#pragma mark - 

int main(int argc, char *argv[])
{
	cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	
	osg::ref_ptr<TestSketch> app = new TestSketch();
	app->run();
	
	return 0;
}
