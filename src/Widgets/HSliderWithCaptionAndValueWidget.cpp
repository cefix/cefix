/*
 *  HSliderWithCaptionAndValueWidget.cpp
 *  shadertest
 *
 *  Created by Stephan Huber on 18.06.08.
 *  Copyright 2008 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include <cefix/TextWidget.h>
#include <cefix/WidgetFactory.h>
#include <cefix/WidgetDefaults.h>
#include <cefix/Serializer.h>
#include <cefix/WidgetDefaults.h>
#include "HSliderWithCaptionAndValueWidget.h"

namespace cefix
{
	
	 HSliderWithCaptionAndValueWidget::HSliderWithCaptionAndValueWidget	(
			const std::string& id, 
			const std::string& caption, 
			const osg::Vec3& pos, 
			const osg::Vec2& size, 
			const osg::Vec2& throbbersize, 
			cefix::floatRange range, 
			Style style,
			const std::string& fontname, 
			float fontsize, 
			AbstractWidget::Responder* responder
		)
	:	TransformWidget(id, osg::Matrix::translate(pos)),
		AbstractWidget::Responder(),
		_range(range),
		_style(style),
		_fontname(fontname),
		_fontsize(fontsize),
		_inset(osg::Vec2(3,3)),
		_value(range.mid())
	{
		if (_style == UNKNOWN) _style = WidgetDefaults::getHSliderWithCaptionAndValueWidgetStyle();
		_sliderStartX = size[0]*2/6.0f;
		_sliderWidth = size[0]*0.5f;
		
		createWidgets(id, caption, size, throbbersize);
		if(responder) addResponder(responder);
	}
	
	HSliderWithCaptionAndValueWidget::HSliderWithCaptionAndValueWidget(PropertyList* pl)
	:	TransformWidget(pl->get("id")->asString(), osg::Matrix::translate(pl->get("position")->asVec3())),
		AbstractWidget::Responder(),
		_range(pl->get("min")->asFloat(), pl->get("max")->asFloat()),
		_style(WidgetDefaults::getHSliderWithCaptionAndValueWidgetStyle()),
		_fontname("system.xml"),
		_fontsize(0.0f),
		_inset(osg::Vec2(3,3)),
		_value(_range.mid())
	{
		osg::Vec2 size = pl->get("size")->asVec2();
		_sliderStartX = size[0]*2/6.0f;
		_sliderWidth = size[0]*0.5f;
		
		osg::Vec2 throbbersize(pl->get("throbbersize")->asVec2()[1] * 2, pl->get("throbbersize")->asVec2()[1]);
		
		if (pl->hasKey("sliderstartx")) _sliderStartX = pl->get("sliderstartx")->asFloat();
		if (pl->hasKey("sliderwidth")) _sliderWidth = pl->get("sliderwidth")->asFloat();
		
		if (pl->hasKey("fontname")) _fontname = pl->get("fontname")->asString();
		if (pl->hasKey("fontsize")) _fontsize = pl->get("fontsize")->asFloat();
		if (pl->hasKey("style") && (pl->get("style")->asString() == "inside")) _style = CAPTION_AND_VALUE_INSIDE;
		if (pl->hasKey("style") && (pl->get("style")->asString() == "right")) _style = CAPTION_AND_VALUE_RIGHT;		
		if (pl->hasKey("inset")) _inset = pl->get("inset")->asVec2();	
		if (pl->hasKey("throbbersize")) throbbersize = pl->get("throbbersize")->asVec2();
			
		createWidgets(pl->get("id")->asString(), pl->get("caption")->asString(), pl->get("size")->asVec2(), throbbersize, pl->get("prefsKey")->asString());
		
		if (pl->hasKey("resolution"))
			setResolution(pl->get("resolution")->asFloat());
			
		if (pl->hasKey("allowedValues")) 
		{
			cefix::Serializer serializer(pl);
			std::vector<double> allowedValues;
			serializer >> "allowedValues" >> allowedValues;
			_slider->setAllowedValues(allowedValues);
		}
	}
		
		
	void HSliderWithCaptionAndValueWidget::createWidgets(const std::string& id, const std::string& caption_str, const osg::Vec2& size, const osg::Vec2& throbberSize, const std::string& widgetprefskey)
	{
		
		osg::Vec3 captionPos, sliderPos, valuePos;
		osg::Vec2 sliderSize;
		osg::Vec2 captionDelta(WidgetDefaults::getHSliderCaptionDelta());
		osg::Vec2 valueDelta(WidgetDefaults::getHSliderValueDelta());
		
		switch (_style) {
		case UNKNOWN:
		case CAPTION_AND_VALUE_OUTSIDE:
			captionPos.set(0,0,10);
			valuePos.set(size[0], 0, 10);
			sliderPos.set(_sliderStartX, 0, 1);
			sliderSize.set(_sliderWidth, size[1]);
			break;
		case CAPTION_AND_VALUE_INSIDE:
			valuePos.set(size[0] - _inset[0], _inset[1], 10);
			captionPos.set(_inset[0], _inset[1], 10);
			sliderPos.set(0, 0, 1);
			sliderSize = size;
			break;
		case CAPTION_AND_VALUE_RIGHT:
			valuePos.set(size[0],0,10);
			captionPos.set(_sliderWidth, 0, 10);
			sliderPos.set(0, 0, 1);
			sliderSize.set(_sliderWidth, size[1]);
			break;
			
		}
		
		captionPos[0] += captionDelta[0];
		captionPos[1] += captionDelta[1];
		valuePos[0] += valueDelta[0];
		valuePos[1] += valueDelta[1];
				
		cefix::WidgetFactory* wf = cefix::WidgetFactory::instance();
		osg::ref_ptr<cefix::PropertyList>	pl(NULL);
		
		// caption widget
		pl = new cefix::PropertyList();
		
		pl->add("id", id+"_caption");
		
		pl->add("position", captionPos);
		pl->add("caption", caption_str);
		pl->add("Font", _fontname);
		pl->add("FontSize", _fontsize);
		pl->add("Alignment", "left");
		if (!widgetprefskey.empty()) pl->add("prefskey", widgetprefskey+"/caption");
		
		osg::ref_ptr<TextWidget> caption = wf->createWidget<TextWidget>("TextWidget", pl.get());
		//std::cout << "alignment " << caption->getTextGeode()->getTextAlignment() << std::endl;
		// Value- widget
		pl = new cefix::PropertyList();
		
		pl->add("id", id+"_value");
		pl->add("position", valuePos);
		pl->add("caption", "");
		pl->add("Font", _fontname);
		pl->add("Alignment", "right");
		pl->add("FontSize", _fontsize);
		if (!widgetprefskey.empty()) pl->add("prefskey", widgetprefskey+"/value");

		
		osg::ref_ptr<TextWidget> value = wf->createWidget<TextWidget>("TextWidget", pl.get());
		
		// slider widget
		pl = new cefix::PropertyList();
		pl->add("id", id +"_slider");
	
		pl->add("position", sliderPos);
		pl->add("size", sliderSize);
		
		pl->add("throbbersize", throbberSize);
		pl->add("min", _range.min() );
		pl->add("max", _range.max() );
		pl->add("value", _value );
		
		if (!widgetprefskey.empty()) pl->add("prefskey", widgetprefskey+"/slider");
		
		_slider = wf->createWidget<HorizontalSliderWidget>("horizontalsliderwidget", pl.get());
		_slider->addResponder(value);
		_slider->addResponder(this);
		
		add(caption);
		add(value);
		add(_slider);
		
		if (pl->hasKey("resolution"))
			setResolution(pl->get("resolution")->asFloat());
		
	}
	
	void HSliderWithCaptionAndValueWidget::respondToAction(const std::string& action, AbstractWidget* widget) {
		if (action == SliderWidget::Actions::valueChanged()) {
			informAttachedResponder(action);
		}
	}
	
	void HSliderWithCaptionAndValueWidget::setCaption(const std::string& caption) {
		TextWidget* w = find<TextWidget*>(getIdentifier()+"_caption", false);
		if (w) w->setCaption(caption);
	}
	
	
	// cefix::WidgetFactory::RegisterProxy<HSliderWithCaptionAndValueWidget> hswcavw("horizontalsliderwithcaptionandvaluewidget", "id position size throbbersize caption");
	
} /* cefix */
