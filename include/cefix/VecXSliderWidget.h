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

#ifndef _VECXSLIDERWIDGET_H_
#define _VECXSLIDERWIDGET_H_

#include <cefix/TransformWidget.h>
#include <cefix/MathUtils.h>
#include <cefix/WidgetFactory.h>
#include <cefix/WidgetDefaults.h>

#include "HSliderWithCaptionAndValueWidget.h"

namespace cefix
{
	template<class VecType, int NumSliders>class VecXSliderWidget : public cefix::TransformWidget, public cefix::AbstractWidget::Responder
	{
	public:
		VecXSliderWidget(const std::string& id, const std::string& captions, const osg::Vec3& pos, const osg::Vec2& size, float dy)
		:  	TransformWidget(id, osg::Matrix::translate(pos)),
			_startX(-1),
			_sliderWidth(-1),
			_style("outside")
		{
			_throbberSize = WidgetDefaults::getSliderThrobberSize();
			createWidgets(id, captions, pos, size, dy);
		}
		
		
		VecXSliderWidget(PropertyList* pl)
		:	TransformWidget(pl),
			_startX(-1),
			_sliderWidth(-1),
			_style("outside")
		{
			osg::Vec2 size = pl->get("size")->asVec2();
			float dy = pl->get("deltaY")->asFloat();
			if (pl->hasKey("throbberSize"))
				_throbberSize = pl->get("throbberSize")->asVec2();
			else
				_throbberSize = WidgetDefaults::getSliderThrobberSize();
			if (pl->hasKey("sliderstartx")) _startX = pl->get("sliderstartx")->asFloat();
			if (pl->hasKey("sliderwidth")) _sliderWidth = pl->get("sliderwidth")->asFloat();
			if (pl->hasKey("style")) _style = pl->get("style")->asString();
			createWidgets(getIdentifier(), pl->get("captions")->asString(), pl->get("position")->asVec3(), size, dy, pl->get("prefskey")->asString());
			
			if (pl->hasKey("value")) {
				cefix::PropertyT<VecType>* t = dynamic_cast< cefix::PropertyT<VecType>* >(pl->get("value"));
				if (t) setValue(t->data());
			}
			
			if (pl->hasKey("resolution"))
				setResolution(pl->get("resolution")->asFloat());
				
			if (pl->hasKey("min") && pl->hasKey("max")) {
				setRange(doubleRange(pl->get("min")->asDouble(), pl->get("max")->asDouble()));
			}
		}
		
		void setValue(const VecType& type)
		{
			for (unsigned int i = 0; i < _sliders.size(); ++i) {
				_sliders[i]->setValue( type[i] );
			}
		}
		
		VecType getValue()
		{
			VecType result;
			for (unsigned int i = 0; i < _sliders.size(); ++i) {
				result[i] = _sliders[i]->getValue();
			}
			return result;	
		}
		
		void setRange(const cefix::doubleRange& r) {
			for(unsigned int i= 0; i < _sliders.size(); ++i) {
				_sliders[i]->setRange(r);
			}
		}
		
		void setResolution(unsigned int r) {
			for(unsigned int i= 0; i < _sliders.size(); ++i) {
				_sliders[i]->setResolution(r);
			}
		}
		
		void setCaptions(const std::string& s) {
			std::vector<std::string> captions;
			cefix::strTokenize(s, captions, "|");
			for(unsigned int i = 0; i < _sliders.size(); ++i) 
			{
				_sliders[i]->setCaption(captions[i]);
			}
		}

		virtual void respondToAction(const std::string& action, AbstractWidget* widget) {
			if (action == "valuechanged")
				informAttachedResponder(action);
		}
	
	protected:
		void createWidgets(const std::string& id, const std::string& in_captions, const osg::Vec3& pos, const osg::Vec2& size, float dy, const std::string& prefskey = "")
		{
			cefix::WidgetFactory* wf = cefix::WidgetFactory::instance();
			std::vector<std::string> captions;
			strTokenize(in_captions, captions, "|");
			std::string def_captions[4] = {"X:", "Y:", "Z:", "W:"};

			float height = size[1];
			osg::Vec3 p(pos + osg::Vec3(0,(NumSliders-1) * (dy+height),0));
			for(unsigned int i = 0; i < NumSliders; ++i) 
			{
				osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
				pl->set("caption", (i<captions.size()) ? captions[i] : def_captions[i]);
				pl->set("position", p);
				pl->set("style", _style);
				//std::cout << p <<std::endl;
				pl->set("size", osg::Vec2(size[0], height));
				pl->set("throbbersize", _throbberSize);
				pl->set("min", 0.0f);
				pl->set("max", 255.0f);
				if (_sliderWidth >= 0) 
					pl->set("sliderwidth", _sliderWidth);
				if (_startX >= 0) 
					pl->set("sliderstartx", _startX);
					
						
				osg::ref_ptr<HSliderWithCaptionAndValueWidget> s = wf->createWidget<HSliderWithCaptionAndValueWidget>("horizontalsliderwithcaptionandvaluewidget", pl.get());
				_sliders.push_back(s);
				s->addResponder(this);
				add(s);		
				p[1] -= dy+height;	
			}

		}
		

	private:
		std::vector< HSliderWithCaptionAndValueWidget* > _sliders;
		osg::Vec2 _throbberSize;
		float _startX, _sliderWidth;
		std::string _style;
	};
	
	
	
	
	typedef VecXSliderWidget<osg::Vec2, 2> Vec2SliderWidget;
	typedef VecXSliderWidget<osg::Vec3, 3> Vec3SliderWidget;
	typedef VecXSliderWidget<osg::Vec4, 4> Vec4SliderWidget;
	
} /* cefix */


#endif /* _VECXSLIDERWIDGET_H_ */
