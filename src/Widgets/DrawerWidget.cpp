/*
 *  DrawerWidget.cpp
 *  theseuslogo
 *
 *  Created by Stephan Huber on 21.11.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "DrawerWidget.h"
#include <cefix/WidgetFactory.h>
#include <cefix/PropertyList.h>


namespace cefix {

DrawerWidget::DrawerWidget(const std::string& identifier, const osg::Vec3& collapsedPosition, const osg::Vec3& expandedPosition, const osg::Vec2&size, const std::string& caption, TabPosition tabPosition, bool expanded) :
	TransformWidget(identifier, osg::Matrix::identity()),
	_size(size),
	_positionRange(collapsedPosition, expandedPosition),
	_caption(caption),
	_tabPosition(tabPosition),
	_animationDuration(0.25f),
	_tabOffset(10.0f)
{
	_scalar = (expanded) ? 1 : 0;
	_collapsing = _expanding = false;
	_collapsed = !expanded;
	_expanded = expanded;
}


DrawerWidget::DrawerWidget(cefix::PropertyList* pl):
	TransformWidget(pl->get("id")->asString(), osg::Matrix::identity()),
	_size(pl->get("size")->asVec2()),
	_positionRange(pl->get("collapsedposition")->asVec3(), pl->get("expandedposition")->asVec3()),
	_caption(pl->get("caption")->asString()),
	_tabPosition(TOP),
	_animationDuration(0.25f),
	_collapseOtherDrawers(false),
	_tabOffset(10.0f)
{
	bool expanded(false);
	if (pl->hasKey("Expanded")) {
		expanded = (pl->get("expanded")->asInt() != 0);
	}
	if (pl->hasKey("animationDuration")) _animationDuration = pl->get("animationDuration")->asFloat();
	
	if (pl->hasKey("tabPosition")) {
		std::string p = cefix::strToLower(pl->get("tabPosition")->asString());
		if (p=="bottom")
			_tabPosition = BOTTOM;
		else if (p=="left")
			_tabPosition = LEFT;
		else if (p=="right")
			_tabPosition = RIGHT;
	
	}
	if (pl->hasKey("tabOffset"))
		_tabOffset = pl->get("tabOffset")->asFloat();
		
	
	if (pl->hasKey("collapseOtherDrawers"))
		_collapseOtherDrawers = (pl->get("collapseOtherDrawers")->asInt() != 0);
	
	_scalar = (expanded) ? 1 : 0;
	_collapsing = _expanding = false;
	_collapsed = !expanded;
	_expanded = expanded;
	
}


void DrawerWidget::doCollapseOtherDrawers() {
	if (!_collapseOtherDrawers) return;
	
	cefix::GroupWidget* group = dynamic_cast<cefix::GroupWidget*>(getParent());
	if (group) {
	
		std::vector<DrawerWidget*> drawers;
		group->findOfType(drawers);
		for(std::vector<DrawerWidget*>::iterator i = drawers.begin(); i != drawers.end(); ++i) {
			if (*i != this)
				(*i)->collapse();
		}
	}
}

SimpleDrawerDrawImplementation::SimpleDrawerDrawImplementation(DrawerWidget* drawer) :
	DrawerWidget::Implementation(drawer),
	cefix::Pickable(),
	_tabHeight(12),
	_font("system.xml"),
	_fontsize(0),
	_textOffset(osg::Vec3(10,3,1)),
	_backColor(osg::Vec4(0.2,0.2,0.2,0.8)),
	_captionColor(osg::Vec4(1,1,1,0.7))
{
	stopPropagation();
}

				
void SimpleDrawerDrawImplementation::updateDrawerPosition(const osg::Vec3& pos)
{
	getWidget()->setMatrix(osg::Matrix::translate(pos+osg::Vec3(0,0,1)));
}

void SimpleDrawerDrawImplementation::applyPropertyList(PropertyList* pl)
{
	if (pl->hasKey("font")) _font = pl->get("font")->asString();
	if (pl->hasKey("fontSize")) _fontsize = pl->get("fontsize")->asFloat();
	if (pl->hasKey("textOffset")) _textOffset = pl->get("textOffset")->asVec3();
	if (pl->hasKey("tabHeight")) _tabHeight = pl->get("tabHeight")->asFloat();
	if (pl->hasKey("backgroundColor")) _backColor = pl->get("backgroundColor")->asVec4();
	if (pl->hasKey("captionColor")) _captionColor = pl->get("captionColor")->asVec4();
}



void SimpleDrawerDrawImplementation::createGeometry() 
{
	osg::Vec2 size = getWidget()->getSize();
	
	_textgeode = new cefix::Utf8TextGeode(_font, _fontsize);
	_textgeode->setText(getWidget()->getCaption());
	_textTransform = new osg::MatrixTransform();
	_textTransform->addChild(_textgeode.get());
	
	float tab_offset = getWidget()->getTabOffset();
	
	switch(getWidget()->getTabPosition()) {
		case DrawerWidget::TOP:
			_textgeode->setPosition(_textOffset+osg::Vec3(tab_offset, size[1], 1));
			break;
		case DrawerWidget::BOTTOM:
			_textgeode->setPosition(_textOffset+osg::Vec3(tab_offset, -_tabHeight, 1));
			break;
		case DrawerWidget::LEFT:
			_textgeode->setPosition(_textOffset+osg::Vec3(tab_offset,0 , 1));
			_textTransform->setMatrix(osg::Matrix::rotate(osg::PI/2, osg::Vec3(0,0,1)));
			break;
		case DrawerWidget::RIGHT:
			_textgeode->setPosition(osg::Vec3(-_textOffset[0], _textOffset[1], _textOffset[2])+osg::Vec3(-tab_offset, size[0] , 1));
			_textgeode->setTextAlignment(cefix::AsciiTextGeode::RenderIterator::ALIGN_RIGHT);
			_textTransform->setMatrix(osg::Matrix::rotate(-osg::PI/2, osg::Vec3(0,0,1)));
			break;
	}
	_textgeode->setTextColor(_captionColor);
	osg::Geode* geode = new osg::Geode();
	osg::Geometry* geo = new osg::Geometry();
	
	osg::Vec4Array* c = new osg::Vec4Array();
	c->push_back(_backColor);
	geo->setColorArray(c);
	geo->setColorBinding(osg::Geometry::BIND_OVERALL);
	
	osg::Vec3Array* vec = new osg::Vec3Array();
	vec->resize(9);
	(*vec)[0].set(0, 0,-1);
	(*vec)[1].set(size[0], 0, -1);
	(*vec)[2].set(size[0], size[1], -1);
	(*vec)[3].set(0,size[1],-1);
	
	osg::DrawElementsUShort* da = new osg::DrawElementsUShort(GL_TRIANGLES,5*3);
	(*da)[ 0] = 0; (*da)[ 1] = 1; (*da)[ 2] = 3;
	(*da)[ 3] = 1; (*da)[ 4] = 2; (*da)[ 5] = 3;
	(*da)[ 6] = 7; (*da)[ 7] = 4; (*da)[ 8] = 6;
	(*da)[ 9] = 4; (*da)[10] = 5; (*da)[11] = 6;
	(*da)[12] = 4; (*da)[13] = 8; (*da)[14] = 5;
	
	geo->addPrimitiveSet(da);
	geo->setVertexArray(vec);
	_vec = vec;
	_geo = geo;
	
	updateTab();
	
	geode->addDrawable(geo);
	geode->setUserData(this);
	dynamic_cast<osg::Group*>(getWidget()->getNode())->addChild(geode);
	dynamic_cast<osg::Group*>(getWidget()->getNode())->addChild(_textTransform.get());

}

void SimpleDrawerDrawImplementation::updateTab() {
	osg::Vec2 size = getWidget()->getSize();
	
	float tab_offset = getWidget()->getTabOffset();

	switch(getWidget()->getTabPosition()) {
	
		case DrawerWidget::TOP: 
			(*_vec)[4].set(tab_offset +_textOffset[0] * 2 + _textgeode->getWidth(), size[1], -1);
			(*_vec)[5].set(tab_offset + _textOffset[0] * 2 + _textgeode->getWidth(), size[1] + _tabHeight, -1);
			(*_vec)[6].set(tab_offset, size[1] + _tabHeight, -1);
			(*_vec)[7].set(tab_offset, size[1] + 0, 0);
			(*_vec)[8].set(tab_offset + _textOffset[0] * 2 + _textgeode->getWidth() + _tabHeight, size[1], -1);
			break;
		case DrawerWidget::BOTTOM: 
			(*_vec)[4].set(tab_offset +_textOffset[0] * 2 + _textgeode->getWidth(), 0, -1);
			(*_vec)[5].set(tab_offset + _textOffset[0] * 2 + _textgeode->getWidth(), - _tabHeight, -1);
			(*_vec)[6].set(tab_offset, -_tabHeight, -1);
			(*_vec)[7].set(tab_offset,  0, 0);
			(*_vec)[8].set(tab_offset + _textOffset[0] * 2 + _textgeode->getWidth() + _tabHeight, 0, -1);
			break;
		
		case DrawerWidget::LEFT: 
			(*_vec)[4].set(0, tab_offset + _textOffset[0] * 2 + _textgeode->getWidth(), -1);
			(*_vec)[5].set(-_tabHeight , tab_offset + _textOffset[0] * 2 + _textgeode->getWidth(), -1);
			(*_vec)[6].set(-_tabHeight, tab_offset, -1);
			(*_vec)[7].set(0, tab_offset, 0);
			(*_vec)[8].set(0, tab_offset + _textOffset[0] * 2 + _textgeode->getWidth() + _tabHeight, -1);
			break;
			
		case DrawerWidget::RIGHT: 
			(*_vec)[4].set(size[0], tab_offset + _textOffset[0] * 2 + _textgeode->getWidth(), -1);
			(*_vec)[5].set(size[0] + _tabHeight , tab_offset + _textOffset[0] * 2 + _textgeode->getWidth(), -1);
			(*_vec)[6].set(size[0] + _tabHeight, tab_offset, -1);
			(*_vec)[7].set(size[0], tab_offset, 0);
			(*_vec)[8].set(size[0], tab_offset + _textOffset[0] * 2 + _textgeode->getWidth() + _tabHeight, -1);
			break;
		default:
			std::cout << "not implemented yet" << std::endl;
	
	};
	_geo->dirtyDisplayList();
	_geo->dirtyBound();
	
	switch(getWidget()->getTabPosition()) {
		case DrawerWidget::TOP:
			_textgeode->setPosition(_textOffset+osg::Vec3(tab_offset, size[1], 1));
			break;
		case DrawerWidget::BOTTOM:
			_textgeode->setPosition(_textOffset+osg::Vec3(tab_offset, -_tabHeight, 1));
			break;
		case DrawerWidget::LEFT:
			_textgeode->setPosition(_textOffset+osg::Vec3(tab_offset,0 , 1));
			_textTransform->setMatrix(osg::Matrix::rotate(osg::PI/2, osg::Vec3(0,0,1)));
			break;
		case DrawerWidget::RIGHT:
			_textgeode->setPosition(osg::Vec3(-_textOffset[0], _textOffset[1], _textOffset[2])+osg::Vec3(-tab_offset, size[0] , 1));
			_textgeode->setTextAlignment(cefix::AsciiTextGeode::RenderIterator::ALIGN_RIGHT);
			_textTransform->setMatrix(osg::Matrix::rotate(-osg::PI/2, osg::Vec3(0,0,1)));
			break;
	}

}

void SimpleDrawerDrawImplementation::down() {
	stopPropagation();
}

void SimpleDrawerDrawImplementation::up(bool inside) {
	bool doFlipFlap = false;
	switch(getWidget()->getTabPosition()) {
		case DrawerWidget::TOP:
			if (getCurrentHit().localIntersectionPoint[1] > getWidget()->getSize()[1])
				doFlipFlap = true;
			break;
		case DrawerWidget::BOTTOM:
			if (getCurrentHit().localIntersectionPoint[1] < 0)
				doFlipFlap = true;
			break;
		case DrawerWidget::LEFT:
			if (getCurrentHit().localIntersectionPoint[0] < 0)
				doFlipFlap = true;
			break;
		case DrawerWidget::RIGHT:
			if (getCurrentHit().localIntersectionPoint[0] > getWidget()->getSize()[0])
				doFlipFlap = true;
			break;
	}
	if (doFlipFlap)
		getWidget()->flipFlap();
	
}


}

