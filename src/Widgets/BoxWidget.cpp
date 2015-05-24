/*
 *  BoxWidget.cpp
 *  dm_musikautomaten
 *
 *  Created by Stephan Huber on 30.10.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "BoxWidget.h"
#include <cefix/Quad2DGeometry.h>
#include <cefix/PropertyList.h>
#include <cefix/ColorUtils.h>
#include <cefix/WidgetFactory.h>
#include <cefix/WidgetThemeManager.h>
#include <cefix/ThemedWidgetStateGeometry.h>
#include <osg/io_utils>

namespace cefix {

BoxWidget::BoxWidget(const std::string& identifier, const osg::Vec3& position, const osg::Vec2& size, const std::string& caption)
:	RectControlWidgetT<TransformWidget>(identifier, position, size),
	_caption(NULL),
	_captionDelta(0,0,0)
{
	createCaption(caption);
	setMatrix(osg::Matrix::translate(position));
	add(_caption);
	setCaption(caption);
}
	
BoxWidget::BoxWidget(cefix::PropertyList* pl)
:	RectControlWidgetT<TransformWidget>(pl->get("id")->asString(), pl->get("position")->asVec3(), pl->get("size")->asVec2()),
	_caption( NULL ),
	_captionDelta( pl->get("captionDelta")->asVec3() )
{

	setMatrix(osg::Matrix::translate(pl->get("position")->asVec3()));
	createCaption(pl->get("caption")->asString(), pl);
	add(_caption);
	setCaption(pl->get("caption")->asString());
}


void BoxWidget::createCaption( const std::string& caption, cefix::PropertyList* apl) {

	osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
	pl->add("prefs", apl);
	pl->add("identifier", "boxwidget_caption");
	pl->add("position", osg::Vec3(0,0,0));
	pl->add("caption", caption);
	
	_caption = cefix::WidgetFactory::instance()->createWidget<TextWidget>("textwidget", pl);
}

void SimpleBoxDrawImplementation::applyPropertyList(cefix::PropertyList* pl)
{
	if (pl->hasKey("backgroundColor"))
		_geo->setColor(pl->get("backgroundColor")->asVec4());

}

void SimpleBoxDrawImplementation::update()
{
	_geo->setRect(0,0,getWidget()->getWidth(), getWidget()->getHeight());
}
	
void SimpleBoxDrawImplementation::createGeometry()
{
	_node = new osg::Geode();
	_geo = new cefix::Quad2DGeometry(0,0, getWidget()->getWidth(), getWidget()->getHeight());
	_geo->setColor(cefix::rgb(0x333333));
	_geo->setLocZ(1);
	_node->addDrawable(_geo);
}


}

