/*
 *  ProgressBarWidget.cpp
 *  cefixcvTemplate
 *
 *  Created by Stephan Huber on 22.10.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "ProgressBarWidget.h"
#include <cefix/WidgetFactory.h>
#include <cefix/ColorUtils.h>

namespace cefix {


class InfiniteProgressbarCallback : public osg::NodeCallback {

public:
InfiniteProgressbarCallback(SimpleProgressBarWidgetImplementation* impl) : osg::NodeCallback(), _impl(impl) {}

virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) 
{
	osg::Vec3 p = _impl->getWidget()->getPosition();
	osg::Vec2 s = _impl->getWidget()->getSize();
	
	float t = nv->getFrameStamp()->getSimulationTime();
	float dc = (0.5 + 0.5*sin(t * 3.0)) * (s[0]-40);
	_impl->_front->setRect(p[0] + dc+1 , p[1]+1, 38, s[1]-2);
	_impl->_front->setLocZ(p[2]+1);
}

private:
	SimpleProgressBarWidgetImplementation* _impl;

};

SimpleProgressBarWidgetImplementation::SimpleProgressBarWidgetImplementation(ProgressBarWidget* widget)
:	ProgressBarWidget::Implementation(widget)

{
	_geode = new osg::Geode();
	
	_back = new cefix::Quad2DGeometry(0,0,0,0);
	_front = new cefix::Quad2DGeometry(0,0,0,0);
	_back->setColor(osg::Vec4(0.5,0.5,0.5,1.0));
	_geode->addDrawable(_back.get());
	_geode->addDrawable(_front.get());
	
	setForegroundColor(cefix::rgb(0xffffff));
	setBackgroundColor(cefix::rgb(0x555555));
	setDisabledColor(cefix::rgb(0x666666));
	
	update();
	
}



void SimpleProgressBarWidgetImplementation::applyPropertyList(cefix::PropertyList* pl) 
{
	if (pl->hasKey("foregroundcolor"))
		setForegroundColor(pl->get("foregroundcolor")->asVec4());
		
	if (pl->hasKey("backgroundcolor"))
		setBackgroundColor(pl->get("backgroundcolor")->asVec4());
	if (pl->hasKey("disabledcolor"))
		setDisabledColor(pl->get("disabledcolor")->asVec4());
}



void SimpleProgressBarWidgetImplementation::update() {

	_front->setColor( getWidget()->isEnabled() ? _forecolor : _disabledcolor);

	osg::Vec3 p = getWidget()->getPosition();
	osg::Vec2 s = getWidget()->getSize();
	
	_back->setRect(p[0], p[1], s[0], s[1]);
	_back->setLocZ(p[2]);
	float perc = getWidget()->getPercentDone();
	
	if (perc < 0) {
		if (_geode->getUpdateCallback() == NULL)
			_geode->setUpdateCallback(new InfiniteProgressbarCallback(this));
	} else
		_geode->setUpdateCallback(NULL);
		
	if (perc >= 0) {
		_front->setRect(p[0]+1, p[1]+1, (s[0]-2) * perc / 100.0,  s[1]-2);
		_front->setLocZ(p[2] + 1);
	}
}

}

// static cefix::WidgetFactory::RegisterProxy<cefix::SimpleProgressBarWidget> simple_progress_bar_widget("simpleprogressbarwidget", "id position size");