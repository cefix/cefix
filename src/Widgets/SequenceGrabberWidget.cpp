/*
 *  SequenceGrabberWidget.cpp
 *  CamShiftTracker
 *
 *  Created by Stephan Huber on 23.02.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "SequenceGrabberWidget.h"
#include <cefix/PropertyList.h>
#include <cefix/WidgetFactory.h>
#include <cefix/TextButtonWidget.h>
#include <cefix/HorizontalSliderWidget.h>
#include <cefix/TextWidget.h>
#include <cefix/ImageWidget.h>
#include <cefix/WidgetDefaults.h>
#include <cefix/SequenceGrabberManager.h>




namespace cefix {

// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

SequenceGrabberWidget::SequenceGrabberWidget(const std::string& identifier, const osg::Vec3& position, const osg::Vec2& previewSize) :
	cefix::TransformWidget(identifier, osg::Matrix::translate(position)),
	cefix::AbstractWidget::Responder(),
	_previewImage(NULL)
{
	createWidgets(previewSize);
	_ignoreValuesSet = false;
	startAutoDiscover();
}
		
		
// ----------------------------------------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------------------------------------

SequenceGrabberWidget::SequenceGrabberWidget(cefix::PropertyList* pl) :
	cefix::TransformWidget(pl->get("id")->asString(), osg::Matrix::translate(pl->get("position")->asVec3())),
	cefix::AbstractWidget::Responder(),
	_previewImage(NULL)
{
	createWidgets(pl->get("previewsize")->asVec2(), pl);
	_ignoreValuesSet = false;
	startAutoDiscover();
	
}


// ----------------------------------------------------------------------------------------------------------
// createWidgets
// ----------------------------------------------------------------------------------------------------------

void SequenceGrabberWidget::createWidgets(const osg::Vec2& size, cefix::PropertyList* prefs_pl) {

	osg::Vec3 p(0, 114-7+55+4, 0);
	_size = size;
	osg::ref_ptr<cefix::PropertyList> pl;
	
	pl = new cefix::PropertyList();
	pl->add("id", "preview_image");
	pl->add("size", size);
	pl->add("position", p);
	pl->add("zoomMode", "auto_proportional");
	if (prefs_pl && prefs_pl->hasKey("preview")) pl->addNonExistingKeys(prefs_pl->get("preview")->asPropertyList());
	osg::ref_ptr<ImageWidget> preview = WidgetFactory::instance()->createWidget<ImageWidget>("imageWidget", pl.get());
	preview->addResponder(this);
		
	add(preview);
	
	pl = new cefix::PropertyList();
	pl->add("id", "preview_selection");
	pl->add("position", p+osg::Vec3(0,0,10));
	pl->add("constrainingrect", osg::Vec4(-10,-10, size[0]+10, size[1]+10));
	pl->add("independentCorners", true);
	pl->add("topleft", osg::Vec2(0, 0));
	pl->add("topright", osg::Vec2(size[0], 0));
	pl->add("bottomright", osg::Vec2(size[0], size[1]));
	pl->add("bottomleft", osg::Vec2(0, size[1]));
	osg::ref_ptr<RectangleSelectionWidget> selection = WidgetFactory::instance()->createWidget<RectangleSelectionWidget>("rectangleselectionwidget", pl.get());
	add(selection);
	selection->addResponder(this);
	_rectangleSelectionWidget = selection;
	_rectangleSelectionWidget->setReferenceSize(size);
	
	
	{
		p[1] -= 20;
		std::string slider_captions_str("WhiteBalance|Contrast|Brightness|Gain|Shutter|Exposure|Focus");
		std::string slider_id_str("whitebalance contrast brightness gain shutter exposure focus");
		
		if (prefs_pl && prefs_pl->hasKey("sliderCaptions")) slider_captions_str = prefs_pl->get("sliderCaptions")->asString();
		
		std::vector<std::string> slider_ids, slider_captions;
		cefix::strTokenize(slider_id_str, slider_ids, " ");
		cefix::strTokenize(slider_captions_str, slider_captions, "|");
		
		float slider_left = 100;
		if (prefs_pl && prefs_pl->hasKey("sliderLeft")) slider_left = prefs_pl->get("sliderLeft")->asFloat();
		
		float slider_width = size[0] - slider_left - 40;
		if (prefs_pl && prefs_pl->hasKey("sliderWidth")) slider_width = prefs_pl->get("sliderWidth")->asFloat();
		
		for(std::vector<std::string>::iterator i = slider_ids.begin(), j = slider_captions.begin(); i != slider_ids.end(); ++i, ++j) {
			
			pl = new cefix::PropertyList();
			pl->add("id", (*i)+"_slider" );
			pl->add("size", osg::Vec2(slider_width, WidgetDefaults::getSliderHeight()));
			pl->add("throbbersize", WidgetDefaults::getSliderThrobberSize());
			pl->add("position", osg::Vec3(slider_left,p[1], p[2]));
			pl->add("min", -1);
			pl->add("max", 1000.0);
			pl->add("value", 0);
			
			if (prefs_pl && prefs_pl->hasKey("slider")) pl->addNonExistingKeys(prefs_pl->get("slider")->asPropertyList());
			
			osg::ref_ptr<HorizontalSliderWidget> slider = cefix::WidgetFactory::instance()->createWidget<HorizontalSliderWidget>("horizontalsliderwidget", pl.get());
			add(slider);
			slider->addResponder(this);
			
			pl = new cefix::PropertyList();
			pl->add("position", osg::Vec3(size[0], p[1], p[2]));
			pl->add("caption", "0.0");
			pl->add("alignment", "right");
			pl->add("resolution", 10);
			
			if (prefs_pl && prefs_pl->hasKey("valueCaptionPrefs")) pl->addNonExistingKeys(prefs_pl->get("valueCaptionPrefs")->asPropertyList());
			osg::ref_ptr<TextWidget> value_caption = cefix::WidgetFactory::instance()->createWidget<TextWidget>("TextWidget", pl.get());
			
			slider->addResponder(value_caption);
			add(value_caption);

			pl = new cefix::PropertyList();
			pl->add("position", osg::Vec3(0, p[1], p[2]));
			pl->add("caption", *j);
			
			
			if (prefs_pl && prefs_pl->hasKey("sliderCaptionPrefs")) pl->addNonExistingKeys(prefs_pl->get("sliderCaptionPrefs")->asPropertyList());
			osg::ref_ptr<TextWidget> slider_caption = cefix::WidgetFactory::instance()->createWidget<TextWidget>("TextWidget", pl.get());
			
			add(slider_caption);

			
			p[1] -= 20;

		}
	}
	
	{
		GroupWidget* img_btn_group = new cefix::GroupWidget("function_btn_group");
		
		p[1] -= 6;
		{
			std::string btns("show_settings_btn");
			std::string btn_caption_str("Show Settings");
			if (prefs_pl && prefs_pl->hasKey("modeBtnCaptions")) btn_caption_str = prefs_pl->get("modeBtnCaptions")->asString();
			
			std::vector<std::string> btn_ids, btn_captions;
			cefix::strTokenize(btns, btn_ids, " ");
			cefix::strTokenize(btn_caption_str, btn_captions, "|");
			
			float btn_width = (size[0]) ;
			float btn_height = WidgetDefaults::getButtonHeight();
			
			if (prefs_pl && prefs_pl->hasKey("btnWidth")) btn_width = prefs_pl->get("btnWidth")->asFloat();
			if (prefs_pl && prefs_pl->hasKey("btnHeight")) btn_height = prefs_pl->get("btnHeight")->asFloat();
			float dx = btn_width + 10;
			osg::Vec3 p2(p);
			for(std::vector<std::string>::iterator i = btn_ids.begin(), j = btn_captions.begin(); i != btn_ids.end(); ++i, ++j) {
				pl = new cefix::PropertyList();
				pl->add("id", (*i) );
				pl->add("size", osg::Vec2(btn_width, btn_height));
				pl->add("position", p2);
				pl->add("caption", *j);
				pl->add("selectable", 0);
				if (prefs_pl && prefs_pl->hasKey("modebtnPrefs")) pl->addNonExistingKeys(prefs_pl->get("modebtnPrefs")->asPropertyList());
				
				p2[0] += dx;
				
				osg::ref_ptr<TextButtonWidget> btn = cefix::WidgetFactory::instance()->createWidget<TextButtonWidget>("textbuttonwidget", pl.get());
				img_btn_group->add(btn);
				btn->addResponder(this);
			}
		}
		
		add(img_btn_group);
		
	}
	
	

}


// ----------------------------------------------------------------------------------------------------------
// respondToAction
// ----------------------------------------------------------------------------------------------------------

void SequenceGrabberWidget::respondToAction (const std::string &action, AbstractWidget *widget) 
{
	_ignoreValuesSet = true;
	if (action == SliderWidget::Actions::valueChanged()) 
	{
		HorizontalSliderWidget* slider = dynamic_cast<HorizontalSliderWidget*>(widget);
		if (slider && _sg.valid()) {
			float v = slider->getValue();
		    if(slider->getIdentifier() == "gain_slider") {
				_sg->setGain(v);
			} else if(slider->getIdentifier() == "exposure_slider") {
				_sg->setExposure(v);
			} else  if(slider->getIdentifier() == "focus_slider") {
				_sg->setFocus(v);
			} else  if(slider->getIdentifier() == "shutter_slider") {
				_sg->setShutter(v);
			} else  if(slider->getIdentifier() == "whitebalance_slider") {
				_sg->setWhiteBalance(v);
			} else  if(slider->getIdentifier() == "contrast_slider") {
				_sg->setContrast(v);
			} else  if(slider->getIdentifier() == "brightness_slider") {
				_sg->setBrightness(v);
			} 
		}
	} else if (action == ButtonWidget::Actions::stateChanged()) {
	
		ButtonWidget* btn = dynamic_cast<ButtonWidget*>(widget);
		if (btn && btn->getState() == AbstractWidget::DEPRESSED) {
			if (btn->getIdentifier() == "show_settings_btn") {
				_sg->showSettings();
				_ignoreValuesSet = false;
				updateValues();
			}
		}
	} else if (action == RectangleSelectionWidget::Actions::selectionChanged()) 
	{
		cefix::RectangleSelectionWidget* r_widget = dynamic_cast<cefix::RectangleSelectionWidget*>(widget);
		if (r_widget && _sg) _sg->setSelection(SequenceGrabber::Selection(r_widget->getNormalizedSelection(_sg->getImage()->s(), _sg->getImage()->t() )) );
		informAttachedResponder(Actions::selectionChanged());
	}
	_ignoreValuesSet = false;
}

void SequenceGrabberWidget::updateValues() 
{
	if (!_sg.valid() || _ignoreValuesSet) return;
		
	find<HorizontalSliderWidget*>("gain_slider")->setValue(_sg->getGain());
	find<HorizontalSliderWidget*>("exposure_slider")->setValue(_sg->getExposure());
	find<HorizontalSliderWidget*>("focus_slider")->setValue(_sg->getFocus());	
	find<HorizontalSliderWidget*>("shutter_slider")->setValue(_sg->getShutter());	
	
	find<HorizontalSliderWidget*>("whitebalance_slider")->setValue(_sg->getWhiteBalance());	
	find<HorizontalSliderWidget*>("contrast_slider")->setValue(_sg->getContrast());	
	find<HorizontalSliderWidget*>("brightness_slider")->setValue(_sg->getBrightness());	
	
	find<ImageWidget*>("preview_image")->setImage( _previewImage.valid() ? _previewImage.get() : _sg->getImage() );	
	
	_rectangleSelectionWidget->setNormalizedSelection(_sg->getSelection().asVec2Array(), 1.0 / _sg->getImage()->s(), 1.0 / _sg->getImage()->t());
	
}

void SequenceGrabberWidget::checkForAvailableSequenceGrabber() 
{
	if (getSequenceGrabber() != NULL)
		return;
		
	cefix::SequenceGrabber* device = cefix::SequenceGrabberManager::instance()->getLastAcquiredDevice();
	if (device)
		setSequenceGrabber(device);
	else
		startAutoDiscover();
}



}
