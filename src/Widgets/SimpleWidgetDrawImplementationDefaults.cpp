/*
 *  SimpleWidgetDrawImplementationDefaults.cpp
 *  cefix
 *
 *  Created by Stephan Huber on 10.11.10.
 *  Copyright 2010 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "SimpleWidgetDrawImplementationDefaults.h"

namespace cefix {


SimpleWidgetDrawImplementationDefaults::SimpleWidgetDrawImplementationDefaults() 
:   osg::Referenced()
{
    setBtnBgColorFor( AbstractWidget::DISABLED,         osg::Vec4(0.5, 0.5, 0.5, 0.2) );
	setBtnBgColorFor( AbstractWidget::NORMAL,			osg::Vec4(0.5, 0.5, 0.5, 0.5) );
	setBtnBgColorFor( AbstractWidget::NORMAL_OVER,      osg::Vec4(0.5, 0.5, 0.5, 0.7) );
	setBtnBgColorFor( AbstractWidget::SELECTED,         osg::Vec4(0.7, 0.7, 0.7, 0.5) );
	setBtnBgColorFor( AbstractWidget::SELECTED_OVER,	osg::Vec4(0.7, 0.7, 0.7, 0.7) );
	setBtnBgColorFor( AbstractWidget::PRESSED,          osg::Vec4(1.0, 1.0, 1.0, 0.7) );
    
    setBtnTextColorFor( AbstractWidget::DISABLED,			osg::Vec4(0.5, 0.5, 0.5, 0.5) );
	setBtnTextColorFor( AbstractWidget::NORMAL,             osg::Vec4(0.7, 0.7, 0.7, 0.5) );
	setBtnTextColorFor( AbstractWidget::NORMAL_OVER,		osg::Vec4(0.7, 0.7, 0.7, 0.7) );
	
	setBtnTextColorFor( AbstractWidget::SELECTED,			osg::Vec4(1.0, 1.0, 1.0, 0.5) );
	setBtnTextColorFor( AbstractWidget::SELECTED_OVER,      osg::Vec4(1.0, 1.0, 1.0, 0.7) );
	setBtnTextColorFor( AbstractWidget::PRESSED,			osg::Vec4(1.0, 1.0, 1.0, 1.0) );
    
    _slider.backgroundFillColor = osg::Vec4(0.5,0.5,0.5,0.5);
    _slider.backgroundStrokeColor = osg::Vec4(0.5,0.5,0.5,0.7);
    _slider.throbberFillColor = osg::Vec4(0.5,0.5,0.5,0.5);
    _slider.throbberStrokeColor = osg::Vec4(1.0,1.0,1.0,0.7);
    
    _slider.disabledFillColor = osg::Vec4(0.5,0.5,0.5,0.2);
    _slider.disabledStrokeColor = osg::Vec4(0.5,0.5,0.5,0.2);
    
    setBtnFontName("system.xml");
    setBtnFontSize(0);
    setBtnTextAlignment(cefix::Utf8TextGeode::RenderIterator::ALIGN_CENTER);
    
    setBtnTextOffset(osg::Vec2(0,0));
}

SimpleWidgetDrawImplementationDefaults* SimpleWidgetDrawImplementationDefaults::instance() 
{
    static osg::ref_ptr<SimpleWidgetDrawImplementationDefaults> s_ptr = new SimpleWidgetDrawImplementationDefaults();
    return s_ptr.get();
}


}