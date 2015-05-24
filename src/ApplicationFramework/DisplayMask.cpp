/*
 *  DisplayMask.cpp
 *  DisplayMaskEditor
 *
 *  Created by Stephan Huber on 10.08.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "DisplayMask.h"
#include <cefix/WindowRotation.h>
#include <cefix/Serializer.h>
#include <cefix/DataFactory.h>
#include <cefix/AppController.h>
#include <cefix/Pixel.h>
#include <cefix/ImageUtilities.h>
#include <cefix/Pickable.h>

namespace cefix {


class ClearSelectionPickable : public osg::Referenced, public cefix::Pickable {
public:
	ClearSelectionPickable(DisplayMask* dm) : osg::Referenced(), cefix::Pickable(), _dm(dm) {}
	
	virtual void down() {
		if(_dm.valid()) _dm->resetSelection();
		return cefix::Pickable::down();
	}
private:
	osg::observer_ptr<DisplayMask> _dm;
};

DisplayMask::DisplayMask(bool editable)
:	osg::Referenced(),
	_filename("untitled_DisplayMask.xml"),
	_editable(editable),
	_splines(),
	_group(new osg::Group()),
	_childs(new osg::Group()),
	_inverted(false),
	_blur(false),
	_multiplier(2)
{
	_group->addChild(_childs);
	
	osg::Geode * geode = new osg::Geode();
	
	_backdrop = new cefix::Quad2DGeometry(0,0,0,0);
	_backdrop->setColor(osg::Vec4(0,0,0,0.8));
	geode->addDrawable(_backdrop);
	_group->addChild(geode);
	
	geode->setUserData(new ClearSelectionPickable(this));
	_backdropGeode = geode;
}


DisplayMask::~DisplayMask()
{
	if (_group->getNumParents() > 0) {
		for(int i=_group->getNumParents() - 1; i >= 0; --i) {
			_group->getParent(i)->removeChild(_group.get());
		}
	}

}
void DisplayMask::newSpline(const osg::Vec3& center) 
{
	EditableSpline* spline = new EditableSpline(_editable);
	cefix::SplineGeometry* splineGeo = spline->getSplineGeo();
	
	splineGeo->addControlPoint(center + osg::Vec3(-100,-100, 0));
	splineGeo->addControlPoint(center + osg::Vec3(-80, -100, 0));
	splineGeo->addControlPoint(center + osg::Vec3( 80, -100, 0));
	splineGeo->addControlPoint(center + osg::Vec3(100, -100, 0));
	
	splineGeo->addControlPoint(center + osg::Vec3(100, -80, 0));
	splineGeo->addControlPoint(center + osg::Vec3(100,  80, 0));
	splineGeo->addControlPoint(center + osg::Vec3(100, 100, 0));
	
	splineGeo->addControlPoint(center + osg::Vec3(  80, 100, 0));
	splineGeo->addControlPoint(center + osg::Vec3( -80, 100, 0));
	splineGeo->addControlPoint(center + osg::Vec3(-100, 100, 0));
	
	splineGeo->addControlPoint(center + osg::Vec3(-100,  80, 0));
	splineGeo->addControlPoint(center + osg::Vec3(-100, -80, 0));
	splineGeo->setSplineClosed(true);
	splineGeo->setMode(cefix::BezierSpline);
	_splines.push_back(spline);
	_childs->addChild(spline);

	resetSelection();
	spline->setClearSelectionCallback(cefix::Functor::create(this, &DisplayMask::resetSelection));
	spline->setSelected(true);
	

	spline->recreateControls();
}




void DisplayMask::readFrom(cefix::Serializer& serializer) 
{
	serializer >> "width" >> _width;
	serializer >> "height" >> _height;
	serializer >> "splines" >> _splines;
	serializer >> "editable" >> _editable;
	serializer >> "inverted" >> _inverted;
	serializer >> "do_blur" >> _blur;
	serializer >> "multiplier" >> _multiplier;
	
	_multiplier = osg::clampTo<unsigned int>(_multiplier, 1, 8);
	
	setEditable(_editable);
	setDimensions(_width, _height);
	
	_childs->removeChildren(0, _childs->getNumChildren());
	for(unsigned int i = 0; i < _splines.size();++i) {
		_childs->addChild(_splines[i].get());
		_splines[i]->setClearSelectionCallback(cefix::Functor::create(this, &DisplayMask::resetSelection));
	}
}


void DisplayMask::writeTo(cefix::Serializer& serializer)
{
	serializer << "width" << _width;
	serializer << "height" << _height;
	serializer << "splines" << _splines;
	serializer << "editable" << _editable;
	serializer << "inverted" << _inverted;
	serializer << "do_blur" << _blur;
	serializer << "multiplier" << _multiplier;
}

class FinalCallback : public osg::Camera::DrawCallback
{
public:

FinalCallback(osg::Group* group, osg::Image* image, osg::Image* target, unsigned int multiply, bool inverted, bool blur)
:	osg::Camera::DrawCallback(),
	_group(group),
	_image(image),
	_target(target),
	_multiply(multiply),
	_inverted(inverted),
	_blur(blur)
{}

virtual void operator() (osg::RenderInfo &renderInfo) const
{
	
	cefix::ImageUtilities util(_image.get());
	osg::ref_ptr<osg::Image> temp(NULL);
	
	if (_blur)
		temp = util.scale<cefix::GaussianImageFilter>(_image->s() / _multiply, _image->t() / _multiply);
	else 
		temp = util.scale<cefix::BilinearImageFilter>(_image->s() / _multiply, _image->t() / _multiply);
	
	osg::ref_ptr<osg::Image> result = _target;
	cefix::Pixel p_src(temp.get());
	cefix::Pixel p_dst(result.get());
	
	for( int y = 0; y < temp->t(); ++y) {
		for( int x = 0; x < temp->s(); ++x) {
			if (_inverted)
				p_dst.setR(x,y, 255-p_src.r(x,y));
			else 
				p_dst.setR(x,y, p_src.r(x,y));

		}
	}
		
	_target->dirty();
	_group->removeChild(renderInfo.getCurrentCamera());
	
}

private:
	osg::Group* _group;
	osg::ref_ptr<osg::Image> _image, _target;
	std::string _filename;
	unsigned int _multiply;
	bool	_inverted, _blur;
	
};

void DisplayMask::updateDisplayMaskInto(osg::Image* img) 
{
	resetSelection();
	unsigned int multiply = _multiplier;
	osg::ref_ptr<osg::Image> render_image = new osg::Image();
	render_image->allocateImage(_width*multiply, _height*multiply, 1, GL_RGBA, GL_UNSIGNED_BYTE);
	
	//offscreen camera
	
	osg::Camera* camera = new osg::Camera();
	cefix::WindowRotation w_rot;
	w_rot.setViewport(0, 0, _width*multiply, _width*multiply);

	
	osg::ref_ptr<osg::Viewport> vp = new osg::Viewport(0,0,_width*multiply,_height*multiply);

	camera->setViewport( vp.get() );
	camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
	camera->setRenderOrder(osg::Camera::PRE_RENDER);
	
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	w_rot.setupHudCamera(camera, _width, _height);

	camera->setViewMatrix(osg::Matrix::identity());
	
	camera->setFinalDrawCallback(new FinalCallback(_group.get(), render_image.get(), img, multiply, _inverted, _blur));
	
	for(unsigned int i = 0; i < _childs->getNumChildren(); ++i) {
		camera->addChild(_childs->getChild(i));
	}
	_group->addChild(camera);
	camera->attach(osg::Camera::COLOR_BUFFER, render_image.get());
	
	camera->setClearColor(osg::Vec4(0,0,0,1));
	camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
}

}
