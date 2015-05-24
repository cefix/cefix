/*
 *  ParamsWidgetHelper.cpp
 *  boids
 *
 *  Created by Stephan Huber on 09.11.10.
 *  Copyright 2010 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "WidgetComposer.h"
#include <cefix/WidgetFactory.h>
#include <cefix/WidgetDefaults.h>

namespace cefix {

cefix::HorizontalSliderWidget* WidgetComposer::addSlider(const std::string& caption, const cefix::doubleRange& range, AbstractWidget::Responder* resp )
{
    osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
    pl->add("id", caption+ " slider");
    pl->add("caption", caption);
    pl->add("position", _topLeft);
    pl->add("size", osg::Vec2(_width, cefix::WidgetDefaults::getSliderHeight()));
    pl->add("min", range.min());
    pl->add("max", range.max());
    pl->add("throbberSize", cefix::WidgetDefaults::getSliderThrobberSize ());
    
    osg::ref_ptr<cefix::HSliderWithCaptionAndValueWidget> widget = cefix::WidgetFactory::instance()->createWidget<cefix::HSliderWithCaptionAndValueWidget>("HSliderWithCaptionAndValueWidget", pl.get());
    _group->add(widget);

    advance( cefix::WidgetDefaults::getSliderHeight() + _deltaY);
    if (widget && resp) widget->addResponder(resp);
    
    return (widget) ? widget->getSlider() : NULL;
}



cefix::ButtonWidget*  WidgetComposer::addButton(const std::string& caption, AbstractWidget::Responder* resp) 
{    
    osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
    pl->add("id", caption+ " button");
    pl->add("caption", caption);
    pl->add("position", _topLeft);
    pl->add("size", osg::Vec2(_width, cefix::WidgetDefaults::getButtonHeight()));
    
    osg::ref_ptr<cefix::ButtonWidget> widget = cefix::WidgetFactory::instance()->createWidget<cefix::ButtonWidget>("TextButtonWidget", pl.get());
    _group->add(widget);
    
    
    advance( cefix::WidgetDefaults::getButtonHeight() + _deltaY );
    
    if (widget && resp) widget->addResponder(resp);
    return widget.get();

}

cefix::CheckboxWidget*  WidgetComposer::addCheckbox(const std::string& caption, AbstractWidget::Responder* resp) 
{    
    osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
    pl->add("id", caption+ " checkbox");
    pl->add("caption", caption);
    pl->add("position", _topLeft);
    pl->add("size", cefix::WidgetDefaults::getCheckboxSize() );
    
    osg::ref_ptr<cefix::CheckboxWidget> widget = cefix::WidgetFactory::instance()->createWidget<cefix::CheckboxWidget>("CheckboxWidget", pl.get());
    _group->add(widget);
    
    
    advance( cefix::WidgetDefaults::getCheckboxSize()[1] + _deltaY );
    
    if (widget && resp) widget->addResponder(resp);
    return widget.get();
}

cefix::TextInputWidget* WidgetComposer::addTextInput(const std::string& id, AbstractWidget::Responder* resp)
{
	osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
    pl->add("id", id);
    pl->add("position", _topLeft);
	pl->add("value", "");
    pl->add("size", osg::Vec2(_width, WidgetDefaults::getTextInputHeight()) );
    pl->add("inset", osg::Vec2(3,3));
    osg::ref_ptr<cefix::TextInputWidget> widget = cefix::WidgetFactory::instance()->createWidget<cefix::TextInputWidget>("TextInputWidget", pl.get());
    _group->add(widget);
    
    
    advance( WidgetDefaults::getTextInputHeight() + _deltaY );
    
    if (widget && resp) widget->addResponder(resp);
    return widget.get();
}


cefix::ProgressBarWidget* WidgetComposer::addProgressBar(const std::string& id, AbstractWidget::Responder* resp) 
{
    osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
    pl->add("id", id);
    pl->add("position", _topLeft);
    pl->add("size", osg::Vec2(_width, WidgetDefaults::getProgressBarHeight()) );
    
    osg::ref_ptr<cefix::ProgressBarWidget> widget = cefix::WidgetFactory::instance()->createWidget<cefix::ProgressBarWidget>("ProgressBarWidget", pl.get());
    _group->add(widget);
    
    
    advance( WidgetDefaults::getProgressBarHeight() + _deltaY );
    
    if (widget && resp) widget->addResponder(resp);
    return widget.get();
}

cefix::DrawerWidget* WidgetComposer::createDrawer(const std::string& caption, const osg::Vec3& position, bool expanded) 
{

	osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
    pl->add("id", caption+ " drawer");
    pl->add("caption", caption);
    pl->add("expandedposition", position);
	pl->add("collapsedposition", position-osg::Vec3(0, 20+_height,0));
	pl->add("size", osg::Vec2(20+_width, 20+_height));
	pl->add("expanded", expanded);
	pl->add("tabPosition", "top");
	osg::ref_ptr<cefix::DrawerWidget> drawer = cefix::WidgetFactory::instance()->createWidget<cefix::DrawerWidget>("drawerWidget", pl);
	if (drawer) {
		for(GroupWidget::iterator i = _group->begin(); i != _group->end(); ++i) {
            drawer->add(i->second);
        }
		drawer->update();
		if (!expanded) {
			drawer->collapse();
		} else {
			drawer->expand();
		}

	}
	return drawer.release();
}

cefix::BoxWidget* WidgetComposer::createBox(const std::string& caption, const osg::Vec3& position) 
{
	osg::ref_ptr<cefix::PropertyList> pl = new cefix::PropertyList();
    pl->add("id", caption+ " box");
    pl->add("caption", caption);
    pl->add("size", osg::Vec2(20+_width, 20+_height));
	pl->add("position", position);
	osg::ref_ptr<cefix::BoxWidget> box = cefix::WidgetFactory::instance()->createWidget<cefix::BoxWidget>("boxWidget", pl);
	if (box) {
		for(GroupWidget::iterator i = _group->begin(); i != _group->end(); ++i) {
            box->add(i->second);
        }
		box->update();

	}
	return box.release();
}

}