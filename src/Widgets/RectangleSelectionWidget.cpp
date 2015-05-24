/*
 *  RectangleSelectionWidget.cpp
 *  CamShiftTracker
 *
 *  Created by Stephan Huber on 08.03.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */
#include "RectangleSelectionWidget.h"
#include <cefix/WidgetFactory.h>
namespace cefix	{

RectangleSelectionWidget::RectangleSelectionWidget(
	const std::string& identifier, 
	const osg::Vec3& position, 
	const osg::Vec4& rect, 
	const osg::Vec4& constrainingRect, 
	bool independentCorners)
:
	TransformWidget(identifier, osg::Matrix::translate(position)),
	_constrainingRect(constrainingRect),
	_independentCorners(independentCorners),
	_referenceSize(1,1),
	_rectIsDraggable(true)
{
	_points = new osg::Vec2Array(4);
	

}



RectangleSelectionWidget::RectangleSelectionWidget(
	const std::string& identifier, 
	const osg::Vec3& position,
	osg::Vec2Array* points, 
	const osg::Vec4& constrainingRect, 
	bool independentCorners)
:
	TransformWidget(identifier, osg::Matrix::translate(position)),
	_constrainingRect(constrainingRect),
	_independentCorners(independentCorners),
	_referenceSize(1,1),
	_rectIsDraggable(true)
	
{
	_points = new osg::Vec2Array(4);
	for(unsigned int i = 0; i < std::min<unsigned int>(4,points->size()); ++i) {
		(*_points)[i] = (*points)[i];
	}
}



	

RectangleSelectionWidget::RectangleSelectionWidget(PropertyList* pl)
:	TransformWidget(pl->get("identifier")->asString(), osg::Matrix::translate(pl->get("position")->asVec3())),
	_constrainingRect(pl->get("constrainingRect")->asVec4()),
	_independentCorners(pl->get("independentCorners")->asInt()),
	_referenceSize(1,1),
	_rectIsDraggable(true)
{
	_points = new osg::Vec2Array(4);
	
	if (pl->hasKey("rect")) {
		osg::Vec4 rect = pl->get("rect")->asVec4();
		(*_points)[0].set(rect[0], rect[1]);
		(*_points)[1].set(rect[2], rect[1]);
		(*_points)[2].set(rect[2], rect[3]);
		(*_points)[3].set(rect[0], rect[3]);
	} else {
		(*_points)[0] = pl->get("topleft")->asVec2();
		(*_points)[1] = pl->get("topright")->asVec2();
		(*_points)[2] = pl->get("bottomright")->asVec2();
		(*_points)[3] = pl->get("bottomleft")->asVec2();
	}
	
	if (pl->hasKey("rectIsDraggable")) {
		setRectIsDraggable(pl->get("rectIsDraggable")->asInt() != 0);
	}
}

osg::Vec2 RectangleSelectionWidget::constrain(const osg::Vec2& p) {
	osg::Vec2 result(p);
	result[0] = osg::clampTo(result[0], _constrainingRect[0], _constrainingRect[2]);
	result[1] = osg::clampTo(result[1], _constrainingRect[1], _constrainingRect[3]);	
	return result;
}


RectangleSelectionWidget::Implementation::CornerDraggable::CornerDraggable(RectangleSelectionWidget* widget, unsigned int ndx)
:	cefix::Draggable(DRAG_ON_CUSTOM_LOCAL_PLANE, osg::Z_AXIS),
	osg::Referenced(),
	_widget(widget),
	_ndx(ndx)
{
	stopPropagation();
}
	
bool RectangleSelectionWidget::Implementation::CornerDraggable::dragStarted () 
{
	if (!_widget->isEnabled()) return false; 
	
	switch (_ndx) {
		case 0: _p = _widget->getTopLeft(); break;
		case 1: _p = _widget->getTopRight(); break;
		case 2: _p = _widget->getBottomRight(); break;
		case 3: _p = _widget->getBottomLeft(); break;
	}
	return true; 
}
	
bool RectangleSelectionWidget::Implementation::CornerDraggable::drag(const osg::Vec3 &delta) {
	
	osg::Vec2 p = osg::Vec2(_p[0] + delta[0], _p[1] + delta[1]);
	p = _widget->constrain(p);
	switch (_ndx) {
		case 0: _widget->setTopLeft(p); break;
		case 1: _widget->setTopRight(p); break;
		case 2: _widget->setBottomRight(p); break;
		case 3: _widget->setBottomLeft(p); break;
	}
	_widget->update();
	return true;
}


RectangleSelectionWidget::Implementation::QuadDraggable::QuadDraggable(RectangleSelectionWidget* widget)
:	cefix::Draggable(),
	osg::Referenced(),
	_widget(widget)
{
	stopPropagation();
}
	
bool RectangleSelectionWidget::Implementation::QuadDraggable::dragStarted () 
{
	if (!_widget->isEnabled()) return false; 

	_p1 = _widget->getTopLeft(); 
	_p2 = _widget->getTopRight(); 
	_p3 = _widget->getBottomRight(); 
	_p4 = _widget->getBottomLeft(); 
	
	return true; 
}
	
bool RectangleSelectionWidget::Implementation::QuadDraggable::drag(const osg::Vec3 &delta) {
	osg::Vec2 p1,p2,p3,p4;
	osg::Vec2 d2(delta[0], delta[1]);
	p1 = _widget->constrain(_p1 + d2);
	p2 = _widget->constrain(_p2 + d2);
	p3 = _widget->constrain(_p3 + d2);
	p4 = _widget->constrain(_p4 + d2);
	
	if (	(p1 == _p1 + d2) && 
			(p2 == _p2 + d2) && 
			(p3 == _p3 + d2) && 
			(p4 == _p4 + d2)
		) 
	{
		_widget->setTopLeft(p1);
		_widget->setTopRight(p2);
		_widget->setBottomRight(p3);
		_widget->setBottomLeft(p4);
	}
	
	_widget->update();
	return true;
}



SimpleRectangleSelectionWidgetImplementation::SimpleRectangleSelectionWidgetImplementation(RectangleSelectionWidget* widget)
:	RectangleSelectionWidget::Implementation(widget)
{
	_group = new osg::Group();
	_group->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	{
		_frame = new cefix::LineStripGeometry(true);
		_frame->resize(4);
		_frame->setColor(osg::Vec4(0.7, 0.7, 0.7, 0.8));
		osg::Geode* geode  = new osg::Geode();
		geode->addDrawable(_frame.get());
		
		_group->addChild(geode);
		if (widget->isRectDraggable()) {
			geode->setUserData(new QuadDraggable(getWidget()));
		}
		
		osg::Geometry* geo = new osg::Geometry();
		geo->setVertexArray(_frame->getVertices());
		geo->setUseDisplayList(false);
		
		osg::Vec4Array* c = new osg::Vec4Array();
		c->push_back(osg::Vec4(0,0,0,0.1));
		geo->setColorArray(c);
		geo->setColorBinding(osg::Geometry::BIND_OVERALL);

		geo->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
		
		
		geode->addDrawable(geo);
		_geode = geode;
	}
	
	_corners.resize(4);
	
	for(unsigned int i = 0; i < 4; ++i) {
		_corners[i] = new cefix::Quad2DGeometry(osg::Vec4(0,0,6,6));
		_corners[i]->setColor(osg::Vec4(0.5, 0.5, 0.5, 0.8));
		osg::Geode* geode = new osg::Geode();
		geode->addDrawable(_corners[i].get());
		geode->setUserData(new CornerDraggable(getWidget(), i));
		_group->addChild(geode);
		
	}
	
	
}


void SimpleRectangleSelectionWidgetImplementation::update(osg::Vec2Array* points) 
{
	if (getWidget()->isRectDraggable() && !_geode->getUserData()) {
		_geode->setUserData(new QuadDraggable(getWidget()));
	}
	
	if (!getWidget()->isRectDraggable() && _geode->getUserData()) {
		_geode->setUserData(NULL);
	}
	
	for(unsigned int i = 0; i < 4; ++i) {
		_corners[i]->setRect((*points)[i][0] - 2, (*points)[i][1] - 2, 5,5); 
		(*_frame->getVertices())[i].set((*points)[i][0], (*points)[i][1], 0);
	}
	_frame->update();
}

void SimpleRectangleSelectionWidgetImplementation::setColor(const osg::Vec4& color){
    for(unsigned int i = 0; i < 4; ++i) {
        _corners[i]->setColor(color);
    }
    _frame->setColor(color);
    _frame->update();
}

void SimpleRectangleSelectionWidgetImplementation::applyPropertyList(PropertyList* pl)
{

}

}


