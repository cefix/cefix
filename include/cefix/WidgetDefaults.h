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

#ifndef WIDGET_DEFAULTS_HEADER
#define WIDGET_DEFAULTS_HEADER

#include <osg/Vec2>
#include <cefix/HSliderWithCaptionAndValueWidget.h>


namespace cefix {

class WidgetDefaults {
public:
	static const osg::Vec2& getSliderThrobberSize() { return _sliderThrobberSize; }
	static float getButtonHeight() { return _buttonHeight; }
	static float getSliderHeight() { return _sliderHeight; }
	static float getProgressBarHeight() { return _progressbarHeight; }
	
	static const osg::Vec2& getCheckboxSize() { return _checkboxSize; }
	static const osg::Vec2& getRadioButtonSize() { return _radiobuttonSize; }
	
	static const osg::Vec2 getHSliderCaptionDelta() { return _hsliderCaptionDelta; }
	static const osg::Vec2 getHSliderValueDelta() { return _hsliderValueDelta; }
	
	static void setSliderThrobberSize(const osg::Vec2& s) { _sliderThrobberSize = s; }
	static void setButtonHeight(float f) { _buttonHeight = f; }
	static void setSliderHeight(float f) { _sliderHeight = f; }
	static void setProgressBarHeight(float f) { _progressbarHeight = f; }
	
	static void setCheckboxSize(const osg::Vec2& s) {_checkboxSize = s; }
	static void setRadioButtonSize(const osg::Vec2& s) {_radiobuttonSize = s; }
	
	static void setHSliderCaptionDelta(const osg::Vec2& p) { _hsliderCaptionDelta = p; }
	static void setHSliderValueDelta(const osg::Vec2& p) {  _hsliderValueDelta = p; }
	
	static HSliderWithCaptionAndValueWidget::Style getHSliderWithCaptionAndValueWidgetStyle() { return _hSliderWithCaptionAndValueWidgetStyle; }
	static void setHSliderWithCaptionAndValueWidgetStyle(HSliderWithCaptionAndValueWidget::Style style) { _hSliderWithCaptionAndValueWidgetStyle = style; }
	
	static float getTextInputHeight()  { return _textInputHeight; }
	static void  setTextInputHeight(float f) { _textInputHeight = f; }
	
private:
	static osg::Vec2 _sliderThrobberSize;
	static float _buttonHeight;
	static float _sliderHeight;
	static osg::Vec2 _checkboxSize;
	static osg::Vec2 _radiobuttonSize;
	static float _progressbarHeight;
	static osg::Vec2 _hsliderCaptionDelta;
	static osg::Vec2 _hsliderValueDelta;
	static float _textInputHeight;
	
	static HSliderWithCaptionAndValueWidget::Style _hSliderWithCaptionAndValueWidgetStyle;
	
};

}

#endif