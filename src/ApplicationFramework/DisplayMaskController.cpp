/*
 *  DisplayMaskController.cpp
 *  MaskEditor
 *
 *  Created by Stephan Huber on 19.02.11.
 *  Copyright 2011 Digital Mind. All rights reserved.
 *
 */

#include "DisplayMaskController.h"
#include "DisplayMask.h"
 
#include <osg/Geode>
#include <osg/Group>

#include <cefix/AbstractWidget.h>
#include <cefix/Quad2DGeometry.h>
#include <cefix/WidgetComposer.h>
#include <cefix/Serializer.h>



namespace cefix {


DisplayMaskController::DisplayMaskController()
:	osg::Referenced(),
	_editable(false)
{
	_displayMask = new DisplayMask(false);
	_maskImage = new osg::Image();
	_maskGeode = new osg::Geode();
	
	cefix::Quad2DGeometry* geo = cefix::Quad2DGeometry::createFromImage(_maskImage, osg::Vec3(0,0,990));
	geo->setColor(osg::Vec4(0,0,0, 1));
	_maskGeode->addDrawable(geo);
	geo->getOrCreateStateSet()->setBinNumber(300);
	geo->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	geo->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	geo->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	osg::Depth* depth = new osg::Depth();
	depth->setWriteMask(false);
	geo->getOrCreateStateSet()->setAttributeAndModes(depth, osg::StateAttribute::ON);
	
	_maskGeometry = geo;
	
	
	createWidgets();
}
    
DisplayMaskController::~DisplayMaskController()
{
}

void DisplayMaskController::createMask(unsigned int w, unsigned int h) 
{
	_group = new osg::Group();
	_group->getOrCreateStateSet()->setRenderBinDetails(100, "DepthSortedBin");
	_group->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	
	_maskImage->allocateImage(w, h, 1, GL_ALPHA, GL_UNSIGNED_BYTE);
	memset(_maskImage->data(), 0xff, _maskImage->getTotalSizeInBytes());
	_maskImage->dirty();
	_maskGeometry->setTextureFromImage(_maskImage);
	_maskGeometry->setRect(0,0,w,h);
	_maskGeometry->setTextureRect(0,0,w,h);
	_maskGeometry->setColor(osg::Vec4(0,0,0,1.0));
	_group->addChild(_maskGeode);
	_group->addChild(_displayMask->getNode());
	_displayMask->setDimensions(w,h);
	if (_widgets.valid())
		_group->addChild(_widgets->getNode());
		
	setEditable(_editable);
}

void DisplayMaskController::setEditable(bool e) {
	_editable = e;
	if (_group.valid()) {
		_maskGeode->setNodeMask(0xffff * !e);
		_displayMask->setEditable(e);
		_displayMask->getSplineGroup()->setNodeMask(0xffff * e);
		if (_widgets.valid())
			_widgets->getNode()->setNodeMask(0xffff * e);
	}
	if (!_editable) {
		
		_displayMask->updateDisplayMaskInto(_maskImage);
	}
}

void DisplayMaskController::setMaskColor(const osg::Vec4&c)
{
	_maskGeometry->setColor(c);
}


void DisplayMaskController::setMaskAlpha(float a) 
{
	_maskGeometry->setAlpha(a);
}


void DisplayMaskController::createNewSpline()
{
	_displayMask->newSpline(osg::Vec3(_displayMask->getWidth() /2.0, _displayMask->getHeight() /2.0, 0));
}


void DisplayMaskController::deleteSelectedSpline()
{
	_displayMask->removeSelectedSpline();
}


void DisplayMaskController::deleteControlPoint()
{
	_displayMask->removeSelectedControlPoint();
}


void DisplayMaskController::load(const std::string& filename)
{
	osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList(filename);
	cefix::Serializer s(pl.get() );
	s >> "DisplayMask" >> (*_displayMask);
	setEditable(false);
	_filename = filename;
}


void DisplayMaskController::readFrom(cefix::Serializer& serializer)
{
	_displayMask->readFrom(serializer);
	setEditable(false);
}

void DisplayMaskController::writeTo(cefix::Serializer& serializer)
{
	_displayMask->writeTo(serializer);
}



void DisplayMaskController::save(const std::string& filename)
{
	osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
	cefix::Serializer s(pl.get() );
	s << "DisplayMask" << _displayMask;
	pl->saveAsXML(filename, "DisplayMask");
}

void DisplayMaskController::exitEditMode(){
	setEditable(false);
	if (!_filename.empty()) {
		save(_filename);
	}
}


void DisplayMaskController::createWidgets()
{
	cefix::WidgetComposer composer(osg::Vec3(10, 10, 0), 200, 1);
	
    composer.addButton("EXIT EDIT-MODE", AbstractWidget::DEPRESSED, this, &DisplayMaskController::exitEditMode);
	composer.addButton("DELETE POINT", AbstractWidget::DEPRESSED, this, &DisplayMaskController::deleteControlPoint);
	composer.addButton("DELETE SPLINE", AbstractWidget::DEPRESSED, this, &DisplayMaskController::deleteSelectedSpline);
    composer.addButton("ADD SPLINE", AbstractWidget::DEPRESSED, this, &DisplayMaskController::createNewSpline);
	composer.advance(20);
	composer.addSlider("SCALE", cefix::doubleRange(1,8),  &_displayMask->_multiplier)->setResolution(1);
	composer.addCheckbox("BLUR", &_displayMask->_blur);
	composer.addCheckbox("INVERT", &_displayMask->_inverted);
	
    _widgets = composer.createDrawer("DISPLAY MASK", osg::Vec3(10, 0, 900));
	
}


}