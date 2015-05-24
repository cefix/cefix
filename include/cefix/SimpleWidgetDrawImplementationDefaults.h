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

#ifndef SIMPLE_WIDGET_DRAW_IMPLEMENTATION_DEFAULTS_HEADER
#define SIMPLE_WIDGET_DRAW_IMPLEMENTATION_DEFAULTS_HEADER

#include <cefix/AbstractWidget.h>
#include <cefix/TextGeode.h>

namespace cefix {


class SimpleWidgetDrawImplementationDefaults : public osg::Referenced {

public:
   static SimpleWidgetDrawImplementationDefaults* instance();
   
   SimpleWidgetDrawImplementationDefaults();
   
   const osg::Vec4& getBtnBgColorFor(cefix::AbstractWidget::States state) { return _buttonBackgroundColors[state]; }
   void setBtnBgColorFor(cefix::AbstractWidget::States state, const osg::Vec4& color) { _buttonBackgroundColors[state] = color; }
   
    const osg::Vec4& getBtnTextColorFor(cefix::AbstractWidget::States state) { return _buttonTextColors[state]; }
   void setBtnTextColorFor(cefix::AbstractWidget::States state, const osg::Vec4& color) { _buttonTextColors[state] = color; }
   
   const osg::Vec4& getSliderBackgroundFillColor()      { return _slider.backgroundFillColor; }
   const osg::Vec4& getSliderBackgroundStrokeColor()    { return _slider.backgroundStrokeColor; }
   const osg::Vec4& getSliderThrobberFillColor()        { return _slider.throbberFillColor; }
   const osg::Vec4& getSliderThrobberStrokeColor()      { return _slider.throbberStrokeColor; }
   const osg::Vec4& getSliderDisabledFillColor()        { return _slider.disabledFillColor; }
   const osg::Vec4& getSliderDisabledStrokeColor()      { return _slider.disabledStrokeColor; }
   
   void setSliderBackgroundFillColor(const osg::Vec4& c)      { _slider.backgroundFillColor = c; }
   void setSliderBackgroundStrokeColor(const osg::Vec4& c)    { _slider.backgroundStrokeColor = c; }
   void setSliderThrobberFillColor(const osg::Vec4& c)        { _slider.throbberFillColor = c; }
   void setSliderThrobberStrokeColor(const osg::Vec4& c)      { _slider.throbberStrokeColor = c; }
   void setSliderDisabledFillColor(const osg::Vec4& c)        { _slider.disabledFillColor = c; }
   void setSliderDisabledStrokeColor(const osg::Vec4& c)      { _slider.disabledStrokeColor = c; }
   
   const std::string& getBtnFontName() { return _btnFontName; }
   float getBtnFontSize() { return _btnFontSize; }
   cefix::Utf8TextGeode::RenderIterator::TextAlignment getBtnTextAlignment() { return _btnTextAlignment; }
   const osg::Vec2& getBtnTextOffset() { return _btnTextOffset; }
   
   
   void setBtnFontName(const std::string& n) { _btnFontName = n; }
   void setBtnFontSize(float f) { _btnFontSize = f; }
   void setBtnTextAlignment(cefix::Utf8TextGeode::RenderIterator::TextAlignment a) { _btnTextAlignment = a; }
   void setBtnTextOffset(const osg::Vec2& o) { _btnTextOffset = o; }
   
private:
    std::map<cefix::AbstractWidget::States, osg::Vec4> _buttonBackgroundColors, _buttonTextColors;
    struct Slider {
        osg::Vec4 backgroundFillColor, backgroundStrokeColor,throbberFillColor, throbberStrokeColor, disabledFillColor, disabledStrokeColor;
    };
    
    Slider _slider;
    
    std::string _btnFontName;
    float _btnFontSize;
    cefix::Utf8TextGeode::RenderIterator::TextAlignment _btnTextAlignment;
    osg::Vec2 _btnTextOffset;
    
};


}
#endif