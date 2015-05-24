/*
 *  ImageWidget.cpp
 *  VideoDonut
 *
 *  Created by Stephan Huber on 29.11.07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <osgDB/ReadFile>
#include "ImageWidget.h"
#include <cefix/PropertyList.h>
#include <cefix/WidgetFactory.h>
#include <cefix/HorizontalSliderWidget.h>
#include <cefix/Draggable.h>

namespace cefix {

ImageWidget::ImageWidget(const std::string& identifier, const osg::Vec3& position, const osg::Vec2& size, osg::Image* image, ZoomMode zoommode)  :
	RectControlWidget(identifier, position, size),
	_image(image),
	_zoomMode(zoommode),
	_zoomLevel(1.0f),
	_scrollDelta(0,0),
	_horizontalFlip(false)
{
}

		
ImageWidget::ImageWidget(cefix::PropertyList* pl) :
	RectControlWidget(
		pl->get("id")->asString(), 
		pl->get("position")->asVec3(), 
		pl->get("size")->asVec2()
	),
	_zoomMode(AUTO),
	_zoomLevel(1.0f),
	_scrollDelta(0,0),
	_horizontalFlip(false)
{
	
	std::string zoomMode = cefix::strToLower(pl->get("zoomMode")->asString());
	if (zoomMode == "auto_proportional") 
		_zoomMode = AUTO_PROPORTIONAL;
	else if (zoomMode == "manual")
		_zoomMode = MANUAL;
	
	if (pl->hasKey("zoomlevel")) _zoomLevel = pl->get("zoomlevel")->asFloat();
	if (pl->hasKey("image")) _image = osgDB::readImageFile(pl->get("image")->asString());
	if (pl->hasKey("scrolldelta")) _scrollDelta = pl->get("scrollDelta")->asVec2();
	if (pl->hasKey("horizontalflip")) _horizontalFlip = (pl->get("horizontalflip")->asInt() !=0);
	
	if (_impl.valid()) _impl->applyPropertyList(pl);
	
}


void ImageWidgetZoomResponder::respondToAction (const std::string &action, AbstractWidget *widget) 
{			
	if (action == SliderWidget::Actions::valueChanged() && _imageWidget.valid()) {
		HorizontalSliderWidget* hslider = dynamic_cast<HorizontalSliderWidget*>(widget);
		if (widget) _imageWidget->setZoomLevel(hslider->getValue());
	}

}

class SimpleImageWidgetPickable : public cefix::Draggable, public osg::Referenced {
	
	public:
		SimpleImageWidgetPickable(ImageWidget* w) : cefix::Draggable(), _widget(w) { stopPropagation(); }
		
		virtual bool dragStarted() {
			_delta = _widget->getScrollDelta();
			return (_widget->getZoomMode() == ImageWidget::MANUAL);
		}
		
		virtual bool drag(const osg::Vec3& delta) {
			_widget->setScrollDelta(-osg::Vec2(delta[0], delta[1]) + _delta);
			return true;
		}
	private:
		ImageWidget* _widget;
		osg::Vec2 _delta;

};

SimpleImageWidgetDrawImplementation::SimpleImageWidgetDrawImplementation(ImageWidget* widget) : 
	ImageWidget::Implementation(widget),
	_cachedImage(NULL)
{
	_geode = new osg::Geode();
	_geode->setUserData(new SimpleImageWidgetPickable(getWidget()));
	_quad = new cefix::Quad2DGeometry(osg::Vec4(0,0,1,1));	
	_geode->addDrawable(_quad.get());
	_geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
}
		
void SimpleImageWidgetDrawImplementation::update() {
	osg::Vec4 r;
	osg::Vec3 p(getWidget()->getPosition());
	osg::Vec2 s(getWidget()->getSize());
	r.set(p[0], p[1], p[0] + s[0], p[1] + s[1]);
	_quad->setRect(r);
	_quad->setLocZ(p[2]);
	
	osg::Image* image = getWidget()->getImage();
	if (image != _cachedImage) {
		_quad->setTextureFromImage(image);
		_cachedImage = image;
	}
	
	osg::Vec4 texRect;
	if (image) {
		switch (getWidget()->getZoomMode()) 
		{
			case ImageWidget::AUTO:
				texRect.set(0,0,image->s(), image->t());
				break;
				
			case ImageWidget::MANUAL:
				{
					double sx( s[0] / image->s() );
					double sy( s[1] / image->t() );

					double scale = getWidget()->getZoomLevel();
					osg::Vec2 scrollDelta = getWidget()->getScrollDelta();
					if (getWidget()->getHorizontalFlip())
						scrollDelta[0] = image->s() - scrollDelta[0];
					if (image->getOrigin() == osg::Image::TOP_LEFT)
						scrollDelta[1] = image->t() - scrollDelta[1];
						
					scale = osg::maximum(sy, osg::maximum(sx,scale));
					
					double dx(0), dy(0);
					double proj_w(image->s()), proj_h(image->t());
					
					proj_w = s[0] / scale;
					proj_h = s[1] / scale;
					
					dx = osg::minimum<float>(osg::maximum<float>(0, scrollDelta[0]/scale), image->s() - proj_w);
					dy = osg::minimum<float>(osg::maximum<float>(0, scrollDelta[1]/scale), image->t() - proj_h);
					
					texRect.set(dx,dy,dx+proj_w, dy+proj_h);
				}
				break;
			
			case ImageWidget::AUTO_PROPORTIONAL:
				{
					double dx(0), dy(0);
					double proj_w(image->s()), proj_h(image->t());
					
					double sx( s[0] / image->s() );
					double sy( s[1] / image->t() );
					
					if (sx < sy) {
						proj_w = image->t()  * s[0] / s[1];
						dx = (image->s() - proj_w) / 2.0;
					} else {
						proj_h = image->s() * s[1] / s[0];
						dy = (image->t() - proj_h) / 2.0;
					}
					
					texRect.set(dx,dy, dx + proj_w, dy + proj_h);
				}
				break;

		}
		if (image->getOrigin() == osg::Image::TOP_LEFT) {
			std::swap(texRect[1],texRect[3]);
		}
		if (getWidget()->getHorizontalFlip()) {
			std::swap(texRect[0],texRect[2]);
		}
		_quad->setTextureRect(texRect);
	}
	
}


void SimpleImageWidgetDrawImplementation::applyPropertyList(cefix::PropertyList* pl) {

}





}