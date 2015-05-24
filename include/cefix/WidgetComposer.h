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

#ifndef PARAMS_WIDGET_COMPOSER_HEADER
#define PARAMS_WIDGET_COMPOSER_HEADER

#include <cefix/HorizontalSliderWidget.h>
#include <cefix/ButtonWidget.h>
#include <cefix/GroupWidget.h>
#include <cefix/DrawerWidget.h>
#include <cefix/CheckboxWidget.h>
#include <cefix/ProgressBarWidget.h>
#include <cefix/TextInputWidget.h>
#include <cefix/BoxWidget.h>

namespace cefix {

class WidgetComposer {

public:
    WidgetComposer(const osg::Vec3& p, float width, float direction = -1) 
	:	_topLeft(p), 
		_direction(direction), 
		_width(width), 
		_height(0), 
		_deltaY(10) 
	{ 
		_group = new GroupWidget("params"); 
	}
    
    void setTopLeft(const osg::Vec3& tl) { _topLeft = tl; }
    void setWidth(float w) { _width = w; }
    void setDirection(float d) { _direction = d; }
    
    HorizontalSliderWidget* addSlider(const std::string& caption, const doubleRange& range, AbstractWidget::Responder* resp = NULL);
    
    template<typename T>HorizontalSliderWidget* addSlider(const std::string& caption, const doubleRange& range, T* value) 
    {
        HorizontalSliderWidget* widget = addSlider(caption, range);
        if (widget) widget->addResponder(AbstractWidget::createResponderFromValue<HorizontalSliderWidget, T>(value));
        return widget;

    }
    
    template<class T, class U>
    HorizontalSliderWidget* addSlider(const std::string& caption, const doubleRange& range, T* t, U(T::*getter)(), void(T::*setter)(U u) ) {
        HorizontalSliderWidget* w = addSlider(caption, range);
        if (w) w->addResponder(AbstractWidget::createResponderFromGetterSetter<HorizontalSliderWidget, T, U>(t, getter, setter));
        
        return w;
    }
	
	template<class T, class U>
    HorizontalSliderWidget* addSlider(const std::string& caption, const doubleRange& range, T* t, U(T::*getter)() const, void(T::*setter)(U u) ) {
        HorizontalSliderWidget* w = addSlider(caption, range);
        if (w) w->addResponder(AbstractWidget::createResponderFromGetterSetter<HorizontalSliderWidget, T, U>(t, getter, setter));
        
        return w;
    }
        
    ButtonWidget*  addButton(const std::string& caption, AbstractWidget::Responder* resp = NULL);
	
	template <class T>ButtonWidget*  addButton(const std::string& caption, AbstractWidget::States state, T* t,  void (T::*fpt)() ) {
		ButtonWidget* widget = addButton(caption);
		if (widget) widget->addResponder(ButtonWidget::createResponder(state, t, fpt));
		return widget;
	}
    
    template<class T, class U>
    ButtonWidget* addButton(const std::string& caption, T* t, U(T::*getter)(), void(T::*setter)(U u) ) {
        ButtonWidget* w = addButton(caption);
        if (w) w->addResponder(AbstractWidget::createResponderFromGetterSetter<ButtonWidget, T, U>(t, getter, setter));
        
        return w;
    }
	
	template<class T, class U>
    ButtonWidget* addButton(const std::string& caption, T* t, U(T::*getter)() const, void(T::*setter)(U u) ) {
        ButtonWidget* w = addButton(caption);
        if (w) w->addResponder(AbstractWidget::createResponderFromGetterSetter<ButtonWidget, T, U>(t, getter, setter));
        
        return w;
    }
	
	CheckboxWidget*  addCheckbox(const std::string& caption, AbstractWidget::Responder* resp = NULL);
	
	CheckboxWidget*  addCheckbox(const std::string& caption, bool* value ) {
		CheckboxWidget* widget = addCheckbox(caption);
		if (widget) widget->addResponder(AbstractWidget::createResponderFromValue<CheckboxWidget, bool>(value));
		return widget;
	}
	
	template<class T>
    CheckboxWidget* addCheckbox(const std::string& caption, T* t, bool(T::*getter)(), void(T::*setter)(bool) ) {
        CheckboxWidget* w = addCheckbox(
			caption, 
			AbstractWidget::createResponderFromGetterSetter<CheckboxWidget, T, bool>(t, getter, setter)
		);
        
        return w;
    }
	
	TextInputWidget* addTextInput(const std::string& widget_id, AbstractWidget::Responder* resp = NULL);
	
	TextInputWidget* addTextInput(const std::string& widget_id, std::wstring* value)
	{
		TextInputWidget* widget = addTextInput(widget_id);
		if (widget) widget->addResponder(AbstractWidget::createResponderFromValue<TextInputWidget, std::wstring>(value));
		return widget;

	}
	
	template<class T>
	TextInputWidget* addTextInput(const std::string& widget_id, T* t, std::wstring(T::*getter)(), void(T::*setter)(std::wstring) )
	{
		TextInputWidget* widget = addTextInput(widget_id);
		if (widget) widget->addResponder(AbstractWidget::createResponderFromGetterSetter<TextInputWidget, T, std::wstring>(t, getter, setter));
		return widget;
	}
	
	template<class T>
	TextInputWidget* addTextInput(const std::string& widget_id, T* t, std::wstring(T::*getter)() const, void(T::*setter)(std::wstring) )
	{
		TextInputWidget* widget = addTextInput(widget_id);
		if (widget) widget->addResponder(AbstractWidget::createResponderFromGetterSetter<TextInputWidget, T, std::wstring>(t, getter, setter));
		return widget;
	}
    
    ProgressBarWidget* addProgressBar(const std::string& id, AbstractWidget::Responder* resp = NULL);
    
    GroupWidget* getWidgets() { _group->update(); return _group.get(); }
    
    void advance(float f) { _height += f; _topLeft += osg::Vec3(0, osg::round(_direction * f), 0); }
    
	float getHeight() const { return _height; }
    float getWidth() const { return _width; }
	
	DrawerWidget* createDrawer(const std::string& caption, const osg::Vec3& position, bool expanded = false);
	BoxWidget* createBox(const std::string& caption, const osg::Vec3& position);
	
    void setDeltaY(float f) { _deltaY = f; }
    
    void addWidget(cefix::AbstractWidget* widget, float height) { _group->add(widget); advance(height + _deltaY); }
    
    const osg::Vec3& getTopLeft() const { return _topLeft; }
private:
    osg::ref_ptr<GroupWidget> _group;
    osg::Vec3 _topLeft;
    float _direction, _width, _height;
	float _deltaY;

};

}


#endif