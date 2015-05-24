/*
 *  ImageFactory_test.cpp
 *  SimpleGLUTViewer
 *
 *  Created by Stephan Huber on Tue Nov 18 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */
 
 #define __USE_OSX_AGL_IMPLEMENTATION__
#include <sstream>

#include <cefix/Log.h>
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include <osg/Material>
#include <osg/Depth>
#include <osg/Stateset>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <cefix/TextGeode.h>
#include <osg/Autotransform>
#include <osg/io_utils>



#include <cefix/Sketch.h>
#include <cefix/PickEventHandler.h>
#include <cefix/MathUtils.h>
#include <cefix/MouseLocationProvider.h>
#include <cefix/Quad2DGeometry.h>
#include <cefix/Draggable.h>
#include <cefix/FileUtils.h>


class PickableCube : public osg::Referenced, public cefix::Pickable {

    public:
        PickableCube(osg::Geode* geode);
        
        virtual void enter() {
            _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,1,0.0,1));
            _over = true;
        }
        
        virtual void within() {
            //osg::notify(osg::ALWAYS) << "inside" << std::endl;
        }
        
        virtual void down() {
            _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1 ));
            /*
            if (cefix::randomf(1.0) < 0.5) {
                _geode->getParent(0)->removeChild(_geode.get());
            }*/
        }
        
        virtual void leave() {
            _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1,0.0,0.0,1));
            _over = false;
        }
        
        virtual void up(bool inside) {
            if (inside)
                _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,1,0.0,1));
            else
                _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1,0.0,0.0,1));
        }
        
        osg::Material* getMaterial() { return _mat.get();}
        
		~PickableCube() {
			std::cout << "pickablecube destroyed" << std::endl;
		}
		
        bool getMouseOverFlag() const { return false; }
    protected: 
        osg::ref_ptr<osg::Material> _mat;
        osg::observer_ptr<osg::Geode> _geode;
        bool _over;

};

class PickableCubeCallback : public osg::NodeCallback {
    protected:
        PickableCube*   _cube;
        bool            _savedFlag;
    public:
        PickableCubeCallback(PickableCube* cube) : _cube(cube), _savedFlag(false) {}
        
        void operator() (osg::Node* node, osg::NodeVisitor* nv) {
            if((nv) && (_savedFlag != _cube->getMouseOverFlag())) {
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
                
        

PickableCube::PickableCube(osg::Geode* geode) : osg::Referenced(), Pickable() {

    osg::Box* box = new osg::Box();
    box->setHalfLengths(osg::Vec3(0.5,0.5,0.5));
    static float dim = 10.0f;
    box->setCenter(osg::Vec3(dim/2 - cefix::randomf(dim),dim/2 - cefix::randomf(dim),dim/2 - cefix::randomf(dim)));
    osg::ShapeDrawable* drawable = new osg::ShapeDrawable(box);
    
    osg::StateSet* stateset = geode->getOrCreateStateSet();
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    _mat = new osg::Material;
    _mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1,0.0,0.0,1));
    
    stateset->setAttribute(_mat.get());
    
    geode->addDrawable(drawable);
    geode->setUserData(this);
    geode->setUpdateCallback(new PickableCubeCallback(this));
    _geode = geode;
}


class DraggableCube : public osg::Geode, public cefix::Draggable {

public:
	DraggableCube()
	:	osg::Geode(),
		cefix::Draggable()
	{
		const float dim = 10.0f;
		
		_box = new osg::Box();
		_box->setHalfLengths(osg::Vec3(0.5,0.5,0.5));
		_box->setCenter(osg::Vec3(dim/2 - cefix::randomf(dim),dim/2 - cefix::randomf(dim),dim/2 - cefix::randomf(dim)));
		_drawable = new osg::ShapeDrawable(_box);
		_drawable->setColor(osg::Vec4(0, 1, 1, 0));
		addDrawable(_drawable);
	}
	
	virtual bool dragStarted() 
	{
		stopPropagation();
		_pos = _box->getCenter();
		return true;
	}
	
	virtual bool drag(const osg::Vec3& p) {
		_box->setCenter(p+_pos);
		_drawable->dirtyDisplayList();
		_drawable->dirtyBound();
		return true;
	}
private:
	osg::Box* _box;
	osg::ShapeDrawable* _drawable;
	osg::Vec3 _pos;
};

class MouseCoordinateDisplayCallback : public osg::NodeCallback {
	public:
		MouseCoordinateDisplayCallback(cefix::AsciiTextGeode* tg) : osg::NodeCallback(), _textgeode(tg), _mlp(cefix::MouseLocationProvider::instance()) {}
		
	virtual void operator()(osg::Node*, osg::NodeVisitor* nv) {
		osg::Vec2 m = _mlp->getScreen();
        std::ostringstream ss;
		ss.str(""); ss.clear();
		ss << m << " " << std::endl << _mlp->getNear() << "  " << std::endl << _mlp->getFar() << std::endl;
        // std::cout << ss.str() << std::endl;
		_textgeode->setText(ss.str());
	}
	
	private:
		osg::ref_ptr<cefix::AsciiTextGeode>	_textgeode; 
		osg::ref_ptr<cefix::WindowPickable> _mlp;
};

class GetLocalCoordsOnQuadCallback : public osg::NodeCallback {

	public:
		GetLocalCoordsOnQuadCallback(osg::Geode* geode, cefix::Quad2DGeometry* quad, cefix::AsciiTextGeode* tg) 
		:	osg::NodeCallback(),
			_geode(geode),
			_quad(quad),
			_textgeode(tg)
		{
		}
		
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		
			osg::Vec3 p = cefix::MouseLocationProvider::instance()->getLocalNear(_geode->getParentalNodePaths()[0]);
			static std::stringstream ss;
			ss.str("");
			ss << "Mauskoordinaten lokal zum 2D-Layer: "<< p << std::endl;
			_textgeode->setText(ss.str());
			traverse(node,nv);
		}
		
	private:
		osg::Geode* _geode;
		cefix::Quad2DGeometry* _quad;
		cefix::AsciiTextGeode* _textgeode;
};


class MatrixRotateCallback : public osg::NodeCallback 
{
public:
	MatrixRotateCallback(osg::MatrixTransform* mat): osg::NodeCallback(), _mat(mat) 
	{
		_m = _mat->getMatrix();
	}
	
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) 
	{
		_mat->setMatrix(osg::Matrix::rotate(_alpha, osg::Vec3(1,0,0)) * _m);
		_alpha += 0.01;
	}
private:
	osg::MatrixTransform* _mat;
	osg::Matrix			  _m;
	float _alpha;
};

class CreateCubeAnimation : public cefix::AnimationBase {

public:
    CreateCubeAnimation(osg::Group* group) : cefix::AnimationBase(0), _group(group) {}
    
protected:
    virtual void animate(float elapse) {
    
        if (cefix::randomf(10) < 1) {
            unsigned int ndx = cefix::randomf(_group->getNumChildren()-10);
            _group->removeChild(ndx);
        }
        
        if (cefix::randomf(10) < 1) 
        {
            osg::MatrixTransform* mat = new osg::MatrixTransform();
            mat->setMatrix(
                osg::Matrix::rotate(cefix::randomf(osg::PI), osg::Vec3(cefix::in_between(-1,1),cefix::in_between(-1,1),cefix::in_between(-1,1))) * 
                osg::Matrix::translate(cefix::in_between(-30,30), cefix::in_between(-30,30), cefix::in_between(-30,30))
            );
            if (cefix::randomf(1.0) < 0.5) {
                osg::Geode* geode = new osg::Geode();
                osg::ref_ptr<PickableCube> pickable = new PickableCube(geode);
                mat->addChild(geode);
               
                
            } else {
                mat->addChild(new DraggableCube());
                mat->setUpdateCallback(new MatrixRotateCallback(mat));
            }
            
            _group->addChild(mat);

        }
    }
private:    
    osg::ref_ptr<osg::Group> _group;
};

class MySketch : public cefix::Sketch {
	public:
		MySketch() 
			: cefix::Sketch()
		{
			allowOsgHandler(true);
		}

		void setup() {
			
			osg::ref_ptr<osg::Group> g = getWorld();
			for(int i = 0; i < 200; i++) {
				osg::MatrixTransform* mat2 = new osg::MatrixTransform();
				mat2->setMatrix(
					osg::Matrix::scale(cefix::in_between(0.7,1.3), cefix::in_between(0.7,1.3), cefix::in_between(0.7,1.3)) *
					osg::Matrix::rotate(cefix::randomf(osg::PI), osg::Vec3(cefix::in_between(-1,1),cefix::in_between(-1,1),cefix::in_between(-1,1))) * 
					osg::Matrix::translate(cefix::in_between(-30,30), cefix::in_between(-30,30), cefix::in_between(-30,30))
				);
				osg::MatrixTransform* mat = new osg::MatrixTransform();
				mat->setMatrix(
					osg::Matrix::rotate(cefix::randomf(osg::PI), osg::Vec3(cefix::in_between(-1,1),cefix::in_between(-1,1),cefix::in_between(-1,1))) * 
					osg::Matrix::translate(cefix::in_between(-30,30), cefix::in_between(-30,30), cefix::in_between(-30,30))
				);
				if (cefix::randomf(1.0) < 0.5) {
                    osg::Geode* geode = new osg::Geode();
                    osg::ref_ptr<PickableCube> pc = new PickableCube(geode);
					mat->addChild(geode);
                   
					
				} else {
					mat->addChild(new DraggableCube());
					mat->setUpdateCallback(new MatrixRotateCallback(mat));
                    mat->setNodeMask(0x0002);
                }
				
				mat2->addChild(mat);
				g->addChild(mat2);
			}
			// TExtgeode für die Mausposition:
			osg::ref_ptr<cefix::AsciiTextGeode> textgeode = new cefix::AsciiTextGeode("system.xml", 1, "");
			textgeode->setPosition(osg::Vec3(-0, 0, -10));
			textgeode->setTextColor(osg::Vec4(1,1,1,1));
			osg::ref_ptr<osg::AutoTransform> autotransform = new osg::AutoTransform();
			autotransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
			autotransform->addChild(textgeode.get());
			autotransform->setUpdateCallback(new MouseCoordinateDisplayCallback(textgeode.get()));
			g->addChild(autotransform.get());
			
			{	// 2d Geode
				osg::Geode* geode = new osg::Geode();
				cefix::Quad2DGeometry* quad = new cefix::Quad2DGeometry(osg::Vec4(100,100,300,200));
				cefix::AsciiTextGeode* tg = new cefix::AsciiTextGeode("system.xml", 9, "");
				tg->setPosition(osg::Vec3(10,10,10));
				tg->setTextColor(osg::Vec4(1,0,1,1));
				get2DLayer()->addChild(tg);
				geode->addDrawable(quad);
				geode->setUpdateCallback(new GetLocalCoordsOnQuadCallback(geode, quad, tg));
				get2DLayer()->addChild(geode);
			}
			
            // 
            getMainWindow()->requestPicking();
            
            cefix::AnimationFactory::instance()->add(new CreateCubeAnimation(g.get()));
            
		}
};




#pragma mark - 
int main(int argc, char *argv[])
{
	time_t t(0); 
	cefix::getModifiedTimeStamp("Z:/Desktop/background.psd", t);
	cefix::Date d = cefix::Date::getFromUnixTimeStamp(t);
	std::cout << "modified: " << d << std::endl;
	d -= 60*60*24;
	cefix::setModifiedTimeStamp("Z:/Desktop/background.psd", d.getAsUnixTimeStamp());
	
	cefix::getModifiedTimeStamp("Z:/Desktop/background.psd", t);
	d = cefix::Date::getFromUnixTimeStamp(t);
	std::cout << "new modified: " << d << std::endl;

	osg::setNotifyLevel(osg::NOTICE);
	
    osg::ref_ptr<MySketch> app = new MySketch();
	app->setUseOptimizerFlag(false);
	// create the world and apply it
 
	app->run();
	
	app = NULL;

	
	return 0;
}