/*
 *  test_appframework.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 08.11.06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */ 
#include <osg/io_utils>
#include <osg/Material>
#include <cefix/AppController.h>
#include <cefix/DataFactory.h>
#include <cefix/Quad2DGeometry.h>
#include <cefix/TextGeode.h>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
#include <osgDB/ReadFile>
#include <cefix/ConvertScreenCoord.h>
#include <cefix/AnimationFactory.h>
#include <cefix/Pickable.h>
#include <cefix/LineStatistics.h>
#include <cefix/StatisticsGroup.h>
#include <cefix/MathUtils.h>
#include <cefix/AllocationObserver.h>
#include <cefix/ColorUtils.h>
#include <cefix/Draggable.h>
#include <cefix/StatisticsGroup.h>
#include <cefix/FileUtils.h>
#include <osgViewer/ViewerBase>
#include <cefix/ApplicationWindow.h>
#include <cefix/SysUtils.h>
#include <cefix/Ellipse2DGeometry.h>
#include <cefix/Version.h>


class AnimateLoggerTester : public cefix::AnimationBase {
	
	public:
		AnimateLoggerTester() : cefix::AnimationBase(0), _last(0) {}
		void setStats(cefix::AbstractStatistics* stats) { _stats = stats; }
	protected:
		virtual void animate(float time) {
		
			if (_last + 3.0f < time) {
				_last = time;
				std::cout << time << " huhu " << std::endl;
				
				osg::Image* img = osgDB::readImageFile("test.jpg");
				_images.push_back(img);
				_images2.push_back(img);
				if (_stats.valid())
					_stats->addValue(cefix::randomf(100));
				
			}
		}

		float _last;
		osg::ref_ptr<cefix::AbstractStatistics> _stats;
		std::vector<osg::ref_ptr< osg::Image> > _images;
		std::vector<osg::ref_ptr< osg::Image> > _images2;
};


class TestPickable : public cefix::Pickable, public osg::Referenced {
	public:
		TestPickable(std::string name) : Pickable(), _name(name) {}
		
		virtual void enter() {
                std::cout << "mouseEnter " << _name << std::endl;
			}
            
			virtual void leave() {
                std::cout << "mouseLeave " << _name <<  std::endl;
			}
            
			virtual void within() {
                std::cout << "mouseInside " << _name <<  std::endl;
			}
			
			
            virtual void down() {
                std::cout << "mouseDown " << _name <<  std::endl;
			}
            
			virtual void up(bool inside) {
                std::cout << "mouseUp " << _name <<  std::endl;
			}
	protected:
		virtual ~TestPickable() {}
	private:
		std::string _name;
};

class PickableCube : public osg::Geode, public cefix::Pickable {

    public:
        PickableCube();
        
        virtual void enter() {
            _over = true;
            _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,1,0.0,1));
            return true;
        }
        
        virtual void within() {
            osg::notify(osg::ALWAYS) << "inside" << std::endl;
            return true;
        }
        
        virtual void down() {
            _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 0.0, 1 ));
            return true;
        }
        
        virtual void leave() {
            _over = false;
            _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1,0.0,0.0,1));
            return true;
        }
        
        virtual void up(bool inside) {
            if (inside)
                _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,1,0.0,1));
            else
                _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1,0.0,0.0,1));
            return true;
        }
        
        osg::Material* getMaterial() { return _mat.get();}
        
        bool getMouseOverFlag() const { return _over; }
        
    protected: 
        osg::ref_ptr<osg::Material> _mat;
        bool _over;

};

class PickableCubeCallback : public osg::NodeCallback {
    protected:
        PickableCube*   _cube;
        bool            _savedFlag;
    public:
        PickableCubeCallback(PickableCube* cube) : _cube(cube), _savedFlag(false) {}
        
        void operator() (osg::Node* node, osg::NodeVisitor* nv) {
            if((nv) && (_cube == node) && (_savedFlag != _cube->getMouseOverFlag())) {
                _savedFlag = _cube->getMouseOverFlag();
                if (_savedFlag) {
                    _cube->getMaterial()->setAlpha(osg::Material::FRONT, 0.5f);
                }
                else {
                    _cube->getMaterial()->setAlpha(osg::Material::FRONT, 1.0f);
                }
            }
        }
    };
                
        

PickableCube::PickableCube() : osg::Geode(), Pickable() {

    allowPropagation();
    
    osg::Box* box = new osg::Box();
    box->setHalfLengths(osg::Vec3(0.5,0.5,0.5));
    static float dim = 10.0f;
    box->setCenter(osg::Vec3(dim/2 - cefix::randomf(dim),dim/2 - cefix::randomf(dim),dim/2 - cefix::randomf(dim)));
    osg::ShapeDrawable* drawable = new osg::ShapeDrawable(box);
    
    osg::StateSet* stateset = getOrCreateStateSet();
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    _mat = new osg::Material;
    _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1,0.0,0.0,1));
    
    stateset->setAttribute(_mat.get());
    
    addDrawable(drawable);
    setUpdateCallback(new PickableCubeCallback(this));
}



class DragMatrixDraggable : public cefix::Draggable {

	public: 
		DragMatrixDraggable(osg::MatrixTransform* mat) : cefix::Draggable(DRAG_ON_SCREEN_PLANE), _mat(mat) {}
		
		virtual void down() {
			osg::Matrix m(*(getCurrentHit().matrix) );
			std::cout << "local: " << getCurrentHit().localIntersectionPoint << std::endl;
			std::cout << "matrix: " << m << std::endl;
			osg::NodePath np = getCurrentHit().nodePath;
			for(osg::NodePath::iterator i = np.begin(); i != np.end(); ++i) {
				std::cout << (*i) << std::endl;
	        }
			cefix::Draggable::down();
		}
		
		bool dragStarted() 
		{
			_lastDelta.set(0,0,0);
			_m  = _mat->getMatrix();
			stopPropagation();
			return true;
		}
		
		virtual bool drag(const osg::Vec3& dragDelta) 
		{
			//std::cout << dragDelta << std::endl;
			
			_mat->setMatrix(_m * osg::Matrix::translate(dragDelta) );
			//_lastDelta = dragDelta;
			return true;
		}
		
		void dragStopped() {
			_mat->setMatrix(_m);
		}
	
	private:
		osg::observer_ptr<osg::MatrixTransform> _mat;
		osg::Matrix _m;
		osg::Vec3 _lastDelta;
};

class DraggableMatrixTransformAdapter : public osg::Referenced, public DragMatrixDraggable {
	public:
		DraggableMatrixTransformAdapter(osg::MatrixTransform* mat) : osg::Referenced(), DragMatrixDraggable(mat) {}

};

class DragQuad2DGeometryDraggable : public cefix::Draggable, public osg::Referenced {
	
	public:
		DragQuad2DGeometryDraggable(cefix::Quad2DGeometry* quad) : cefix::Draggable(), _quad(quad) {}
		
		bool dragStarted() 
		{
			stopPropagation();
			_startRect = _quad->getRect();
			_startLocZ = _quad->getLocZ();
			return true;
		}
		
		bool drag(const osg::Vec3& dragDelta) 
		{
			_quad->setRect(_startRect + osg::Vec4(dragDelta[0], dragDelta[1], dragDelta[0], dragDelta[1]));
			_quad->setLocZ(_startLocZ + dragDelta[2]);
			std::cout << dragDelta << std::endl;
			return true;
		}
		
		void dragStopped() {
			//_quad->setRect(_startRect);
			_quad->setLocZ(_startLocZ);
		}
	
	private:
		osg::ref_ptr<cefix::Quad2DGeometry> _quad;
		osg::Vec4 _startRect;
		float _startLocZ;

};

class Draggable2DQuad : public osg::Geode {
	public:
		Draggable2DQuad(osg::Vec4 rect) : 
			osg::Geode()
		{	
			cefix::Quad2DGeometry* quad = new cefix::Quad2DGeometry(rect);
			addDrawable(quad);
			quad->setDataVariance(osg::Object::DYNAMIC);
			_dq2gd = new DragQuad2DGeometryDraggable(quad); 
			setUserData(_dq2gd);
			_quad = quad;
		}
		
		cefix::Quad2DGeometry* getQuad() { return _quad.get(); }
		
		DragQuad2DGeometryDraggable* getDragQuad2DGeometryDraggable() { return _dq2gd; }
	private:
		osg::ref_ptr<cefix::Quad2DGeometry> _quad;
		DragQuad2DGeometryDraggable* _dq2gd;
		
};

class PickableQuad : public osg::Referenced, public cefix::Pickable {
	
	public:
		PickableQuad(cefix::Quad2DGeometry* quad) : osg::Referenced(), cefix::Pickable(), _quad(quad) {}
		
        virtual void down() {
			_color = _quad->getColor();
			_quad->setColor(osg::Vec4(0.5,1,1,1));
		}
		virtual void up(bool inside) {
			_quad->setColor(_color);
		}
	private:
		osg::ref_ptr<cefix::Quad2DGeometry> _quad;
		osg::Vec4							_color;
};
		
		


class TestAppFramework : public cefix::AppController {

	public:
		TestAppFramework() : cefix::AppController() {
			
			std::cout << cefix::DataFactory::instance()->getApplicationPath() << std::endl;
			
    
		
			// test the inhertance feature

			cefix::DataFactory::instance()->addPrefsKeyToCheckForInheritance("/testPrefsInheritance");
			cefix::DataFactory::instance()->resolveInheritedPreferences();
			
			
			
			for(int i = 1; i <= 4; ++i) {
				std::ostringstream ss;
				ss << "value_" << i;
				
				std::cout << ss.str() << "/text      : " << cefix::DataFactory::instance()->getPreferencesFor<std::string>("/testPrefsInheritance/"+ss.str()+"/text","- not found -") << std::endl;
				std::cout << ss.str() << "/f         : " << cefix::DataFactory::instance()->getPreferencesFor("/testPrefsInheritance/"+ss.str()+"/f", 0.0f) << std::endl;
				std::cout << ss.str() << "/pos       : " << cefix::DataFactory::instance()->getPreferencesFor("/testPrefsInheritance/"+ss.str()+"/pos", osg::Vec3(0,0,0)) << std::endl;
				std::cout << ss.str() << "/range/min : " << cefix::DataFactory::instance()->getPreferencesFor("/testPrefsInheritance/"+ss.str()+"/range/min", 0) << std::endl;
				std::cout << ss.str() << "/range/max : " << cefix::DataFactory::instance()->getPreferencesFor("/testPrefsInheritance/"+ss.str()+"/range/max", 0) << std::endl;
			}
			
			// test adding and changing prefs
			
			cefix::DataFactory* df = cefix::DataFactory::instance();
			
			df->setPreferencesFor("/myCustomPrefs/partOne/rect", osg::Vec4(100,100, 400, 300), "rect");
			df->setPreferencesFor("/myCustomPrefs/partOne/color", osg::Vec4(1, 0, 0.5, 0.5), "color");
			df->setPreferencesFor("/myCustomPrefs/partOne/vec3", osg::Vec3(2,4,6));
			df->setPreferencesFor("/myCustomPrefs/partOne/boolean", 1, "boolean");
			df->setPreferencesFor("/myCustomPrefs/partTwo/float", 123.0f);
			df->setPreferencesFor("/myCustomPrefs/partTwo/double", 456.7);
			df->setPreferencesFor("/myCustomPrefs/partTwo/string", "ein string, zwei string, drei string");
			df->setPreferencesFor("/myCustomPrefs/partTwo/symbol", "einTollesSymbol", "symbol");
			df->setPreferencesFor("/myCustomPrefs/partThree/lalelu", 234);
			df->setPreferencesFor("/myCustomPrefs/partThree/mamemu", 456);
			df->setPreferencesFor("/myCustomPrefs/partTwo/buuleaan", true);
			
			df->removePreferencesFor("/myCustomPrefs/partThree");
			
			// jetzt ein oder zwei der bestehenden prefs aendern:
			df->setPreferencesFor("/myCustomPrefs/partOne/boolean", 0, "boolean");
			df->setPreferencesFor("/myCustomPrefs/partTwo/double", 987.6);
			
			bool b1 = (0 != df->getPreferencesFor("/myCustomPrefs/partTwo/buuleaan", 0));
			bool b2 = df->getPreferencesFor("/myCustomPrefs/partTwo/buuleaan", false);
			
			assert(b1 == b2);
			
			// und sichern
			
			df->savePreferencesFile();
					
			subscribeTo("Keypress", this, &TestAppFramework::handleKeyPress);
		}
		

		void handleKeyPress(const int& key) 
		{			
			if (key == '.')  {
				std::string result = "";
				if (cefix::showSaveFileDialog(result, "screenshot.png", "/Users/stephan/Desktop"))
					exportSnapshot(result, getMainWindow()->getActualWidth()*3, getMainWindow()->getActualHeight()*3);
			}
			else if (key == 'L') {
				std::string result = "";
				if (cefix::showOpenFileDialog(result, "/Users/stephan/Desktop", "xml,pdf,tif,psd"))
					std::cout << "file selected: " << result << std::endl;			
			}
			else if (key == 'S') {
				std::string result = "";
				if (cefix::showSaveFileDialog(result, "test.xml", "/Users/stephan/Desktop"))
					std::cout << "save into file: " << result << std::endl;			
			}
			else if (key == osgGA::GUIEventAdapter::KEY_F1) {
				cefix::SysUtils::sleep();
			} else if (key == osgGA::GUIEventAdapter::KEY_F2) {
				cefix::SysUtils::shutdown();
			} else if (key == osgGA::GUIEventAdapter::KEY_F3) {
				cefix::SysUtils::reboot();
			} 
		}
		
		osg::Group* createWorld() {
		
			getMainWindow()->setWindowTitle("testappframework");
            
            osg::Group* g = new osg::Group();
            
            cefix::ViewPointNode* vpn = new cefix::ViewPointNode(100);
            osg::MatrixTransform* mat = new osg::MatrixTransform();
            mat->addChild(vpn);
            mat->setMatrix(osg::Matrix::translate(20,0,0));
            g->addChild(mat);
            getMainWindow()->getViewPointManipulator()->setViewPointNode(vpn);

		   //setupTiledRendering( 1600, 1200, osg::Vec4(800, 0, 1600, 600) );
		   
            
		   

			setupPerformanceStatistics();
					
			//setWindowRectangle(800,600);
			//setFullscreen(true);
            osg::setNotifyLevel(osg::DEBUG_INFO);
            osg::Node* cow(NULL);
            
            cow = osgDB::readNodeFile("cow.osg");
			if (cow) {
				g->addChild(cow);
		    }
			osg::setNotifyLevel(osg::WARN);
            
						
			for (int i = 0; i < 0; ++i) {
				osg::Geode* geode = new osg::Geode();
				osg::ShapeDrawable* drawable;
				if (1) {
					osg::Box* box = new osg::Box();
					box->setHalfLengths(osg::Vec3(1.5,0.5,0.5));
					drawable = new osg::ShapeDrawable(box);//new osg::Sphere(osg::Vec3(0,0,0),1));
				} else {
					osg::Cone* sphere = new osg::Cone();
					drawable = new osg::ShapeDrawable(sphere);
				}
				
				osg::StateSet* stateset = geode->getOrCreateStateSet();
				stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
				osg::Material* mat = new osg::Material;
				mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1,1,1.0,1));
				
				stateset->setAttribute(mat);
				
				geode->addDrawable(drawable);
				
				osg::MatrixTransform* m = new osg::MatrixTransform();
				static float dim = 150.0f;
				//osg::AutoTransform* at = new osg::AutoTransform();
				//at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
				m->addChild(geode);
				m->setMatrix(osg::Matrix::translate(osg::Vec3(dim/2 - cefix::randomf(dim),dim/2 - cefix::randomf(dim),dim/2 - cefix::randomf(dim))));
				//at->addChild(geode);
				g->addChild(m);
				DraggableMatrixTransformAdapter* dtma = new DraggableMatrixTransformAdapter(m);
				
				
				cefix::DragDebugGeode* debuggeode = new cefix::DragDebugGeode();
				m->addChild(debuggeode);
				dtma->setDragDebugGeode(debuggeode);
				
				geode->setUserData(dtma);
			}
			
			if (1) 
			{
				Draggable2DQuad* d2d = new Draggable2DQuad(osg::Vec4(10,10, 200, 250));
				osg::MatrixTransform* mat = new osg::MatrixTransform();
				osg::Image* img = dynamic_cast<osg::Image*>(osgDB::readObjectFile("test.jpg"));
				if (img) {
					d2d->getQuad()->setTextureFromImage(img);
					d2d->getQuad()->setTextureRect(osg::Vec4(0,0,img->s(), img->t()));
				}
				//mat->setMatrix(osg::Matrix::translate(0,0,100) * osg::Matrix::scale(3, 3, 1));
				mat->addChild(d2d);
				
				
				/*
				cefix::DragDebugGeode* debuggeode = new cefix::DragDebugGeode();
				d2d->getDragQuad2DGeometryDraggable()->setDragDebugGeode(debuggeode);
				mat->addChild(debuggeode);
				*/
				
				get2DLayer()->addChild(mat);
			}
			
			{
				cefix::Ellipse2DGeometry* circle = new cefix::Ellipse2DGeometry(200,200);
				circle->setCenter(300,300);
				osg::Geode* geode = new osg::Geode();
				geode->addDrawable(circle);
				get2DLayer()->addChild(geode);
				
				osg::Image* img = osgDB::readImageFile("test.jpg");
				if (img) {
					circle->setTextureFromImage(img);
					circle->setTextureRect(osg::Vec4(0,0,img->s(), img->t()));
				}
			}
			
			/*
			for(unsigned int i = 0; i <= 5; ++i) {
				osg::Geode* geode = new osg::Geode();
				cefix::Quad2DGeometry* quad = new cefix::Quad2DGeometry(osg::Vec4(i*100, i* 100, i*100 + 150, i*100 + 120));
				osg::Vec4 c = cefix::HSVtoRGB(cefix::randomf(360.0), 1, 1);
				c[3] = 0.7f;
				quad->setColor(c);
				quad->setLocZ(i*200);
				geode->addDrawable(quad);
				geode->setUserData(new PickableQuad(quad));
				get2DLayer()->addChild(geode);
			}*/
			
			return g;
		
		}


};


#pragma mark - 

int main(int argc, char *argv[])
{
	
    std::cout << cefixGetVersionDetailed() << std::endl;
    
	osg::setNotifyLevel(osg::WARN);
	cefix::log::info("main") << "# of processors: " << OpenThreads::GetNumberOfProcessors() << std::endl;
	cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	cefix::log::info("main") << "root path: " << cefix::DataFactory::instance()->getRootPath() << std::endl;
    osg::ref_ptr<TestAppFramework> app = new TestAppFramework();
	app->setUseOptimizerFlag(false);
	//app->setThreadingModel(osgViewer::ViewerBase::DrawThreadPerContext);
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
