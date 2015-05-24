/*
 *
 *      (\(\
 *     (='.')           cefix --
 *    o(_")")           a multipurpose library on top of OpenSceneGraph
 *  
 *
 *                      Copyright 2001-2011, stephanmaximilianhuber.com 
 *                      Stephan Maximilian Huber. 
 *
 *                      All rights reserved.
 *
 */

#ifndef _HSLIDERWITHCAPTIONANDVALUEWIDGET_H_
#define _HSLIDERWITHCAPTIONANDVALUEWIDGET_H_

#include <cefix/TransformWidget.h>
#include <cefix/HorizontalSliderWidget.h>

namespace cefix
{
	class HSliderWithCaptionAndValueWidget: public TransformWidget, public AbstractWidget::Responder
	{
	public:
		class Responder : public virtual AbstractWidget::Responder {
		
			public:
				Responder(): AbstractWidget::Responder() {}
				
				virtual void sliderValueChanged() {}
				
				virtual void respondToAction(const std::string& action, AbstractWidget* widget) {
					_slider = dynamic_cast<HSliderWithCaptionAndValueWidget*>(widget);
					if (_slider && (action=="valuechanged"))
						sliderValueChanged();
					
				}
			protected:
				HSliderWithCaptionAndValueWidget* getSlider() { return _slider; }
			private:
				HSliderWithCaptionAndValueWidget* _slider;
		};
		
		enum Style {CAPTION_AND_VALUE_INSIDE, CAPTION_AND_VALUE_OUTSIDE, CAPTION_AND_VALUE_RIGHT, UNKNOWN};
		
		HSliderWithCaptionAndValueWidget(
			const std::string& id, 
			const std::string& caption, 
			const osg::Vec3& pos, 
			const osg::Vec2& size, 
			const osg::Vec2& throbbersize, 
			cefix::floatRange range,
			Style style = UNKNOWN, 
			const std::string& fontname="system.xml", 
			float fontsize=0, 
			AbstractWidget::Responder* responder = NULL
		);
		
		HSliderWithCaptionAndValueWidget(cefix::PropertyList* pl);
		
		virtual void respondToAction(const std::string& action, AbstractWidget* widget);
	
		float getValue() { return _slider->getValue(); }
		
		void setValue(float value) { _slider->setValue(value); }
		
		void setResolution(unsigned int res) { _slider->setResolution(res); }
		
		void setCaption(const std::string& caption);
		
		void setRange(const cefix::doubleRange& r) { _slider->setRange(r); } 
		
        HorizontalSliderWidget* getSlider() { return _slider; }
        
	protected:
		void createWidgets(const std::string& id, const std::string& caption, const osg::Vec2& size, const osg::Vec2& throbberSize, const std::string& widgetprefskey = "");

		virtual ~HSliderWithCaptionAndValueWidget () {}
	private:
		cefix::floatRange	_range;
		Style		_style;
		std::string _fontname;
		float 		_fontsize;
		osg::Vec2	_inset;
		float		_value;
		HorizontalSliderWidget* _slider;
		float _sliderStartX, _sliderWidth;
		
	};
} /* cefix */


#endif /* _HSLIDERWITHCAPTIONANDVALUEWIDGET_H_ */
